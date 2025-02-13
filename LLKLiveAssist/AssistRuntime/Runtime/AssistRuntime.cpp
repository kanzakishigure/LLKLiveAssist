#include "AssistRuntime.h"


#include "BiliClientAssist.h"
#include "AudioAssist.h"
#include "GSoVITSAssist.h"

#include "ModuleManager.h"
#include <exception>
#include <functional>
#include <system_error>
#include <exception>

namespace NAssist {

  NAssist::AssistRuntime* NAssist::AssistRuntime::instance = nullptr;
AssistRuntime::AssistRuntime()
{
  init();
}
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

  std::shared_ptr<GSoVITSAssist> gsovits_assist = std::make_shared<GSoVITSAssist>();
  ModuleManager::getInstance().addPlugin(gsovits_assist);

  instance = this;
  

}
void AssistRuntime::shutdown() {
 ModuleManager::getInstance().shutdown();
  // Shutdown the assist core
}

void AssistRuntime::startModule(PluginType plugin)
{
  ModuleManager::getInstance().startModule(plugin);
}

void AssistRuntime::stratAllModule()
{
  ModuleManager::getInstance().startAllModule();
}

void AssistRuntime::stratAllModule(std::function<void(const std::error_code)> callback)
{
	m_core_thread =  std::thread([callback]() {
		std::error_code ec;
		try {
			ModuleManager::getInstance().startAllModule();
		}
		catch (std::exception const& e) {
			ec = std::make_error_code(std::errc::executable_format_error);
		}

		callback(ec);
		});
}

void AssistRuntime::stopAllModule()
{
  ModuleManager::getInstance().stopAllModule();
}

void AssistRuntime::stopAllModule(std::function<void(const std::error_code)> callback)
{
  
  std::error_code ec;
  try {
	  ModuleManager::getInstance().stopAllModule();
  }
  catch (std::exception const& e) {
	  ec = std::make_error_code(std::errc::executable_format_error);
  }
  callback(ec);
}



} // namespace NAssist
