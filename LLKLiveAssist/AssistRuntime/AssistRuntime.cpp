#include "AssistRuntime.h"

#include "BiliClientAssist.h"
#include "AudioAssist.h"
#include "GSoVITSAssist.h"


#include <iostream>

namespace NAssist {

AssistRuntime::~AssistRuntime() {
  shutdown();
}

void AssistRuntime::init() {
  // Initialize the assist core
  // create and add plugins
  std::shared_ptr<AudioAssist> audio_assist = std::make_shared<AudioAssist>();
  ModuleManager::getInstance().addPlugin(audio_assist);

  std::shared_ptr<BiliClientAssist> bili_client_assist = std::make_shared<BiliClientAssist>();
  ModuleManager::getInstance().addPlugin(bili_client_assist);

  std::shared_ptr<GSoVITSAssist> GSoVITS_assist = std::make_shared<GSoVITSAssist>();
  ModuleManager::getInstance().addPlugin(GSoVITS_assist);

  ModuleManager::getInstance().startAllModule();



}
void AssistRuntime::shutdown() {
 ModuleManager::getInstance().shutdown();
  // Shutdown the assist core
}
void AssistRuntime::run() {
  while (!closed) {
    // Run the assist core
    ModuleManager::getInstance().update();
  }
}




} // namespace NAssist
