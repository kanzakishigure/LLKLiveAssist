#pragma once
#include "Data/GSoVITSModel.h"
#include "Net/HttpRequest.h"
#include "PluginBase.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <semaphore>
#include <vector>

namespace NAssist {

class GSoVITSAssist : public PluginBase {
public:

  virtual void init() override;
  virtual void start() override;
  virtual void shutdown() override;
  virtual void drawUI() override;

  virtual PluginType getType() override { return PluginType::GSoVITS; }
  static PluginType getStaticType() { return PluginType::GSoVITS; }

  void pushMsg(const std::string &msg);
  void commitRequest2GSoVits(const std::string& msg);
  void pushAudioStream(std::vector<uint8_t> bytes);
  std::vector<uint8_t> popAudioSteam();
private:
  bool  m_stoped = true;
  std::mutex m_msg_mutex;
  std::mutex m_audio_mutex;
  std::condition_variable m_msg_condition;

  GSoVITSModel m_GSoVITSModel;
  GSoVITSRequestBody m_request_body;
  
  std::queue<std::string> m_msg_queue;
  std::queue<std::vector<uint8_t>> m_audio_buffers;


  std::thread m_thread;
};
} // namespace NAssist