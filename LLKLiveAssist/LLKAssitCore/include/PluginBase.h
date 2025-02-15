#pragma once
#include "Core/ErrorCode.h"
namespace NAssist {

enum class PluginType {
  None = 0,
  Audio,
  GSoVITS,
  BiliClient,
};
class PluginBase {
public:
  virtual void init() = 0;
  virtual std::error_code start() = 0;
  virtual void shutdown() = 0;
  virtual void drawUI() = 0;

  virtual PluginType getType() { return PluginType::None; }
  static PluginType getStaticType() { return PluginType::None; }
};
} // namespace NAssist
