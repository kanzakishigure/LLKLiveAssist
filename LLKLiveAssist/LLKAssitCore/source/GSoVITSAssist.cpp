#include "GSoVITSAssist.h"
#include "Core/ErrorCode.h"
#include "Core/logger.h"
#include "Data/GSoVITSModel.h"
#include "Data/JsonParser.h"
#include "ModuleManager.h"
#include "Net/HttpRequest.h"
#include "Net/TCPClient.h"

#include <boost/json.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serializer.hpp>
#include <boost/process/v1/detail/child_decl.hpp>
#include <boost/process/v1/io.hpp>
#include <boost/process/v1/start_dir.hpp>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdint.h>
#include <string>
#include <system_error>
#include <vector>

namespace NAssist {

const std::string request_url = "http://127.0.0.1:9880";
const std::string request_host = "127.0.0.1";
const uint64_t request_port = 9880;
const std::string inference_endpoint = "/tts";
const std::string control_endpoint = "/control";
const std::string set_gpt_weights_endpoint = "/set_gpt_weights";
const std::string set_sovits_weights_endpoint = "/set_sovits_weights";

void GSoVITSAssist::init() {
  // load model data
  if (std::filesystem::exists(MODEL_DATA_PATH)) {
    std::ifstream istream(MODEL_DATA_PATH);
    using Iterator = std::istreambuf_iterator<char>;
    std::string content(Iterator{istream}, Iterator{});
    istream.close();

    if (!content.empty()) {
      auto jv = boost::json::parse(content);
      if (jv.is_array()) {
        for (const auto &value : jv.as_array()) {
          auto result = Parser<GSoVITSModel>::parse(value);
          if (result.index() == 1) {

            CORE_ERROR_TAG("GSoVITSAssist", "GSoVITSAssist load model fail!");
          }
          m_GSoVITSModels.emplace_back() = get<0>(result);
        }

      } else {
        auto result = Parser<GSoVITSModel>::parse(jv);
        if (result.index() == 1) {

          CORE_ERROR_TAG("GSoVITSAssist", "GSoVITSAssist load model fail!");
        }
        m_GSoVITSModels.emplace_back() = get<0>(result);
      }
    }
  }
  if (std::filesystem::exists(MODEL_CONFIG_PATH)) {
    std::ifstream istream(MODEL_CONFIG_PATH);
    using Iterator = std::istreambuf_iterator<char>;
    std::string content(Iterator{istream}, Iterator{});
    istream.close();

    if (!m_GSoVITSModels.empty()) {
      m_RequestGSoVITSModel = m_GSoVITSModels.front();
    }
    if (!content.empty()) {
      auto jv = boost::json::parse(content);
      if (!jv.is_null())
        if (!jv.at("default_model").is_null()) {
          int index = jv.at("default_model").as_int64();
          if (index < m_GSoVITSModels.size() && index > 0) {
            m_RequestGSoVITSModel = m_GSoVITSModels.at(index);
          }
        }
    }
  }
  CORE_INFO_TAG("GSoVITSAssist", "GSoVITSAssist init success");
}

void GSoVITSAssist::pushAudioStream(std::vector<uint8_t> bytes_stream) {
  std::lock_guard<std::mutex> lg(m_audio_mutex);
  m_audio_buffers.push(bytes_stream);
}
std::vector<uint8_t> GSoVITSAssist::popAudioSteam() {
  std::lock_guard<std::mutex> lg(m_audio_mutex);
  std::vector<uint8_t> result;
  if (!m_audio_buffers.empty()) {
    result = m_audio_buffers.front();
    m_audio_buffers.pop();
  }
  return result;
}
std::error_code GSoVITSAssist::start() {
  // 启动gpt-sovits实例

  {

#ifdef LLKDebug
    std::string gps_sovist_args = std::format(
        "{0}runtime/python.exe {0}api_v2.py -a {1} -p {2} -c "
        "{0}GPT_SoVITS/configs/tts_infer.yaml",
        GPT_SOVITS_ROOT, request_host, std::to_string(request_port));
#endif
#ifdef LLKRelease
    std::string gps_sovist_args = std::format(
        "{0}runtime/python.exe api_v2.py -a {1} -p {2} -c "
        "GPT_SoVITS/configs/tts_infer.yaml",
        GPT_SOVITS_ROOT, request_host, std::to_string(request_port));
#endif
    std::error_code ec;

    CORE_TRACE_TAG("GSoVITSAssist", "gps_sovist_args is {}", gps_sovist_args);

    m_gpt_sovist_process = std::make_unique<boost::process::child>(
        gps_sovist_args, boost::process::start_dir(GPT_SOVITS_ROOT), ec);

    if (ec) {
      return make_error_code(gpt_sovits_errc::gsovist_not_found);
    }

    // 尝试使用tcp连接服务
    TCPClient tcp(request_host, request_port);
    while (true) {
      if (tcp.try_connect()) {
        break;
      }
    }
    CORE_INFO_TAG("GSoVITSAssist", "gpt-sovist start success");
  }
  // set the sovits model and gpt model
  {
    std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
        request_url, set_gpt_weights_endpoint, HttpRequestMethod::get);
    request->AddRequestParameter("weights_path",
                                 m_RequestGSoVITSModel.gpt_weights);
    CORE_INFO_TAG("GSoVITSAssist", "set_gpt_weights :{}", request->Receive());
  }

  {
    std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
        request_url, set_sovits_weights_endpoint, HttpRequestMethod::get);
    request->AddRequestParameter("weights_path",
                                 m_RequestGSoVITSModel.sovits_weights);

    CORE_INFO_TAG("GSoVITSAssist", "set_sovits_weights :{}",
                  request->Receive());
  }
  // generate request body use data from model
  m_request_body.text_lang = "zh";
  m_request_body.ref_audio_path = m_RequestGSoVITSModel.ref_audio_path;
  m_request_body.prompt_text = m_RequestGSoVITSModel.prompt_text;
  m_request_body.prompt_lang = m_RequestGSoVITSModel.prompt_lang;

  // create a thread use for send request;
  {
    std::lock_guard<std::mutex> lg(m_msg_mutex);
    m_stoped = false;
  }

  m_thread = std::thread([this]() {
    while (!m_stoped) {
      std::unique_lock<std::mutex> lock(m_msg_mutex); // 获取锁
      m_msg_condition.wait(lock);                     // 等待条件
      while (!m_msg_queue.empty()) {
        std::string msg = m_msg_queue.front();
        m_msg_queue.pop();
        lock.unlock();
        commitRequest2GSoVits(msg);
        lock.lock();
      }
    }
  });

  return make_error_code(gpt_sovits_errc::success);
}
std::error_code GSoVITSAssist::stop() {
  {
    std::lock_guard<std::mutex> lg(m_msg_mutex);
    m_stoped = true;
    m_msg_condition.notify_all();
  }

  if (m_thread.joinable()) {
    m_thread.join();
  }
  try {
    if (m_gpt_sovist_process) {
      m_gpt_sovist_process->terminate();
    }
  } catch (const std::exception &e) {
    CORE_ERROR_TAG("GSoVITSAssist", "gpt-sovits terminate fail : {}", e.what());
    return make_error_code(gpt_sovits_errc::success);
  }
  CORE_INFO_TAG("GSoVITSAssist", "GSoVITSAssist stop");
  return make_error_code(gpt_sovits_errc::success);
}
void GSoVITSAssist::shutdown() {

  auto ec = stop();

  if (std::filesystem::exists(MODEL_DATA_PATH)) {
    std::ofstream ostream(MODEL_DATA_PATH);

    if (!ostream) {
      CORE_ERROR_TAG("GSoVITSAssist", "保存模型文件到 : {} 失败",
                     MODEL_DATA_PATH);
      return;
    }
    boost::json::value result = boost::json::value_from(m_GSoVITSModels);
    std::string json_str = boost::json::serialize(result);
    CORE_INFO_TAG("GSoVITSAssist", "保存模型文件信息 :\n {} ", json_str);
    ostream << json_str;
    ostream.close();
  }

  if (std::filesystem::exists(MODEL_CONFIG_PATH)) {
    std::ofstream ostream(MODEL_CONFIG_PATH);

    if (!ostream) {
      CORE_ERROR_TAG("GSoVITSAssist", "保存模型设定到 : {} 失败",
                     MODEL_CONFIG_PATH);
      return;
    }
    size_t index = 0;
    for (auto &model : m_GSoVITSModels) {

      if (m_RequestGSoVITSModel == model) {
        break;
      }
      index++;
    }
    std::string json_str =
        std::format("{{\"{0}\":{1}}}", "default_model", index);

    CORE_INFO_TAG("GSoVITSAssist", "保存模型设定 :\n {} ", json_str);
    ostream << json_str;
    ostream.close();
  }

  CORE_INFO_TAG("GSoVITSAssist", "GSoVITSAssist shutdown");
}

void GSoVITSAssist::pushMsg(const std::string &msg) {

  std::lock_guard<std::mutex> lg(m_msg_mutex);
  m_msg_queue.push(msg);
  m_msg_condition.notify_one();
}

void GSoVITSAssist::commitRequest2GSoVits(const std::string &msg) {

  m_request_body.text = msg;
  auto result = Serializer<GSoVITSRequestBody>::serializer(m_request_body);
  if (result.index() == 0) {
    auto js = std::get<0>(result);

    std::string json_string = boost::json::serialize(js);

    // receive audio data
    {
      std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
          request_url, inference_endpoint, HttpRequestMethod::post);

      request->AddHeader("Content-Type", "application/json");
      request->SetContent(json_string);

      request->AsyncReceive([request, msg](boost::beast::error_code ec,
                                           std::size_t transfered) {
        if (ec) {

          CORE_ERROR_TAG("GSoVITSAssist", "commitRequest2GSoVits fail : {}",
                         ec.what());
          return;
        }
        auto response = request->GetRespons();
        auto str_data = boost::beast::buffers_to_string(response.body().data());
        auto byte_stream =
            std::vector<uint8_t>(str_data.begin(), str_data.end());
        auto gsovits_assist =
            ModuleManager::getInstance().getModule<GSoVITSAssist>();

        gsovits_assist->pushAudioStream(byte_stream);
      });
    }
  }
}
} // namespace NAssist