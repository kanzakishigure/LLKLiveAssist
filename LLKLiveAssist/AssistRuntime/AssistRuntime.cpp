#include "AssistRuntime.h"

#include "BiliClientAssist.h"
#include "BroadcastAssist.h"
#include "GSoVITSAssist.h"

#include <iostream>

namespace NAssist {
void AssistRuntime::init() {
  // Initialize the assist core
  // create and add plugins
  std::shared_ptr<BroadcastAssist> broadcast_assist = std::make_shared<BroadcastAssist>();
  AddAssistPlugin(broadcast_assist);

  std::shared_ptr<BiliClientAssist> bili_client_assist = std::make_shared<BiliClientAssist>();
  //AddAssistPlugin(bili_client_assist);

  std::shared_ptr<GSoVITSAssist> GSoVITS_assist = std::make_shared<GSoVITSAssist>();
  //AddAssistPlugin(GSoVITS_assist);

  for (auto &plug : plugins) {
    plug->start();
  }
}
void AssistRuntime::shutdown() {
  for (const auto &plugin : plugins) {
    plugin->shutdown();
  }
  // Shutdown the assist core
}
void AssistRuntime::run() {
  while (!closed) {
    // Run the assist core
    for (const auto &plugin : plugins) {
      plugin->drawUI();
    }
  }
}
void AssistRuntime::AddAssistPlugin(const std::shared_ptr<PluginBase> plugin) {
  plugin->init();
  plugins.push_back(plugin);
}

} // namespace NAssist
