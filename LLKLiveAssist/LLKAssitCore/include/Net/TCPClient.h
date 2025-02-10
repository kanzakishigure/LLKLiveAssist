#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <boost/asio.hpp>
namespace NAssist {
    class TCPClient
    {
        public:
        TCPClient(std::string url,uint64_t port);

        bool try_connect();
        private:
        boost::asio::io_context m_io_ctx;
        boost::asio::ip::tcp::resolver m_resolver;
        boost::asio::ip::tcp::socket m_socket;
        boost::asio::ip::tcp::resolver::results_type m_endpoints;

    };
}