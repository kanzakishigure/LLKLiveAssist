#include "GUIConfig.h"
#include <Core/logger.h>
#include <cstdint>
namespace boost::json {


NAssist::RuntimeConfig
tag_invoke(const boost::json::value_to_tag<NAssist::RuntimeConfig> &,
           boost::json::value const &jv) {
  NAssist::RuntimeConfig config;
  try {

    if (jv.is_null()) {
      throw std::runtime_error("invalid json_value to NAssist::GUIConfig");
    }
    config.theme_mode =
        static_cast<NAssist::ThemeMode>(jv.at("theme_mode").as_int64());
    config.enable_mica = jv.at("enable_mica").as_bool();
    config.enabel_logger = jv.at("enabel_logger").as_bool();
    config.enable_remote_server = jv.at("enable_remote_server").as_bool();
    config.navigation_displaymode = static_cast<NAssist::NavigationDisplayMode>(
        jv.at("navigation_displaymode").as_int64());
    config.remote_server_url = jv.at("remote_server_url").as_string();

  } catch (const std::exception &e) {
    RUNTIME_ERROR("invalid json_value to NAssist::GUIConfig : {}", e.what());
  }
  return config;
}
void tag_invoke(const value_from_tag &, value &jv,
                NAssist::RuntimeConfig const &config) {

  jv = {
      {"theme_mode", static_cast<uint32_t>(config.theme_mode)},
      {"enable_mica", config.enable_mica},
      {"enabel_logger", config.enabel_logger},
      {"enable_remote_server", config.enable_remote_server},
      {"navigation_displaymode", static_cast<uint32_t>(config.navigation_displaymode)},
      {"remote_server_url", config.remote_server_url},
  };
}
} // namespace boost::json