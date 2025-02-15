#include "Net/WebsocketClient.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace NAssist {

void fail(boost::beast::error_code ec, char const *what) {
  std::cerr << what << ": " << ec.message() << "\n";
}

std::shared_ptr<WebsocketClient>
WebsocketClient::Create(const std::string &host) {
  return std::make_shared<WebsocketClient>(host);
}

WebsocketClient::WebsocketClient(const std::string &host)
    : m_Host(host), m_ssl_ctx(boost::asio::ssl::context::tlsv12_client),
      m_sync_stream(m_io_ctx, m_ssl_ctx),
      m_resolver(boost::asio::make_strand(m_io_ctx)), m_state(0) {
  try {
    std::string wss_proto_uri = "wss://";
    std::string ws_proto_uri = "ws://";

    if (host.find_first_of(wss_proto_uri) != std::string::npos) {
      m_protocolType = ProtocolType::WSS;
      m_Host =
          host.substr(wss_proto_uri.size(), host.size() - wss_proto_uri.size());
    } else if (host.find_first_of(ws_proto_uri) != std::string::npos) {
      m_protocolType = ProtocolType::WS;
      m_Host =
          host.substr(ws_proto_uri.size(), host.size() - ws_proto_uri.size());
    } else {
      throw std::runtime_error("invaild Protocol");
    }

    switch (m_protocolType) {
    case NAssist::WebsocketClient::ProtocolType::WSS: {
      size_t index = m_Host.find_first_of(':');
      std::string pram = m_Host.substr(index + 1, m_Host.size() - index - 1);
      m_Host = m_Host.substr(0, index);
      index = pram.find_first_of('/');
      m_Target = "/";
      m_Port = pram;
      if (index != std::string::npos) {
        m_Target = pram.substr(index, pram.size() - index);
        m_Port = pram.substr(0, index);
      }

      // The SSL context is required, and holds certificates
      m_ssl_ctx.set_default_verify_paths();

    } break;
    case NAssist::WebsocketClient::ProtocolType::WS: {

    } break;
    }

  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

WebsocketClient::~WebsocketClient() {

  m_sync_stream.close(boost::beast::websocket::close_code::normal);
}

void WebsocketClient::Connect() {

  try {
    m_resolver = boost::asio::ip::tcp::resolver(m_io_ctx);
    auto results = m_resolver.resolve(m_Host, m_Port);

    auto ep = boost::asio::connect(
        boost::beast::get_lowest_layer(m_sync_stream), results);

    if (!SSL_set_tlsext_host_name(m_sync_stream.next_layer().native_handle(),
                                  m_Host.c_str()))
      throw boost::beast::system_error(
          boost::beast::error_code(static_cast<int>(::ERR_get_error()),
                                   boost::asio::error::get_ssl_category()),
          "Failed to set SNI Hostname");

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    m_Host += ':' + std::to_string(ep.port());

    // Perform the SSL handshake
    m_sync_stream.next_layer().handshake(boost::asio::ssl::stream_base::client);

    // Set a decorator to change the User-Agent of the handshake
    m_sync_stream.set_option(boost::beast::websocket::stream_base::decorator(
        [](boost::beast::websocket::request_type &req) {
          req.set(boost::beast::http::field::user_agent,
                  std::string(BOOST_BEAST_VERSION_STRING) +
                      " websocket-client-coro");
        }));
    // Perform the websocket handshake
    m_sync_stream.handshake(m_Host, m_Target);
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void WebsocketClient::Send(const std::vector<uint8_t> &data) {
  size_t size = 0;
  try {
    size = m_sync_stream.write(boost::asio::buffer(data));
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void WebsocketClient::Receive() {
  try {

    m_sync_stream.read(m_buffer);

    std::cout << boost::beast::make_printable(m_buffer.data()) << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

AsyncWebsocketClient::AsyncWebsocketClient(const std::string &host)
    : m_Host(host), m_timer(m_io_ctx, boost::asio::chrono::seconds(0)),
      m_ssl_ctx(boost::asio::ssl::context::tlsv12_client),
      m_async_stream(boost::asio::make_strand(m_io_ctx), m_ssl_ctx),
      m_resolver(boost::asio::make_strand(m_io_ctx)), m_state(0) {
  try {
    std::string wss_proto_uri = "wss://";
    std::string ws_proto_uri = "ws://";

    if (host.find_first_of(wss_proto_uri) != std::string::npos) {
      m_protocolType = ProtocolType::WSS;
      m_Host =
          host.substr(wss_proto_uri.size(), host.size() - wss_proto_uri.size());
    } else if (host.find_first_of(ws_proto_uri) != std::string::npos) {
      m_protocolType = ProtocolType::WS;
      m_Host =
          host.substr(ws_proto_uri.size(), host.size() - ws_proto_uri.size());
    } else {
      throw std::runtime_error("invaild Protocol");
    }

    switch (m_protocolType) {
    case NAssist::AsyncWebsocketClient::ProtocolType::WSS: {
      size_t index = m_Host.find_first_of(':');
      std::string pram = m_Host.substr(index + 1, m_Host.size() - index - 1);
      m_Host = m_Host.substr(0, index);
      index = pram.find_first_of('/');
      m_Target = "/";
      m_Port = pram;
      if (index != std::string::npos) {
        m_Target = pram.substr(index, pram.size() - index);
        m_Port = pram.substr(0, index);
      }

      // The SSL context is required, and holds certificates
      m_ssl_ctx.set_default_verify_paths();

    } break;
    case NAssist::AsyncWebsocketClient::ProtocolType::WS: {

    } break;
    }

  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

AsyncWebsocketClient::~AsyncWebsocketClient() {
  m_async_stream.async_close(boost::beast::websocket::close_code::normal,
                             [](boost::beast::error_code ec) {
                               if (ec)
                                 return fail(ec, "close");
                             });
}

void AsyncWebsocketClient::AsyncConnect(
    std::function<void()> func /*= nullptr*/) {
  if (func != nullptr) {
    m_function_map[FunctionType::OnConnect] = func;
  }

  m_resolver.async_resolve(
      m_Host, m_Port,
      boost::beast::bind_front_handler(&AsyncWebsocketClient::on_resolve,
                                       shared_from_this()));
  StartAsyncTask();
}

void AsyncWebsocketClient::AsyncSend(
    const std::vector<uint8_t> &data,
    std::function<void(boost::beast::error_code, std::size_t)> func) {
  m_async_stream.async_write(boost::asio::buffer(data), func);
}

void AsyncWebsocketClient::AsyncReceive(
    std::function<void(boost::beast::error_code, std::size_t)> func) {
  m_async_stream.async_read(m_buffer, func);
}

void AsyncWebsocketClient::Send(const std::vector<uint8_t> &data) {
  size_t size = 0;
  try {
    size = m_async_stream.write(boost::asio::buffer(data));
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

std::vector<uint8_t> AsyncWebsocketClient::Receive() {
  try {
    m_buffer.clear();
    m_async_stream.read(m_buffer);
    std::string data = boost::beast::buffers_to_string(m_buffer.data());
    return std::vector<uint8_t>(data.begin(), data.end());
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return {};
}

void AsyncWebsocketClient::StartAsyncTask() { m_io_ctx.run(); }

void AsyncWebsocketClient::CommitAsyncTask() {
  m_io_ctx.run();
  m_io_ctx.poll();
}

void AsyncWebsocketClient::AddFunc(FunctionType type,
                                   std::function<void()> func) {
  m_function_map[type] = func;
}

std::shared_ptr<AsyncWebsocketClient>
AsyncWebsocketClient::Create(const std::string &host) {
  return std::make_shared<AsyncWebsocketClient>(host);
}

void AsyncWebsocketClient::on_resolve(
    boost::beast::error_code ec,
    boost::asio::ip::tcp::resolver::results_type results) {
  if (ec)
    return fail(ec, "resolve");
  // Set a timeout on the operation
  boost::beast::get_lowest_layer(m_async_stream)
      .expires_after(std::chrono::seconds(30));

  // Make the connection on the IP address we get from a lookup
  boost::beast::get_lowest_layer(m_async_stream)
      .async_connect(
          results, boost::beast::bind_front_handler(
                       &AsyncWebsocketClient::on_connect, shared_from_this()));
}

void AsyncWebsocketClient::on_connect(
    boost::beast::error_code ec,
    boost::asio::ip::tcp::resolver::results_type::endpoint_type ep) {
  if (ec)
    return fail(ec, "connect");

  // Set a timeout on the operation
  boost::beast::get_lowest_layer(m_async_stream)
      .expires_after(std::chrono::seconds(30));

  // Set SNI Hostname (many hosts need this to handshake successfully)
  if (!SSL_set_tlsext_host_name(m_async_stream.next_layer().native_handle(),
                                m_Host.c_str())) {
    ec = boost::beast::error_code(static_cast<int>(::ERR_get_error()),
                                  boost::asio::error::get_ssl_category());
    return fail(ec, "connect");
  }

  // Update the host_ string. This will provide the value of the
  // Host HTTP header during the WebSocket handshake.
  // See https://tools.ietf.org/html/rfc7230#section-5.4
  m_Host += ':' + std::to_string(ep.port());

  // Perform the SSL handshake
  m_async_stream.next_layer().async_handshake(
      boost::asio::ssl::stream_base::client,
      boost::beast::bind_front_handler(&AsyncWebsocketClient::on_ssl_handshake,
                                       shared_from_this()));
}

void AsyncWebsocketClient::on_ssl_handshake(boost::beast::error_code ec) {
  if (ec)
    return fail(ec, "ssl_handshake");

  // Turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  boost::beast::get_lowest_layer(m_async_stream).expires_never();

  // Set suggested timeout settings for the websocket
  m_async_stream.set_option(
      boost::beast::websocket::stream_base::timeout::suggested(
          boost::beast::role_type::client));

  // Set a decorator to change the User-Agent of the handshake
  m_async_stream.set_option(boost::beast::websocket::stream_base::decorator(
      [](boost::beast::websocket::request_type &req) {
        req.set(boost::beast::http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                    " websocket-client-async-ssl");
      }));

  // Perform the websocket handshake
  m_async_stream.async_handshake(
      m_Host, m_Target,
      boost::beast::bind_front_handler(&AsyncWebsocketClient::on_handshake,
                                       shared_from_this()));
}

void AsyncWebsocketClient::on_handshake(boost::beast::error_code ec) {
  if (ec)
    return fail(ec, "handshake");

  m_state = m_state | static_cast<uint8_t>(ClientState::started);

  if (m_function_map.find(FunctionType::OnConnect) != m_function_map.end()) {
    m_function_map[FunctionType::OnConnect]();
  }
  std::cout << " success handshake" << std::endl;
}

} // namespace NAssist
