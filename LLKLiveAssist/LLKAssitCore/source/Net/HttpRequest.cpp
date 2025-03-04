#include "Net/HttpRequest.h"

#include <boost/beast/http.hpp>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

namespace NAssist {

// URL编码辅助函数
std::string url_encode(const std::string &value) {
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (char c : value) {
    // 保留字母数字及其他特定字符
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
      continue;
    }
    // 其他字符转换为%XX形式
    escaped << '%' << std::setw(2)
            << static_cast<int>(static_cast<unsigned char>(c));
  }

  return escaped.str();
}

std::shared_ptr<HttpRequest>
HttpRequest::CreateRequest(const std::string &url, const std::string &uri,
                           HttpRequestMethod request_method) {
  return std::make_shared<HttpRequest>(url, uri, request_method);
}

HttpRequest::HttpRequest(const std::string &url, const std::string &uri,
                         const HttpRequestMethod request_method)
    : m_Host(url), m_Uri(uri), m_tcp_stream(m_io_ctx),
      m_ssl_ctx(boost::asio::ssl::context::tlsv12_client),
      m_ssl_stream(m_io_ctx, m_ssl_ctx) {

  try {
    const std::string http_protocol = "http://";
    const std::string https_protocol = "https://";
    auto version = 11;
    std::string port;

    // These objects perform our I/O

    auto resolve_host = url;

    if (resolve_host.find(http_protocol) != std::string::npos) {
      port = "80";
      resolve_host = resolve_host.substr(
          resolve_host.find(http_protocol) + http_protocol.length(),
          resolve_host.length() - http_protocol.length());
    }
    if (resolve_host.find(https_protocol) != std::string::npos) {
      port = "443";
      request_type = HttpRequestType::https;
      resolve_host = resolve_host.substr(
          resolve_host.find(https_protocol) + https_protocol.length(),
          resolve_host.length() - https_protocol.length());
    }

    boost::asio::ip::tcp::resolver resolver(m_io_ctx);
    m_Host = resolve_host;
    if (m_Host.find_first_of(':') != std::string::npos) {
      size_t index = m_Host.find_first_of(':');
      port = m_Host.substr(index + 1, m_Host.size() - index - 1);
      m_Host = m_Host.substr(0, index);
    }
    // test ssl
    switch (request_type) {
    case NAssist::HttpRequestType::http: {
      // Look up the domain name
      auto const results = resolver.resolve(m_Host, port);

      // Make the connection on the IP address we get from a lookup
      m_tcp_stream.connect(results);
    } break;
    case NAssist::HttpRequestType::https: {
      m_ssl_ctx.set_default_verify_paths();

      if (!SSL_set_tlsext_host_name(m_ssl_stream.native_handle(),
                                    m_Host.data())) {
        boost::beast::error_code ec{static_cast<int>(::ERR_get_error()),
                                    boost::asio::error::get_ssl_category()};
        throw boost::beast::system_error{ec};
      }
      // Look up the domain name
      auto const results = resolver.resolve(m_Host, port);
      // Make the connection on the IP address we get from a lookup
      boost::beast::get_lowest_layer(m_ssl_stream).connect(results);

      // Perform the SSL handshake
      m_ssl_stream.handshake(boost::asio::ssl::stream_base::client);
    } break;
    }

    // Set up an HTTP POST request message
    m_req = boost::beast::http::request<boost::beast::http::string_body>{
        boost::beast::http::verb(request_method), m_Uri, version};
    m_req.set(boost::beast::http::field::host, m_Host);
    m_req.set(boost::beast::http::field::user_agent,
              BOOST_BEAST_VERSION_STRING);
    m_req.set(boost::beast::http::field::connection, "Keep-Alive");

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

HttpRequest::~HttpRequest() {
  try {

    boost::beast::error_code ec;
    switch (request_type) {
    case NAssist::HttpRequestType::http: {
      // Gracefully close the socket

      m_tcp_stream.socket().shutdown(
          boost::asio::ip::tcp::socket::shutdown_both, ec);
      // not_connected happens sometimes
      // so don't bother reporting it.
      //
      if (ec && ec != boost::beast::errc::not_connected)
        throw boost::beast::system_error{ec};
    } break;
    case NAssist::HttpRequestType::https: {
      m_ssl_stream.shutdown(ec);
      if (ec) {
        throw boost::beast::system_error{ec};
      }
    } break;
    default:
      break;
    }
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void HttpRequest::AddHeader(const std::string &key, const std::string &value) {
  m_req.insert(key, value);
}
void HttpRequest::AddRequestParameter(const std::string &key,
                                      const std::string &value) {
  m_RequestParameter.emplace_back() = {key, value};
}
void HttpRequest::ClearHeader() { m_req.clear(); }

void HttpRequest::SetContent(const std::string &data) {
  m_req.body() = data;

  m_req.prepare_payload();
}

void HttpRequest::SetTarget(const std::string &url) { m_req.target(url); }

std::string HttpRequest::Receive() {
  // Send the HTTP request to the remote host
  std::string result;
  if (!m_RequestParameter.empty()) {
    std::string query = m_Uri;
    for (auto const &[key, value] : m_RequestParameter) {
      if (!query.empty())
        query += "&";
      query += url_encode(key) + "=" + url_encode(value);
    }
    std::string target = m_Uri += "?" + query;
    m_req.target(target);
  }
  try {

    switch (request_type) {
    case NAssist::HttpRequestType::http: {
      boost::beast::http::write(m_tcp_stream, m_req);

      // Receive the HTTP response
      boost::beast::http::read(m_tcp_stream, m_buffer, m_res);

      // Write the message to standard out

    } break;
    case NAssist::HttpRequestType::https: {
      boost::beast::http::write(m_ssl_stream, m_req);

      // Receive the HTTP response
      boost::beast::http::read(m_ssl_stream, m_buffer, m_res);

      // Write the message to standard out

    } break;
    }

    result = boost::beast::buffers_to_string(m_res.body().data());

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return result;
}

void HttpRequest::AsyncReceive(
    std::function<void(boost::beast::error_code, std::size_t)> callback) {
  // Send the HTTP request to the remote host

  switch (request_type) {
  case NAssist::HttpRequestType::http: {
    boost::beast::http::write(m_tcp_stream, m_req);

    // Receive the HTTP response
    boost::beast::http::async_read(m_tcp_stream, m_buffer, m_res, callback);

    // Write the message to standard out

  } break;
  case NAssist::HttpRequestType::https: {
    boost::beast::http::write(m_ssl_stream, m_req);

    // Receive the HTTP response
    boost::beast::http::async_read(m_ssl_stream, m_buffer, m_res, callback);

    // Write the message to standard out

  } break;
  }
  m_io_ctx.run();
}
} // namespace NAssist