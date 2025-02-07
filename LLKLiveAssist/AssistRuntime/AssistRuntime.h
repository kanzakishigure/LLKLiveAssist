#pragma once
#include "PluginBase.h"
#include "ModuleManager.h"
namespace NAssist {
class AssistRuntime {
public:
  AssistRuntime() = default;
  ~AssistRuntime() ;
  virtual void init();
  virtual void shutdown();
  virtual void run();

private:
  

  bool closed = false;
 
};
} // namespace NAssist
