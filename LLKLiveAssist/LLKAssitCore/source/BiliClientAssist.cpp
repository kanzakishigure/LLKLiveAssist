#include "BiliClientAssist.h"
#include <iostream>
#include <sstream>
#include <string>
#include <format>
#include <map>
#include <random>
#include <chrono>
#include <thread>

#include "Data/ProtoPacket.h"

#include <boost/asio.hpp>


namespace NAssist {

static std::string OpenLiveDomain  = "https://live-open.biliapi.com";
const std::string k_InteractivePlayStart = "/v2/app/start";
const std::string k_InteractivePlayEnd = "/v2/app/end";
const std::string k_InteractivePlayBatchHeartBeat = "/v2/app/batchHeartbeat";
const std::string k_InteractivePlayHeartBeat = "/v2/app/heartbeat";

void BiliClientAssist::init() {
    std::cout << "BiliClientAssist Init\n";
    //TODO: initiate the data from json file.
    m_AccessKey = "BP2Yl3cBDNc225qmrK53Fsyv";
    m_AccessKeySecret = "idiGra89obnAzmlZNjWRnw9jDi6vY6";
    m_AppId = "1739800103262";
    m_Code = "BVJSMTS1T88W9";
    //SetConsoleOutputCP(CP_UTF8);
    
}
void BiliClientAssist::shutdown() {}
void BiliClientAssist::drawUI() {
 
   


}
void BiliClientAssist::start() {
  std::cout << "BiliClientAssist Start function\n";
  StartInteractivePlay();
  AuthWebsocket();
  


  m_AppHeartPeriodicTask = std::make_shared<PeriodicTask>([this]() {

	  std::cout << "app heartbeat" << std::endl;
	  HeartBeatInteractivePlay(m_AppStartInfo.GameInfo.GameId);


	  }, 20);

  m_GameHeartPeriodicTask = std::make_shared<PeriodicTask>([this]() {

	  std::cout << "game heartbeat" << std::endl;
	  HeartBeatWebsocketClient();
      
	  }, 30);
   appthread  = std::thread([this]() {m_AppHeartPeriodicTask->Start(); });
   gamethread = std::thread([this]() {m_GameHeartPeriodicTask->Start(); });
   msgthread = std::thread([this]() { while (true)
   {
       WebsocketClientReceive();
   } });
   
}

void BiliClientAssist::StartInteractivePlay() {

    std::string param = (std::stringstream() << "{\"code\":" << "\"" << m_Code << "\"" << ",\"app_id\":" << m_AppId << "}").str();
    auto json_value = RequestWebUTF8(OpenLiveDomain, k_InteractivePlayStart, HttpRequestMethod::post, param);


    m_AppStartInfo = boost::json::value_to<AppStartInfo>(json_value);
}

void BiliClientAssist::EndInteractivePlay()
{
	std::string param = (std::stringstream() << "{\"game_id\":" << "\"" << m_AppStartInfo.GameInfo.GameId << "\"" << ",\"app_id\":" << m_AppId << "}").str();
	auto json_value = RequestWebUTF8(OpenLiveDomain, k_InteractivePlayEnd, HttpRequestMethod::post, param);
}

void BiliClientAssist::HeartBeatInteractivePlay(std::string gameId)
{
	std::string param = (std::stringstream() << "{\"game_id\":" << "\"" << gameId << "\"" << "}").str();
	auto json_value = RequestWebUTF8(OpenLiveDomain, k_InteractivePlayHeartBeat, HttpRequestMethod::post, param);
}

boost::json::value BiliClientAssist::RequestWebUTF8(std::string host , std::string uri, HttpRequestMethod request_method, std::string param, std::string cookie /*= ""*/)
{
    if(!cookie.empty())
    {
        std::cout<<"bilibili cookie not support cookie"<<std::endl;  
    }
    //bilibili鉴权 header
    auto now_ms = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    auto now_s = std::chrono::duration_cast<std::chrono::seconds>(now_ms);


	
	std::mt19937 urbg{ static_cast<unsigned int>(now_s.count()) };
	std::uniform_int_distribution<int> distr1{ 1, 100000 };

    std::string timestamp = std::to_string(now_s.count());
    std::string nonce = std::to_string(distr1(urbg));
    std::string md5data = MD5Encryption::getMd5(param);

    
    
    std::map<std::string, std::string> head_map
    {
        { "x-bili-timestamp", timestamp },
        { "x-bili-signature-method" ,"HMAC-SHA256" },
        { "x-bili-signature-nonce" ,nonce },
        { "x-bili-accesskeyid", m_AccessKey},
        { "x-bili-signature-version","1.0"},
        { "x-bili-content-md5" , md5data}
    };
    auto head_list = SortHeadMap(head_map);
    
    std::string signstr = {};
    for (size_t index = 0;index<head_list.size();index++)
    {
        if (index != head_list.size() - 1)
        {
            signstr += head_list[index] + "\n";
        }
        else
        {
            signstr += head_list[index];
        }
       
    }

    //使用 hmac-sha265进行签名
    auto signature = HMACEncryption::SHA256(m_AccessKeySecret, signstr);
    head_map["Authorization"] = signature;
	head_map["Content-Type"] = "application/json";
	head_map["Accept"] = "application/json";
    
    //创建请求，接受响应
    
    m_bili_request = HttpRequest::CreateRequest(host, uri, request_method);
    m_bili_request->SetContent(param);
    for (const auto& head : head_map)
    {
        m_bili_request->AddHeader(head.first, head.second);
    }
    auto data = m_bili_request->Receive();

    std::cout << data << std::endl;
    //parse the data to json data
    boost::json::value result;
    try
    {
        result =  boost::json::parse(data);
    }
    catch (std::bad_alloc const& e)
    {
        std::cout << "Parsing failed: " << e.what() << "\n";
    }
    
    return result;
     
     
}

void BiliClientAssist::AuthWebsocket()
{
	ProtoPacket packet;
	packet.header.packet_length = m_AppStartInfo.WebsocketInfo.AuthBody.size() + packet.header.header_length;
	packet.header.operation = ProtoOperation::Authority;
	packet.header.version = ProtoVersion::HeartBeat;
	packet.header.sequence_id = 1;
	packet.body = std::vector<uint8_t>(m_AppStartInfo.WebsocketInfo.AuthBody.begin(), m_AppStartInfo.WebsocketInfo.AuthBody.end());
	std::vector<uint8_t> bytes = ProtoPacket2bytes(packet);



	m_bili_websocket = AsyncWebsocketClient::Create(m_AppStartInfo.WebsocketInfo.WssLink.front());
    
	m_bili_websocket->AsyncConnect([this, bytes]() {


		m_bili_websocket->AsyncSend(bytes, [this](boost::beast::error_code ec, std::size_t bytes_transferred) {
			if (ec)
				std::cout << ec.what();

			m_bili_websocket->AsyncReceive([this](boost::beast::error_code ec, std::size_t bytes_transferred) {
				if (ec)
					std::cout << ec.what();
                std::cout << boost::beast::make_printable(m_bili_websocket->GetBuffer().data()) << std::endl;
				});

			});


		});

	m_bili_websocket->CommitAsyncTask();
}

void BiliClientAssist::HeartBeatWebsocketClient()
{
	ProtoPacket packet;
	packet.header.packet_length = packet.header.header_length;
	packet.header.operation = ProtoOperation::HeartBeat;
	packet.header.version = ProtoVersion::HeartBeat;
	packet.header.sequence_id = 1;
    std::vector<uint8_t> bytes = ProtoPacket2bytes(packet);
    m_bili_websocket->Send(bytes);
    std::cout << "game heartbeat success" << std::endl;
    
}

void BiliClientAssist::WebsocketClientReceive()
{
	/*
	m_bili_websocket->AsyncReceive([this](boost::beast::error_code ec, std::size_t bytes_transferred) {
		if (ec)
			std::cout << ec.what();

		std::cout << boost::beast::make_printable(m_bili_websocket->GetBuffer().data()) << std::endl;
		m_bili_websocket->CleanBuffer();

		WebsocketClientReceive();
		});
	m_bili_websocket->CommitAsyncTask();
    
    */
    m_bili_websocket->Receive();
    m_bili_websocket->CleanBuffer();
    
}

std::vector<std::string> BiliClientAssist::SortHeadMap(std::map<std::string, std::string> head_map)
{
    std::vector<std::string> head_list;
    for (auto& head_pair : head_map)
    {
        head_list.emplace_back() = head_pair.first + ":" + head_pair.second;
    }
	std::sort(head_list.begin(), head_list.end(), [](std::string s1, std::string s2) { return s1 < s2;});
    return head_list;
}

}; // namespace NAssist


