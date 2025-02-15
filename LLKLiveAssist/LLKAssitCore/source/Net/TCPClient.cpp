#include "Net/TCPClient.h"

namespace NAssist {
TCPClient::TCPClient(std::string url, uint64_t port)
    : m_resolver(m_io_ctx), m_socket(m_io_ctx) {
  m_endpoints = m_resolver.resolve(url, std::to_string(port));
}

bool TCPClient::try_connect() {
  try {

    // 尝试连接
    boost::asio::connect(m_socket, m_endpoints);
    m_socket.close();
    return true;
  } catch (const std::exception &e) {
    return false;
  }
}
} // namespace NAssist