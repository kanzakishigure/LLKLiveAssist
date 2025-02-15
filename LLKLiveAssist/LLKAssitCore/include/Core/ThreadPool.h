#pragma once
#include <Boost/asio.hpp>
#include <thread>
#include <vector>

namespace NAssist {
class ThreadPool {
public:
  ThreadPool();
  ~ThreadPool();
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  static ThreadPool &GetInstance() {
    static ThreadPool instance;
    return instance;
  }

private:
  std::vector<boost::asio::io_context> m_io_contexts;
  std::vector<std::thread> m_threads;
};
}; // namespace NAssist