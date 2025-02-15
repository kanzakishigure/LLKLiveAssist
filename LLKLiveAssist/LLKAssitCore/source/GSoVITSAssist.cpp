#include "GSoVITSAssist.h"
#include "ModuleManager.h"
#include "Net/HttpRequest.h"
#include "Net/TCPClient.h"

#include <boost/process/v1/detail/child_decl.hpp>
#include <boost/process/v1/io.hpp>
#include <boost/process/v1/start_dir.hpp>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdint.h>
#include <string>
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
  if (std::filesystem::exists(MODEL_CONFIG_PATH)) {
    std::ifstream istream(MODEL_CONFIG_PATH);
    using Iterator = std::istreambuf_iterator<char>;
    std::string content(Iterator{istream}, Iterator{});
    istream.close();

    if (!content.empty()) {
      auto jv = boost::json::parse(content);
      auto result = Parser<GSoVITSModel>::parse(jv);
      if (result.index() == 1) {
        std::cout << "load model fail!" << std::endl;
      }
      m_GSoVITSModel = get<0>(result);
    }
  }
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
    std::string gps_sovist_args = std::format(
        "{0}runtime/python.exe {0}api_v2.py -a {1} -p {2} -c "
        "{0}GPT_SoVITS/configs/tts_infer.yaml",
        GPT_SOVITS_ROOT, request_host, std::to_string(request_port));
    m_gpt_sovist_process = std::make_unique<boost::process::child>(
        gps_sovist_args, boost::process::start_dir(GPT_SOVITS_ROOT));

    // 尝试使用tcp连接服务
    TCPClient tcp(request_host, request_port);
    while (true) {
      if (tcp.try_connect()) {
        break;
      }
    }
    std::cout << "gpt-sovist init success" << std::endl;
  }
  // set the sovits model and gpt model
  {
    std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
        request_url, set_gpt_weights_endpoint, HttpRequestMethod::get);

    std::cout << request->Receive() << std::endl;
  }

  {
    std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
        request_url, set_sovits_weights_endpoint, HttpRequestMethod::get);

    std::cout << request->Receive() << std::endl;
  }
  // generate request body use data from model
  m_request_body.text_lang = "zh";
  m_request_body.ref_audio_path = m_GSoVITSModel.ref_audio_path;
  m_request_body.prompt_text = m_GSoVITSModel.prompt_text;
  m_request_body.prompt_lang = m_GSoVITSModel.prompt_lang;

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

void GSoVITSAssist::shutdown() {

  {
    std::lock_guard<std::mutex> lg(m_msg_mutex);
    m_stoped = true;
    m_msg_condition.notify_all();
  }

  if (m_thread.joinable()) {
    m_thread.join();
  }
  if (m_gpt_sovist_process) {
    m_gpt_sovist_process->terminate();
  }
}

void GSoVITSAssist::drawUI() {}

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
          std::cout << "error" << ec.what() << std::endl;
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