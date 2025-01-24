#pragma once

namespace NAssist {

enum class PluginType {
  None = 0,
  Broadcast,
  GSoVITS,
  BiliClient,
};
class PluginBase {
public:
  virtual void init() = 0;
  virtual void start() = 0;
  virtual void shutdown() = 0;
  virtual void drawUI() = 0;

  virtual PluginType getStaticType() { return m_type; }

private:
  PluginType m_type = PluginType::None;
};
} // namespace NAssist
