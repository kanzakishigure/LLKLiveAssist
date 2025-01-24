#pragma once
#include "PluginBase.h"
#include <memory>
#include <vector>
namespace NAssist {
class AssistRuntime {
public:
  virtual void init();
  virtual void shutdown();
  virtual void run();

private:
  void AddAssistPlugin(std::shared_ptr<PluginBase> plugin);

  bool closed = false;
  std::vector<std::shared_ptr<PluginBase>> plugins;
};
} // namespace NAssist
