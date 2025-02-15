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
  // Shutdown the assist core
 ModuleManager::getInstance().shutdown();
 //join main therad
 if (m_core_thread.joinable())
 {
	 m_core_thread.join();
 }
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
  if(m_core_thread.joinable())
  {
    m_core_thread.join();
  }
  
	m_core_thread =  std::thread([callback]() {
		
	
		auto res = ModuleManager::getInstance().startAllModule();
		callback(res);
		});
}

void AssistRuntime::stopAllModule()
{
	
  ModuleManager::getInstance().stopAllModule();
}

void AssistRuntime::stopAllModule(std::function<void(const std::error_code)> callback)
{
  
  
  if (m_core_thread.joinable())
  {
	  m_core_thread.join();
  }

  m_core_thread = std::thread([callback]() {


	  ModuleManager::getInstance().stopAllModule();
	  callback(std::error_code());
	  });
}



} // namespace NAssist
