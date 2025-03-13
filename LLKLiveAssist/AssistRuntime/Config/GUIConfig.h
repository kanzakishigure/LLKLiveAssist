#pragma once
#include "Data/JsonParser.h"
#include <cstdint>

namespace NAssist {

enum class NavigationDisplayMode : uint32_t {
  Auto = 0x0000,
  Minimal = 0x0001,
  Compact = 0x0002,
  Maximal = 0x0003,
};
enum class ThemeMode : uint32_t {
  Light = 0x0000,
  Dark = 0x0001,
};
struct RuntimeConfig : public Serializeable {

  ThemeMode theme_mode;
  bool enable_mica;
  bool enabel_logger = false;
  bool enable_remote_server;
  NavigationDisplayMode navigation_displaymode;
  std::string remote_server_url;
};
} // namespace NAssist

namespace boost::json {
NAssist::RuntimeConfig
tag_invoke(const boost::json::value_to_tag<NAssist::RuntimeConfig> &,
           boost::json::value const &jv);
void tag_invoke(const value_from_tag &, value &jv,
                NAssist::RuntimeConfig const &config);

} // namespace boost::json