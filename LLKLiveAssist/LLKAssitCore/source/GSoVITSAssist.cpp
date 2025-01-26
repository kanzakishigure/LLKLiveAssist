#include "GSoVITSAssist.h"
#include "Net/HttpRequest.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

namespace NAssist {

const std::string request_host = "http://127.0.0.1:9880";
const std::string inference_endpoint = "/tts";
const std::string control_endpoint = "/control";
const std::string set_gpt_weights_endpoint = "/set_gpt_weights";
const std::string set_sovits_weights_endpoint = "/set_sovits_weights";

void GSoVITSAssist::init() {
  // load model data
  m_GSoVITSModel.model_name = "胡桃";
  m_GSoVITSModel.sovits_weights = "SoVITS_weights_v2/胡桃/胡桃_e15_s825.pth";
  m_GSoVITSModel.gpt_weights = "SoVITS_weights_v2/胡桃/胡桃-e10.ckpt";

  m_GSoVITSModel.prompt_text = "本堂主略施小计，你就败下阵来了，嘿嘿。";
  m_GSoVITSModel.prompt_lang = "zh";
  m_GSoVITSModel.ref_audio_path = "SoVITS_weights_v2/胡桃/本堂主略施小计，你就败下阵来了，嘿嘿。.wav";

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
      std::unique_lock<std::mutex> lock(m_mutex); // 获取锁
      m_condition.wait(lock,
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

void GSoVITSAssist::start() {}

void GSoVITSAssist::shutdown() {
  std::lock_guard<std::mutex> lg(m_mutex);
  m_stoped = true;
}

void GSoVITSAssist::drawUI() {}

void GSoVITSAssist::pushMsg(const std::string &msg) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_msg_queue.push(msg);
  m_condition.notify_one();
}

void GSoVITSAssist::commitRequest2GSoVits(const std::string &msg) {

  m_request_body.text = msg;
  auto result = Serializer<GSoVITSRequestBody>::serializer(m_request_body);
  if (result.index() == 0) {
    auto js = std::get<0>(result);

    std::string json_string = boost::json::serialize(js);

    {
      std::shared_ptr<HttpRequest> request = HttpRequest::CreateRequest(
          request_host, inference_endpoint, HttpRequestMethod::post);

      request->AddHeader("Content-Type", "application/json");
      request->SetContent(json_string);
      
      request->AsyncReceive([request,msg](boost::beast::error_code ec,std::size_t transfered){

        if(ec)
        {
          std::cout << "error"<<ec.what() << std::endl;
          return;
        }
        auto response = request->GetRespons();
      
      if(!std::filesystem::exists("./output/"))
      {
        std::filesystem::create_directory("./output/");
      }
      
      std::string file_name = "./output/test.wav";
      std::ofstream out(file_name, std::ios::binary);
      out<<boost::beast::buffers_to_string(response.body().data());
      out.close();
      });
    
    }
  }
}
} // namespace NAssist