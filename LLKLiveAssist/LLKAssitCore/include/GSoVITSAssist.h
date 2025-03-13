#pragma once
#include "AudioAssist.h"
#include "Data/GSoVITSModel.h"
#include "PluginBase.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>
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
  void setServerInferModel(GSoVITSModel model);
  void commitRequest2GSoVits(const std::string &msg);
  void commitRequest2GSoVits(GSoVITSModel model, const std::string &msg);
  void
  pushAudioStream(std::tuple<std::vector<uint8_t>, AudioConfig> audio_data);
  std::tuple<std::vector<uint8_t>, AudioConfig> popAudioSteam();

  std::shared_ptr<std::vector<GSoVITSModel>> getModelLib() {
    return m_model_lib;
  }
  InferData getInferData() const { return m_request_body.infer_data; }
  void setInferData(InferData data) { m_request_body.infer_data = data; }
  void setDefaultModel(size_t index) {
    if (index >= 0 && index < m_model_lib->size())
      m_default_model = m_model_lib->at(index);

    if (!m_stoped) {
      setServerInferModel(m_default_model);
    }
  }
  GSoVITSModel getDefaultModel() const { return m_default_model; }
  void setGptSovitsServer(const std::string url) { gpt_sovits_server = url; }
  std::string getGptSovitsServer() { return gpt_sovits_server; }
  void enableNativeServer(bool enable) { enable_native_server = enable; }

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
  std::queue<std::tuple<std::vector<uint8_t>, AudioConfig>> m_audio_buffers;

  std::thread m_thread;

  std::string gpt_sovits_server{"http://127.0.0.1:9880"};
  std::string request_host{"127.0.0.1"};
  int request_port = 9880;
  bool enable_native_server{true};
};
} // namespace NAssist