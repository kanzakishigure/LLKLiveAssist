#pragma once
#include "PluginBase.h"

#include <functional>
#include <thread>

namespace NAssist {
class AssistRuntime {
public:
  AssistRuntime();
  ~AssistRuntime();

  void init();
  void shutdown();

  void startModule(PluginType plugin);
  void stratAllModule(std::function<void(const std::error_code ec)> callback);
  void stratAllModule();
  void stopAllModule();
  void stopAllModule(std::function<void(const std::error_code ec)> callback);

  static NAssist::AssistRuntime *getInstance() { return instance; }

private:
  static NAssist::AssistRuntime *instance;

  std::thread m_core_thread;
};
} // namespace NAssist
