#include "GSoVITSAssist.h"
#include "AudioAssist.h"
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

#include "AudioAssist.h"
#include <stdint.h>
#include <string>
#include <system_error>
#include <tuple>
#include <vector>

namespace NAssist {

const std::string inference_endpoint = "/tts";
const std::string control_endpoint = "/control";
const std::string set_gpt_weights_endpoint = "/set_gpt_weights";
const std::string set_sovits_weights_endpoint = "/set_sovits_weights";
const std::string native_gpt_sovits_server = "http://127.0.0.1:9880";
void GSoVITSAssist::init() {
  // load model data
  m_model_lib = std::make_shared<std::vector<GSoVITSModel>>();
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
          m_model_lib->emplace_back() = get<0>(result);
        }

      } else {
        auto result = Parser<GSoVITSModel>::parse(jv);
        if (result.index() == 1) {

          CORE_ERROR_TAG("GSoVITSAssist", "GSoVITSAssist load model fail!");
        }
        m_model_lib->emplace_back() = get<0>(result);
      }
    }
  }
  if (std::filesystem::exists(MODEL_CONFIG_PATH)) {
    std::ifstream istream(MODEL_CONFIG_PATH);
    using Iterator = std::istreambuf_iterator<char>;
    std::string content(Iterator{istream}, Iterator{});
    istream.close();

    if (!m_model_lib->empty()) {
      m_default_model = m_model_lib->front();
    }
    if (!content.empty()) {
      auto jv = boost::json::parse(content);
      if (!jv.is_null())
        if (!jv.at("default_model").is_null()) {
          int index = jv.at("default_model").as_int64();
          if (index < m_model_lib->size() && index > 0) {
            m_default_model = m_model_lib->at(index);
          }
        }
    }
  }
  if (std::filesystem::exists(INFER_DATA_PATH)) {
    std::ifstream istream(INFER_DATA_PATH);
    using Iterator = std::istreambuf_iterator<char>;
    std::string content(Iterator{istream}, Iterator{});
    istream.close();

    if (!content.empty()) {
      auto jv = boost::json::parse(content);

      auto result = Parser<InferData>::parse(jv);
      if (result.index() == 1) {

        CORE_ERROR_TAG("GSoVITSAssist", "GSoVITSAssist load infer data fail!");
      }
      m_request_body.infer_data = get<0>(result);
    }
  }
  CORE_INFO_TAG("GSoVITSAssist", "GSoVITSAssist init success");
}

void GSoVITSAssist::pushAudioStream(
    std::tuple<std::vector<uint8_t>, AudioConfig> audio_data) {
  std::lock_guard<std::mutex> lg(m_audio_mutex);
  m_audio_buffers.push(audio_data);
}
std::tuple<std::vector<uint8_t>, AudioConfig> GSoVITSAssist::popAudioSteam() {
  std::lock_guard<std::mutex> lg(m_audio_mutex);
  std::tuple<std::vector<uint8_t>, AudioConfig> result;
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
    if (enable_native_server) {
      std::error_code ec;

      CORE_TRACE_TAG("GSoVITSAssist", "gps_sovist_args is {}", gps_sovist_args);

      m_gpt_sovist_process = std::make_unique<boost::process::child>(
          gps_sovist_args, boost::process::start_dir(GPT_SOVITS_ROOT), ec);

      if (ec) {
        return make_error_code(gpt_sovits_errc::gsovist_not_found);
      }
    } else {
      auto index = gpt_sovits_server.find_last_of(':');
      auto port = gpt_sovits_server.substr(index + 1, gpt_sovits_server.size() -
                                                          index - 1);
      request_host = gpt_sovits_server.substr(0, index);
      if (request_host.find_first_of("http://") != std::string::npos) {
        request_host = request_host.substr(7, request_host.size() - 7);
      }
      request_port = std::stoi(port);
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
  setServerInferModel(m_default_model);
  // generate request body use data from model

  std::filesystem::path ref_audio_path = m_default_model.ref_audio_path;
  if (ref_audio_path.is_relative() && enable_native_server) {
    std::error_code ec;
    ref_audio_path = std::filesystem::absolute(ref_audio_path);
    ref_audio_path = std::filesystem::canonical(ref_audio_path, ec);
    if (ec) {
      CORE_ERROR_TAG("GSoVITSAssist", "ref_audio_path :{{}} invalid",
                     ref_audio_path.generic_string());
      return make_error_code(gpt_sovits_errc::ref_audio_path_invalid);
    }
  }
  m_request_body.text_lang = m_default_model.prompt_lang;
  m_request_body.ref_audio_path = ref_audio_path.generic_string();
  m_request_body.prompt_text = m_default_model.prompt_text;
  m_request_body.prompt_lang = m_default_model.prompt_lang;

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
  if (enable_native_server) {
    try {
      if (m_gpt_sovist_process) {
        m_gpt_sovist_process->terminate();
      }
    } catch (const std::exception &e) {
      CORE_ERROR_TAG("GSoVITSAssist", "gpt-sovits terminate fail : {}",
                     e.what());
      return make_error_code(gpt_sovits_errc::success);
    }
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
    auto model_lib = *m_model_lib;
    boost::json::value result = boost::json::value_from(model_lib);
    std::string json_str = boost::json::serialize(result);
    ostream << json_str;
    ostream.close();
    CORE_INFO_TAG("GSoVITSAssist",
                  "保存模型文件信息 ://///////////////////////////////////");
    for (const auto &model : *m_model_lib) {
      CORE_TRACE("model : {} ", model.model_name);
      CORE_TRACE("sovits_weights : {} ", model.sovits_weights);
      CORE_TRACE("gpt_weights : {}\n", model.gpt_weights);
    }
    CORE_INFO_TAG("GSoVITSAssist",
                  "/////////////////////////////////////////////////////");
  }

  if (std::filesystem::exists(MODEL_CONFIG_PATH)) {
    std::ofstream ostream(MODEL_CONFIG_PATH);

    if (!ostream) {
      CORE_ERROR_TAG("GSoVITSAssist", "保存模型设定到 : {} 失败",
                     MODEL_CONFIG_PATH);
      return;
    }
    size_t index = 0;
    for (auto &model : *m_model_lib) {

      if (m_default_model == model) {
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

  if (std::filesystem::exists(INFER_DATA_PATH)) {
    std::ofstream ostream(INFER_DATA_PATH);

    if (!ostream) {
      CORE_ERROR_TAG("GSoVITSAssist", "保存推理设置到 : {} 失败",
                     INFER_DATA_PATH);
      return;
    }

    boost::json::value result =
        boost::json::value_from(m_request_body.infer_data);
    std::string json_str = boost::json::serialize(result);
    ostream << json_str;
    ostream.close();
    CORE_INFO_TAG("GSoVITSAssist", "保存推理设定 :\n {} ", json_str);
  }
  CORE_INFO_TAG("GSoVITSAssist", "GSoVITSAssist shutdown");
}

void GSoVITSAssist::pushMsg(const std::string &msg) {

  std::lock_guard<std::mutex> lg(m_msg_mutex);
  m_msg_queue.push(msg);
  m_msg_condition.notify_one();
}

void GSoVITSAssist::setServerInferModel(GSoVITSModel model) {

  auto request_url =
      enable_native_server ? native_gpt_sovits_server : gpt_sovits_server;
  {
    std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
        request_url, set_gpt_weights_endpoint, HttpRequestMethod::get);

    std::filesystem::path path = model.gpt_weights;
    if (path.is_relative() && enable_native_server) {
      std::error_code ec;
      path = std::filesystem::absolute(path);
      path = std::filesystem::canonical(path, ec);
      if (ec) {
        CORE_ERROR_TAG("GSoVITSAssist", "gpt_model_path :{{}} invalid",
                       path.generic_string());
      }
    }
    request->AddRequestParameter("weights_path", path.generic_string());
    CORE_INFO_TAG("GSoVITSAssist", "set_gpt_weights :{}", request->Receive());
  }

  {
    std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
        request_url, set_sovits_weights_endpoint, HttpRequestMethod::get);

    std::filesystem::path path = model.sovits_weights;
    if (path.is_relative() && enable_native_server) {
      std::error_code ec;
      path = std::filesystem::absolute(path);
      path = std::filesystem::canonical(path, ec);
      if (ec) {
        CORE_ERROR_TAG("GSoVITSAssist", "sovist_model_path :{{}} invalid",
                       path.generic_string());
      }
    }
    request->AddRequestParameter("weights_path", path.generic_string());

    CORE_INFO_TAG("GSoVITSAssist", "set_sovits_weights :{}",
                  request->Receive());
  }
}
void GSoVITSAssist::commitRequest2GSoVits(const std::string &msg) {

  std::filesystem::path ref_audio_path = m_default_model.ref_audio_path;
  if (ref_audio_path.is_relative() && enable_native_server) {
    std::error_code ec;
    ref_audio_path = std::filesystem::absolute(ref_audio_path);
    ref_audio_path = std::filesystem::canonical(ref_audio_path, ec);
    if (ec) {
      CORE_ERROR_TAG("GSoVITSAssist", "ref_audio_path :{{}} invalid",
                     ref_audio_path.generic_string());
    }
  }

  m_request_body.text = msg;
  m_request_body.text_lang = m_default_model.prompt_lang;
  m_request_body.ref_audio_path = ref_audio_path.generic_string();
  m_request_body.prompt_text = m_default_model.prompt_text;
  m_request_body.prompt_lang = m_default_model.prompt_lang;
  
  auto request_url =
      enable_native_server ? native_gpt_sovits_server : gpt_sovits_server;
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

      request->AsyncReceive([request, msg, this](boost::beast::error_code ec,
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

        AudioConfig config;
        config.format = Utils::String2SoundType(m_request_body.media_type);
        config.AudioVolume = m_default_model.model_volume_factor;
        gsovits_assist->pushAudioStream({byte_stream, config});
      });
    }
  }
}

void GSoVITSAssist::commitRequest2GSoVits(GSoVITSModel model,
                                          const std::string &msg) {

  TCPClient tcp(request_host, request_port);
  if (!tcp.try_connect()) {
    return;
  }
  auto request_url =
      enable_native_server ? native_gpt_sovits_server : gpt_sovits_server;
  // set the sovits model and gpt model
  setServerInferModel(model);

  std::filesystem::path ref_audio_path = model.ref_audio_path;
  if (ref_audio_path.is_relative() && enable_native_server) {
    std::error_code ec;
    ref_audio_path = std::filesystem::absolute(ref_audio_path);
    ref_audio_path = std::filesystem::canonical(ref_audio_path, ec);
    if (ec) {
      CORE_ERROR_TAG("GSoVITSAssist", "ref_audio_path :{{}} invalid",
                     ref_audio_path.generic_string());
    }
  }

  GSoVITSRequestBody request_body;
  request_body.text = msg;
  request_body.text_lang = model.prompt_lang;
  request_body.ref_audio_path = ref_audio_path.generic_string();
  request_body.prompt_text = model.prompt_text;
  request_body.prompt_lang = model.prompt_lang;

  request_body.infer_data = m_request_body.infer_data;

  auto result = Serializer<GSoVITSRequestBody>::serializer(request_body);
  if (result.index() == 0) {
    auto js = std::get<0>(result);

    std::string json_string = boost::json::serialize(js);

    // receive audio data
    {
      std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
          request_url, inference_endpoint, HttpRequestMethod::post);

      request->AddHeader("Content-Type", "application/json");
      request->SetContent(json_string);

      request->AsyncReceive([request_body, request,
                             model](boost::beast::error_code ec,
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

        AudioConfig cfg;
        cfg.AudioVolume = model.model_volume_factor;
        cfg.format = Utils::String2SoundType(request_body.media_type);
        gsovits_assist->pushAudioStream({byte_stream, cfg});
      });
    }
  }
}
} // namespace NAssist