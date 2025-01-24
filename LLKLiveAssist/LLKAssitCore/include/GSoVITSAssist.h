#pragma once
#include "Data/GSoVITSModel.h"
#include "Net/HttpRequest.h"
#include "PluginBase.h"
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <semaphore>

namespace NAssist {

class GSoVITSAssist : public PluginBase {
public:

  virtual void init() override;
  virtual void start() override;
  virtual void shutdown() override;
  virtual void drawUI() override;

  virtual PluginType getStaticType() override { return m_type; }

  void pushMsg(const std::string &msg);
  void commitRequest2GSoVits(const std::string& msg);

private:
  PluginType m_type = PluginType::GSoVITS;

  bool  m_stoped = true;
  std::mutex m_mutex;
  std::condition_variable m_condition;

  GSoVITSModel m_GSoVITSModel;
  GSoVITSRequestBody m_request_body;
  
  std::queue<std::string> m_msg_queue;

  std::thread m_thread;
};
} // namespace NAssist