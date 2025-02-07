#include "GSoVITSAssist.h"
#include "ModuleManager.h"
#include "Net/HttpRequest.h"
#include <boost/json/parse.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


namespace NAssist {

const std::string request_host = "http://127.0.0.1:9880";
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

  // set the sovits model and gpt model
  {

    std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
        request_host, set_gpt_weights_endpoint, HttpRequestMethod::get);
    request->AddHeader("weights_path", m_GSoVITSModel.gpt_weights);
    std::cout << request->Receive() << std::endl;
  }
  {

    std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
        request_host, set_sovits_weights_endpoint, HttpRequestMethod::get);
    request->AddHeader("weights_path", m_GSoVITSModel.sovits_weights);
    std::cout << request->Receive() << std::endl;
  }
  // generate request body use data from model
  m_request_body.text_lang = "zh";
  m_request_body.ref_audio_path = m_GSoVITSModel.ref_audio_path;
  m_request_body.prompt_text = m_GSoVITSModel.prompt_text;
  m_request_body.prompt_lang = m_GSoVITSModel.prompt_lang;

  // create a thread use for send request;
  m_stoped = false;
  m_thread = std::thread([this]() {
    while (!m_stoped) {
      std::unique_lock<std::mutex> lock(m_msg_mutex); // 获取锁
      m_msg_condition.wait(lock,
                       [this] { return !m_msg_queue.empty(); }); // 等待条件
      while (!m_msg_queue.empty()) {
        std::string msg = m_msg_queue.front();
        m_msg_queue.pop();
        lock.unlock();
        commitRequest2GSoVits(msg);
        lock.lock();
      }
    }
  });
}

void GSoVITSAssist::pushAudioStream(std::vector<uint8_t> bytes_stream)
{
  std::lock_guard<std::mutex> lg(m_audio_mutex);
  m_audio_buffers.push(bytes_stream);
}
std::vector<uint8_t> GSoVITSAssist::popAudioSteam()
{
  std::lock_guard<std::mutex> lg(m_audio_mutex);
  std::vector<uint8_t> result;
  if(!m_audio_buffers.empty())
  {
    result =  m_audio_buffers.front();
    m_audio_buffers.pop();
  }
  return  result;
}
void GSoVITSAssist::start() {}

void GSoVITSAssist::shutdown() {
  std::lock_guard<std::mutex> lg(m_msg_mutex);
  m_stoped = true;
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

    //receive audio data
    {
      std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
          request_host, inference_endpoint, HttpRequestMethod::post);

      request->AddHeader("Content-Type", "application/json");
      request->SetContent(json_string);

      request->AsyncReceive([request, msg](boost::beast::error_code ec, std::size_t transfered) {
            if (ec) {
              std::cout << "error" << ec.what() << std::endl;
              return;
            }
            auto response = request->GetRespons();
            auto str_data = boost::beast::buffers_to_string(response.body().data());
            auto byte_stream = std::vector<uint8_t>(str_data.begin(),str_data.end());
            auto gsovits_assist = ModuleManager::getInstance().getModule<GSoVITSAssist>();

            gsovits_assist->pushAudioStream(byte_stream);
            
          });
    }
  }
}
} // namespace NAssist