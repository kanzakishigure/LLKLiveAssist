#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/ssl.hpp>
#include <functional>



namespace NAssist
{
    enum class HttpRequestMethod
    {
        unknown = 0,
        delete_,
        get,
        head,
        post,
        put,
        connect,
        options,
        trace,
    };
	enum class HttpRequestType
	{
		http,
        https
	};
    class HttpRequest : public std::enable_shared_from_this<HttpRequest>
    {
        
    public:

        HttpRequest(const std::string& host, const std::string& uri, HttpRequestMethod request_method);
        ~HttpRequest();
        void AddHeader(const std::string& key,const std::string& value);
        
        void ClearHeader();
        void SetContent(const std::string& data);
        void SetTarget(const std::string& url);
        std::string Receive();
        void AsyncReceive(std::function<void(boost::beast::error_code,std::size_t)> callback);
        const boost::beast::http::response<boost::beast::http::dynamic_body>& GetRespons(){return m_res;}

        
        static std::shared_ptr<HttpRequest> CreateRequest(const std::string& host, const std::string& uri, HttpRequestMethod request_method);

    private:
        std::string m_Host;
        std::string m_Uri;
        
        HttpRequestType request_type = HttpRequestType::http;

        boost::asio::io_context m_io_ctx;
        boost::beast::tcp_stream m_tcp_stream;
        //use for ssl
        boost::asio::ssl::context m_ssl_ctx;
        boost::beast::ssl_stream<boost::beast::tcp_stream> m_ssl_stream;


        boost::beast::flat_buffer m_buffer;
        boost::beast::http::request< boost::beast::http::string_body> m_req;
        boost::beast::http::response<boost::beast::http::dynamic_body> m_res;

    
    };
}
