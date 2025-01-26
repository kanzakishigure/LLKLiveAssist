#pragma once
#include "PluginBase.h"
#include <thread>
#include <queue>



struct ma_decoder;
struct ma_device_config;
struct ma_device;

namespace NAssist {
class BroadcastAssist : public PluginBase {
public:
  virtual void init() override;
  virtual void start() override;
  virtual void drawUI() override;
  virtual void shutdown() override;

private:

  
	
};
} // namespace NAssist
