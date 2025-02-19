#pragma once
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>

#include "Core/logger.h"
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace NAssist {

class PeriodicTask : public std::enable_shared_from_this<PeriodicTask> {
public:
  PeriodicTask(std::function<void()> task, int interval);
  ~PeriodicTask();
  void Start();
  void Stop();
  void CommitTask();

private:
  void onTimerEnd(const boost::system::error_code &e);
  std::function<void()> m_task;
  int m_interval;
  bool m_isRunning;

  boost::asio::io_context m_io_ctx;
  boost::asio::steady_timer m_timer;
};
} // namespace NAssist