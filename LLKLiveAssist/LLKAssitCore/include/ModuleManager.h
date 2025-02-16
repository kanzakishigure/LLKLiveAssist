#pragma once
#include "PluginBase.h"
#include <memory>
#include <system_error>
#include <variant>
#include <vector>


#include "AudioAssist.h"
#include "BiliClientAssist.h"
#include "Core/ErrorCode.h"
#include "GSoVITSAssist.h"

namespace NAssist {
class ModuleManager {
public:
  ModuleManager() = default;
  ~ModuleManager() { shutdown();}

  void shutdown();

  std::variant<bool, std::error_code>
  addPlugin(std::shared_ptr<PluginBase> plugin);
  std::error_code startAllModule();
  bool startModule(PluginType type);

  void stopAllModule();
  bool stopModule(PluginType type);

  template <typename T>
  std::variant<std::shared_ptr<T>, std::error_code> tryGetModule() {
    if (!std::is_base_of<PluginBase, T>()) {
      return std::make_error_code(std::errc::wrong_protocol_type);
    }
    for (auto &plugin : m_plugins) {
      if (plugin->getType() == T::getStaticType()) {
        return std::dynamic_pointer_cast<T>(plugin);
      }
    }
    return std::make_error_code(std::errc::wrong_protocol_type);
  }

  template <typename T> std::shared_ptr<T> getModule() {
    auto res = tryGetModule<T>();
    if (res.index() == 1) {
      return nullptr;
    }
    return std::get<0>(res);
  }

  inline static ModuleManager &getInstance() {
    static ModuleManager instance;
    return instance;
  }

private:
  std::vector<std::shared_ptr<PluginBase>> m_plugins;
};
} // namespace NAssist