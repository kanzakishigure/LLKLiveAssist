#pragma once
#include "Data/GSoVITSModel.h"
#include "Net/HttpRequest.h"
#include "PluginBase.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <boost/process.hpp>
#include <boost/process/v1/detail/child_decl.hpp>

namespace NAssist {

class GSoVITSAssist : public PluginBase {
public:
  virtual void init() override;
  virtual std::error_code start() override;
  virtual std::error_code stop() override;
  virtual void shutdown() override;
  

  virtual PluginType getType() override { return PluginType::GSoVITS; }
  static PluginType getStaticType() { return PluginType::GSoVITS; }

  void pushMsg(const std::string &msg);
  void commitRequest2GSoVits(const std::string &msg);
  void pushAudioStream(std::vector<uint8_t> bytes);
  std::vector<uint8_t> popAudioSteam();

  std::shared_ptr<std::vector<GSoVITSModel>> getModelLib() { return m_model_lib; }
  void setDefaultModel(size_t index) {
    if (index >= 0 && index < m_model_lib->size())
    m_default_model = m_model_lib->at(index);
  }
  GSoVITSModel getDefaultModel() const{
      return m_default_model;
  }

private:
  bool m_stoped = true;

  std::unique_ptr<boost::process::child> m_gpt_sovist_process;
  std::mutex m_msg_mutex;
  std::mutex m_audio_mutex;
  std::condition_variable m_msg_condition;

  GSoVITSModel m_default_model;
  std::shared_ptr<std::vector<GSoVITSModel>> m_model_lib;
  GSoVITSRequestBody m_request_body;

  std::queue<std::string> m_msg_queue;
  std::queue<std::vector<uint8_t>> m_audio_buffers;

  std::thread m_thread;
};
} // namespace NAssist