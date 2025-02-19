#include "Core/logger.h"
#include "PluginBase.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>
#include <iostream>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
namespace NAssist {

  
std::unique_ptr<LLKLogger> LLKLogger::m_instance =
    std::make_unique<LLKLogger>();
    std::shared_ptr<spdlog::logger>  LLKLogger::s_CoreLogger;
void LLKLogger::init() {

 
  spdlog::set_pattern("[%T] [%n]: %^[%l]%$ %v");
  spdlog::set_level(spdlog::level::trace);
  auto core = spdlog::stdout_color_mt("core");
  auto runtime = spdlog::stderr_color_mt("runtime");
  addLogger(core,LLKLogger::Type::Core );
  addLogger(runtime, LLKLogger::Type::Runtime);
  CORE_INFO("logger init success");
  
}
void LLKLogger::shutdown() {
  for(auto& [type,logger] : m_loggers)
  {
    logger.reset();
  }
  spdlog::shutdown();
}
void LLKLogger::addLogger(std::shared_ptr<spdlog::logger> logger, LLKLogger::Type type)
{
  
  if(!m_loggers.count(type))
  {
    

    if(LLKLogger::Type::GUI==type)
    {
      for(auto& [type,m_log]:m_loggers)
      {
        for(auto& sink : logger->sinks() )
        {
          m_log->sinks().push_back(sink);
        }
      }
    }
    m_loggers[type] = logger;
  }
}
} // namespace NAssist