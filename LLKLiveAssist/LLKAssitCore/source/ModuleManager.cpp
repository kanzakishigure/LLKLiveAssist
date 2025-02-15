#include "ModuleManager.h"

namespace NAssist {

std::variant<bool, std::error_code>
ModuleManager::addPlugin(const std::shared_ptr<PluginBase> plugin) {
  if (plugin == nullptr) {
    return std::make_error_code(std::errc::invalid_argument);
  }

  // error will appear here
  plugin->init();
  // error will appear here

  m_plugins.push_back(plugin);
  return true;
}
std::error_code ModuleManager::startAllModule() {
    
    for (const auto &plugin : m_plugins) {
    auto res = plugin->start();
    if (res.value()!=0)
    {
        return res;
    }

  }
  return std::error_code();
}
void ModuleManager::stopAllModule() {
  for (const auto &plugin : m_plugins) {
    plugin->shutdown();
  }
}

bool ModuleManager::startModule(PluginType type) {
  for (const auto &plugin : m_plugins) {
    if (plugin->getStaticType() == type) {
     auto res =  plugin->start();
     if(!res)
         return true;
     else
         std::cout << res.message() << std::endl;
    }
  }
  return false;
}

bool ModuleManager::stopModule(PluginType type) {
  for (const auto &plugin : m_plugins) {
    if (plugin->getStaticType() == type) {
      plugin->shutdown();
      return true;
    }
  }
  return false;
}

void ModuleManager::shutdown() {
  for (const auto &plugin : m_plugins) {
    plugin->shutdown();
  }
}
} // namespace NAssist