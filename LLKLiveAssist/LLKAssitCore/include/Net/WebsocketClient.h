#pragma once

#include <stack>
#include <functional>
#include <map>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>

#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/ssl.hpp>




namespace NAssist
{
    
    class WebsocketClient : public std::enable_shared_from_this<WebsocketClient>
    {
    public:

        enum class FunctionType
        {
            OnConnect,
            OnReceive,
            OnSend,
            
        };
		
        enum class ClientState : uint8_t {
            stoped = 0,
            started = 1,
            ssl_link = 2,
            bad_link = 4,

            
        };
		enum class ProtocolType
        {
            WSS ,
            WS
        };

        WebsocketClient(const std::string& host);
        ~WebsocketClient();
        void Connect();
        void Send(const std::vector<uint8_t>& data);
        void Receive();
		
        boost::beast::flat_buffer GetBuffer() { return m_buffer; }
        static std::shared_ptr<WebsocketClient> Create(const std::string& host);

    private:

        uint8_t m_state ;

        ProtocolType m_protocolType;
		std::string m_Host;
        std::string m_Port;
        std::string m_Target;

		boost::asio::io_context m_io_ctx;
		boost::asio::ssl::context m_ssl_ctx;
        boost::asio::ip::tcp::resolver m_resolver;
        boost::beast::flat_buffer m_buffer;
        
        //sync stream
        boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>> m_sync_stream;
            

        std::map<FunctionType,std::function<void()>> m_function_map;
        

		
    };

	
	class AsyncWebsocketClient : public std::enable_shared_from_this<AsyncWebsocketClient> 
	{
	public:

		enum class FunctionType
		{
			OnConnect,
			OnReceive,
			OnSend,

		};

		enum class ClientState : uint8_t {
			stoped = 0,
			started = 1,
			ssl_link = 2,
			bad_link = 4,


		};
		enum class ProtocolType
		{
			WSS,
			WS
		};

		AsyncWebsocketClient(const std::string& host);
		~AsyncWebsocketClient();
		

		void AsyncConnect(std::function<void()>func = nullptr);
		void AsyncSend(const std::vector<uint8_t>& data, std::function<void(boost::beast::error_code, std::size_t)> func);
		void AsyncReceive(std::function<void(boost::beast::error_code, std::size_t)> func);
		
		void Send(const std::vector<uint8_t>& data);
		void Receive();

		void StartAsyncTask();
		void CommitAsyncTask();
		
		
		void AddFunc(FunctionType type, std::function<void()> func);
		

		boost::beast::flat_buffer GetBuffer() { return m_buffer; }
		void CleanBuffer() { m_buffer.clear(); }
		static std::shared_ptr<AsyncWebsocketClient> Create(const std::string& host);

	private:
		void on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
		void on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type ep);
		void on_ssl_handshake(boost::beast::error_code ec);
		void on_handshake(boost::beast::error_code ec);





		

	private:

		uint8_t m_state;

		ProtocolType m_protocolType;
		std::string m_Host;
		std::string m_Port;
		std::string m_Target;

		boost::asio::io_context m_io_ctx;
		boost::asio::ssl::context m_ssl_ctx;
		boost::asio::ip::tcp::resolver m_resolver;
		boost::beast::flat_buffer m_buffer;
		boost::asio::steady_timer m_timer;

		//async stream
		boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> m_async_stream;
		
		std::map<FunctionType, std::function<void()>> m_function_map;



	};

}