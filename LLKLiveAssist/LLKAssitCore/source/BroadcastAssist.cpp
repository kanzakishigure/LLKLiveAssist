#include "BroadcastAssist.h"
#include <iostream>

namespace NAssist {
void BroadcastAssist::init() { std::cout << "BroadcastAssist Init \n"; }

void BroadcastAssist::shutdown() {
  std::cout << "BroadcastAssist Shutdown function\n";
}

void BroadcastAssist::drawUI() {
  
}

void BroadcastAssist::start() {
  std::cout << "BroadcastAssist Start function\n";
}
} // namespace NAssist