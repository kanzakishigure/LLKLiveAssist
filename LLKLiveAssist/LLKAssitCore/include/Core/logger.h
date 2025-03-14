#pragma  once



#include <cstdint>
#include <format>
#include <map>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/format.h>
#include <utility>

#include "spdlog/spdlog.h"


namespace NAssist {

class LLKLogger {
public:
  enum class Type : uint8_t { Core = 0, Runtime, GUI };
  enum class Level : uint8_t { Trace = 0, Info, Warn, Error, Critical };
  LLKLogger() = default;
  ~LLKLogger() = default;
  void init();
  void shutdown();

  void addLogger(std::shared_ptr<spdlog::logger> logger, Type type);
  static const std::unique_ptr<LLKLogger> &instance() { return m_instance; }
  
 
  static std::shared_ptr<spdlog::logger> GetLogger(LLKLogger::Type type);
  template <typename T, typename... Args>
  static std::string format(T&& fmt_s,Args &&...args);
  template <typename... Args>
  static void PrintMessage(LLKLogger::Type type, LLKLogger::Level level,
                           std::string_view tag,   Args &&...args);

private:
  static std::unique_ptr<LLKLogger> m_instance;
  std::map<Type, std::shared_ptr<spdlog::logger>> m_loggers;
  static std::shared_ptr<spdlog::logger>  s_CoreLogger;
};


} // namespace NAssist


//core logging marcos
#define CORE_TRACE(...)      ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::Core)->trace(__VA_ARGS__)
#define CORE_INFO(...)       ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::Core)->info(__VA_ARGS__)
#define CORE_WARN(...)       ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::Core)->warn(__VA_ARGS__)
#define CORE_ERROR(...)      ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::Core)->error(__VA_ARGS__)
#define CORE_CRITICAL(...)   ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::Core)->critical(__VA_ARGS__)

//client logging marcos
#define RUNTIME_TRACE(...)    ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::Runtime)->trace(__VA_ARGS__)
#define RUNTIME_INFO(...)     ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::Runtime)->info(__VA_ARGS__)
#define RUNTIME_WARN(...)     ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::Runtime)->warn(__VA_ARGS__)
#define RUNTIME_ERROR(...)    ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::Runtime)->error(__VA_ARGS__)
#define RUNTIME_CRITICAL(...) ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::Runtime)->critical(__VA_ARGS__)

#define CONCAT_DIRECT(a, b)    a##b
#define CONCAT(a, b)           CONCAT_DIRECT(a, b)
#define GUI_TRACE(...)     auto CONCAT(logger,__LINE__) = ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::GUI);if(CONCAT(logger,__LINE__))CONCAT(logger,__LINE__)->trace(__VA_ARGS__)
#define GUI_INFO(...)      auto CONCAT(logger,__LINE__) = ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::GUI);if(CONCAT(logger,__LINE__))CONCAT(logger,__LINE__)->info(__VA_ARGS__) 
#define GUI_WARN(...)      auto CONCAT(logger,__LINE__) = ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::GUI);if(CONCAT(logger,__LINE__))CONCAT(logger,__LINE__)->warn(__VA_ARGS__) 
#define GUI_ERROR(...)     auto CONCAT(logger,__LINE__) = ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::GUI);if(CONCAT(logger,__LINE__))CONCAT(logger,__LINE__)->error(__VA_ARGS__)
#define GUI_CRITICAL(...)  auto CONCAT(logger,__LINE__) = ::NAssist::LLKLogger::GetLogger(::NAssist::LLKLogger::Type::GUI);if(CONCAT(logger,__LINE__))CONCAT(logger,__LINE__)->critical(__VA_ARGS__) 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tagged logs                                                                                    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CORE_TRACE_TAG(tag,...)      ::NAssist::LLKLogger::PrintMessage(::NAssist::LLKLogger::Type::Core,::NAssist::LLKLogger::Level::Trace,tag,__VA_ARGS__)
#define CORE_INFO_TAG(tag,...)       ::NAssist::LLKLogger::PrintMessage(::NAssist::LLKLogger::Type::Core,::NAssist::LLKLogger::Level::Info,tag,__VA_ARGS__)
#define CORE_WARN_TAG(tag,...)       ::NAssist::LLKLogger::PrintMessage(::NAssist::LLKLogger::Type::Core,::NAssist::LLKLogger::Level::Warn,tag,__VA_ARGS__)
#define CORE_ERROR_TAG(tag,...)      ::NAssist::LLKLogger::PrintMessage(::NAssist::LLKLogger::Type::Core,::NAssist::LLKLogger::Level::Error,tag,__VA_ARGS__)
#define CORE_CRITICAL_TAG(tag,...)   ::NAssist::LLKLogger::PrintMessage(::NAssist::LLKLogger::Type::Core,::NAssist::LLKLogger::Level::Critical,tag,__VA_ARGS__)

#define RUNTIME_TRACE_TAG(tag,...)    ::NAssist::LLKLogger::PrintMessage(::NAssist::LLKLogger::Type::Runtime,::NAssist::LLKLogger::Level::Trace,tag,__VA_ARGS__)
#define RUNTIME_INFO_TAG(tag,...)     ::NAssist::LLKLogger::PrintMessage(::NAssist::LLKLogger::Type::Runtime,::NAssist::LLKLogger::Level::Info,tag,__VA_ARGS__)
#define RUNTIME_WARN_TAG(tag,...)     ::NAssist::LLKLogger::PrintMessage(::NAssist::LLKLogger::Type::Runtime,::NAssist::LLKLogger::Level::Warn,tag,__VA_ARGS__)
#define RUNTIME_ERROR_TAG(tag,...)    ::NAssist::LLKLogger::PrintMessage(::NAssist::LLKLogger::Type::Runtime,::NAssist::LLKLogger::Level::Error,tag,__VA_ARGS__)
#define RUNTIME_CRITICA_TAGL(tag,...) ::NAssist::LLKLogger::PrintMessage(::NAssist::LLKLogger::Type::Runtime,::NAssist::LLKLogger::Level::Critical,tag,__VA_ARGS__)


namespace NAssist
{
template <typename T, typename... Args>
std::string LLKLogger::format(T&& fmt_s, Args &&...args) {
	return fmt::format(fmt::runtime(fmt_s),args...);							
}
template <typename... Args>
void LLKLogger::PrintMessage(LLKLogger::Type type, LLKLogger::Level level,
                             std::string_view tag, Args &&...args) {
  auto logger = LLKLogger::GetLogger(type);
 

								
  std::string_view logStringFMT = tag.empty() ? "{0}{1}" : "[{0}] {1}";
  switch (level) {
  case LLKLogger::Level::Trace:
    logger->trace(fmt::runtime(logStringFMT), tag,LLKLogger::format(std::forward<Args>(args)...));
    break;
  case LLKLogger::Level::Info:
    logger->info(fmt::runtime(logStringFMT), tag,LLKLogger::format(std::forward<Args>(args)...));
    break;
  case LLKLogger::Level::Warn:
    logger->warn(fmt::runtime(logStringFMT), tag,LLKLogger::format(std::forward<Args>(args)...));
    break;
  case LLKLogger::Level::Error:
    logger->error(fmt::runtime(logStringFMT), tag,LLKLogger::format(std::forward<Args>(args)...));
    break;
  case LLKLogger::Level::Critical:
    logger->critical(fmt::runtime(logStringFMT), tag,LLKLogger::format(std::forward<Args>(args)...));
    break;
  }
	}

	

	

}