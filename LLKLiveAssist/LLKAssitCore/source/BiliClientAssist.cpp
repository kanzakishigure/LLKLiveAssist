#include "BiliClientAssist.h"
#include <chrono>

#include "Core/ErrorCode.h"
#include "Core/logger.h"
#include "Data/ProtoPacket.h"
#include "GSoVITSAssist.h"
#include "ModuleManager.h"
#include <cmath>
#include <exception>
#include <filesystem>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <system_error>
#include <thread>


#include <boost/asio.hpp>

namespace NAssist {
const bool isTestEnv = false;
static std::string OpenLiveDomain = isTestEnv ? "http://test-live-open.biliapi.net" : "https://live-open.biliapi.com";

const std::string k_InteractivePlayStart = "/v2/app/start";
const std::string k_InteractivePlayEnd = "/v2/app/end";
const std::string k_InteractivePlayBatchHeartBeat = "/v2/app/batchHeartbeat";
const std::string k_InteractivePlayHeartBeat = "/v2/app/heartbeat";

BiliClientAssist::BiliWebCMD string2BiliWebCMD(std::string cmd) {
  if (cmd == "LIVE_OPEN_PLATFORM_DM") {
    return BiliClientAssist::BiliWebCMD::LIVE_OPEN_PLATFORM_DM;
  } else if (cmd == "LIVE_OPEN_PLATFORM_SEND_GIFT") {
    return BiliClientAssist::BiliWebCMD::LIVE_OPEN_PLATFORM_SEND_GIFT;
  } else if (cmd == "LIVE_OPEN_PLATFORM_SUPER_CHAT") {
    return BiliClientAssist::BiliWebCMD::LIVE_OPEN_PLATFORM_SUPER_CHAT;
  } else if (cmd == "LIVE_OPEN_PLATFORM_SUPER_CHAT_DEL") {
    return BiliClientAssist::BiliWebCMD::LIVE_OPEN_PLATFORM_SUPER_CHAT_DEL;
  } else if (cmd == "LIVE_OPEN_PLATFORM_GUARD") {
    return BiliClientAssist::BiliWebCMD::LIVE_OPEN_PLATFORM_GUARD;
  } else if (cmd == "LIVE_OPEN_PLATFORM_LIKE") {
    return BiliClientAssist::BiliWebCMD::LIVE_OPEN_PLATFORM_LIKE;
  } else {
    return BiliClientAssist::BiliWebCMD::None;
  }
}

void BiliClientAssist::init() {
  CORE_INFO_TAG("BiliClient", "BiliClientAssist init");
  // TODO: initiate the data from json file.
  m_AccessKey = "BP2Yl3cBDNc225qmrK53Fsyv";
  m_AccessKeySecret = "idiGra89obnAzmlZNjWRnw9jDi6vY6";
  m_AppId = "1739800103262";
  m_Code = "xxxxxxxxx";

  // SetConsoleOutputCP(CP_UTF8);

  if (std::filesystem::exists(BILI_CONFIG_PATH)) {
    std::ifstream istream(BILI_CONFIG_PATH);
    using Iterator = std::istreambuf_iterator<char>;
    std::string content(Iterator{istream}, Iterator{});
    istream.close();

    if (!content.empty()) {
      auto jv = boost::json::parse(content);
      if (!jv.is_null())
        if (!jv.at("uid").is_null()) {
          m_Code = jv.at("uid").as_string();
        }
    }
  }

  app_heart_periodic_task = std::make_shared<PeriodicTask>(
      [this]() {
        
        HeartBeatInteractivePlay(m_AppStartInfo.GameInfo.GameId);
      },
      20);

  game_heart_periodic_task = std::make_shared<PeriodicTask>(
      [this]() {
        
        HeartBeatWebsocketClient();
      },
      30);
}
void BiliClientAssist::shutdown() {

  auto ec = stop();

  if (std::filesystem::exists(BILI_CONFIG_PATH)) {
    std::ofstream ostream(BILI_CONFIG_PATH);

    if (!ostream) {
      CORE_ERROR_TAG("BiliClientAssist", "保存UID : {} 失败", BILI_CONFIG_PATH);
      return;
    }
    size_t index = 0;

    std::string json_str = std::format("{{\"{0}\":\"{1}\"}}", "uid", m_Code);

    CORE_INFO_TAG("BiliClientAssist", "保存UID :\n {} ", json_str);
    ostream << json_str;
    ostream.close();
  }
  CORE_INFO_TAG("BiliClient", "BiliClientAssist shutdown");
}
std::error_code BiliClientAssist::stop() {
  if (msgthread.joinable()) {
    m_stoped.set_value();

    msgthread.join();
  }

  app_heart_periodic_task->Stop();
  game_heart_periodic_task->Stop();

  if (appthread.joinable()) {
    appthread.join();
  }
  if (gamethread.joinable()) {
    gamethread.join();
  }
  CORE_INFO_TAG("BiliClient", "BiliClientAssist stop");
  return make_error_code(bili_client_errc::success);
}

std::error_code BiliClientAssist::start() {

  CORE_INFO_TAG("BiliClient", "BiliClientAssist start");
  StartInteractivePlay();
  auto res = AuthWebsocket();
  if (res) {
    return res;
  }
  appthread = std::thread([this]() { app_heart_periodic_task->Start(); });
  gamethread = std::thread([this]() { game_heart_periodic_task->Start(); });

  m_stoped = std::promise<void>();
  std::shared_future<void> future = m_stoped.get_future();
  msgthread = std::thread(
      [this](std::shared_future<void> future) {
        while (future.wait_for(std::chrono::milliseconds(100)) ==
               std::future_status::timeout) {
          WebsocketClientReceive();
        }
      },
      std::move(future));

  return make_error_code(audio_engine_errc::success);
}

void BiliClientAssist::StartInteractivePlay() {

  std::string param =
      (std::stringstream() << "{\"code\":" << "\"" << m_Code << "\""
                           << ",\"app_id\":" << m_AppId << "}")
          .str();
  auto json_value = RequestWebUTF8(OpenLiveDomain, k_InteractivePlayStart,
                                   HttpRequestMethod::post, param);

  m_AppStartInfo = boost::json::value_to<AppStartInfo>(json_value);
}

void BiliClientAssist::EndInteractivePlay() {
  std::string param =
      (std::stringstream() << "{\"game_id\":" << "\""
                           << m_AppStartInfo.GameInfo.GameId << "\""
                           << ",\"app_id\":" << m_AppId << "}")
          .str();
  auto json_value = RequestWebUTF8(OpenLiveDomain, k_InteractivePlayEnd,
                                   HttpRequestMethod::post, param);
}

void BiliClientAssist::HeartBeatInteractivePlay(std::string gameId) {
  std::string param =
      (std::stringstream() << "{\"game_id\":" << "\"" << gameId << "\"" << "}")
          .str();
  auto json_value = RequestWebUTF8(OpenLiveDomain, k_InteractivePlayHeartBeat,
                                   HttpRequestMethod::post, param);
  if(json_value.is_null())
  {
    CORE_ERROR_TAG("BiliClient", "app heartbeat error");
  }
}

boost::json::value BiliClientAssist::RequestWebUTF8(
    std::string host, std::string uri, HttpRequestMethod request_method,
    std::string param, std::string cookie /*= ""*/) {
  if (!cookie.empty()) {
    CORE_ERROR_TAG("BiliClient", "bilibili http request not support cookie");
  }
  // bilibili鉴权 header
  auto now_ms = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch());
  auto now_s = std::chrono::duration_cast<std::chrono::seconds>(now_ms);

  std::mt19937 urbg{static_cast<unsigned int>(now_s.count())};
  std::uniform_int_distribution<int> distr1{1, 100000};

  std::string timestamp = std::to_string(now_s.count());
  std::string nonce = std::to_string(distr1(urbg));
  std::string md5data = MD5Encryption::getMd5(param);

  std::map<std::string, std::string> head_map{
      {"x-bili-timestamp", timestamp},
      {"x-bili-signature-method", "HMAC-SHA256"},
      {"x-bili-signature-nonce", nonce},
      {"x-bili-accesskeyid", m_AccessKey},
      {"x-bili-signature-version", "1.0"},
      {"x-bili-content-md5", md5data}};
  auto head_list = SortHeadMap(head_map);

  std::string signstr = {};
  for (size_t index = 0; index < head_list.size(); index++) {
    if (index != head_list.size() - 1) {
      signstr += head_list[index] + "\n";
    } else {
      signstr += head_list[index];
    }
  }

  // 使用 hmac-sha265进行签名
  auto signature = HMACEncryption::SHA256(m_AccessKeySecret, signstr);
  head_map["Authorization"] = signature;
  head_map["Content-Type"] = "application/json";
  head_map["Accept"] = "application/json";

  // 创建请求，接受响应

  m_bili_request = HttpRequest::CreateRequest(host, uri, request_method);
  m_bili_request->SetContent(param);
  for (const auto &head : head_map) {
    m_bili_request->AddHeader(head.first, head.second);
  }
  auto data = m_bili_request->Receive();

  // parse the data to json data
  boost::json::value result;
  try {
    result = boost::json::parse(data);
  } catch (std::bad_alloc const &e) {
    CORE_ERROR_TAG("BiliClient", "http response pasrse fail :{}", e.what());
  }

  return result;
}

std::error_code BiliClientAssist::AuthWebsocket() {
  ProtoPacket packet;
  packet.header.packet_length = m_AppStartInfo.WebsocketInfo.AuthBody.size() +
                                packet.header.header_length;
  packet.header.operation = ProtoOperation::Authority;
  packet.header.version = ProtoVersion::HeartBeat;
  packet.header.sequence_id = 1;
  packet.body =
      std::vector<uint8_t>(m_AppStartInfo.WebsocketInfo.AuthBody.begin(),
                           m_AppStartInfo.WebsocketInfo.AuthBody.end());
  std::vector<uint8_t> bytes = ProtoPacket2bytes(packet);

  if (m_AppStartInfo.WebsocketInfo.WssLink.empty()) {
    return make_error_code(bili_client_errc::auth_websocket_fail);
  }
  m_bili_websocket = AsyncWebsocketClient::Create(
      m_AppStartInfo.WebsocketInfo.WssLink.front());

  m_bili_websocket->AsyncConnect([this, bytes]() {
    m_bili_websocket->AsyncSend(bytes, [this](boost::beast::error_code ec,
                                              std::size_t bytes_transferred) {
      if (ec) {
        CORE_ERROR_TAG("BiliClient", "bilibili auth send fail :", ec.what());
      }

      m_bili_websocket->AsyncReceive(
          [this](boost::beast::error_code ec, std::size_t bytes_transferred) {
            if (ec) {
              CORE_ERROR_TAG("BiliClient",
                             "bilibili auth receive fail :", ec.what());
            }
            std::stringstream ss;
            ss << boost::beast::make_printable(
                m_bili_websocket->GetBuffer().data());
            CORE_INFO_TAG("BiliClient", "bilibili auth result: {}", ss.str());
          });
    });
  });

  m_bili_websocket->CommitAsyncTask();
  return make_error_code(bili_client_errc::success);
}

void BiliClientAssist::HeartBeatWebsocketClient() {
  ProtoPacket packet;
  packet.header.packet_length = packet.header.header_length;
  packet.header.operation = ProtoOperation::HeartBeat;
  packet.header.version = ProtoVersion::HeartBeat;
  packet.header.sequence_id = 1;
  std::vector<uint8_t> bytes = ProtoPacket2bytes(packet);
  auto ec = m_bili_websocket->Send(bytes);
  if(ec)
  {
    CORE_ERROR_TAG("","WebsocketClient HeartBeat fail : {}",ec.message());
    return;
  }
  
}

void BiliClientAssist::WebsocketClientReceive() {
  /*
  m_bili_websocket->AsyncReceive([this](boost::beast::error_code ec, std::size_t
  bytes_transferred) { if (ec) std::cout << ec.what();

          std::cout <<
  boost::beast::make_printable(m_bili_websocket->GetBuffer().data()) <<
  std::endl; m_bili_websocket->CleanBuffer();

          WebsocketClientReceive();
          });
  m_bili_websocket->CommitAsyncTask();

*/
  auto res = m_bili_websocket->Receive();
  if(res.index()==1)
  {
    auto ec = std::get<1>(res);
    CORE_ERROR_TAG("","WebsocketClient Receive fail : {}",ec.message());
    return;
  }
  auto bytes = std::get<0>(res);
  if (bytes.empty())
    return;
  auto packet = bytes2ProtoPacket(bytes);
  
  

  if (ProtoOperation::ServerNotify == packet.header.operation) {
    std::error_code ec;
    std::string result = std::string(packet.body.begin(), packet.body.end());
    auto json_value = boost::json::parse(result,ec);
    if(ec){
      CORE_ERROR_TAG("BiliClientAssist"," parse websocket respon fail : {}",result);
      return;
    }
    std::string cmd = json_value.at("cmd").as_string().c_str();
    BiliWebCMD bili_cmd = string2BiliWebCMD(cmd);
    switch (bili_cmd) {
    case BiliWebCMD::LIVE_OPEN_PLATFORM_DM: {

      CORE_TRACE_TAG("BiliClient", "websocket receive LIVE_OPEN_PLATFORM_DM");
      auto p = ModuleManager::getInstance().getModule<GSoVITSAssist>();
      std::string uname = json_value.at("data").at("uname").as_string().c_str();
      std::string content = json_value.at("data").at("msg").as_string().c_str();
      std::string msg = std::format("{0}说：“{1}", uname, content);
      p->pushMsg(msg);
      break;
    }
    case BiliWebCMD::LIVE_OPEN_PLATFORM_SEND_GIFT: {

      CORE_TRACE_TAG("BiliClient",
                     "websocket receive LIVE_OPEN_PLATFORM_SEND_GIFT");
      break;
    }
    case BiliWebCMD::LIVE_OPEN_PLATFORM_SUPER_CHAT: {
      CORE_TRACE_TAG("BiliClient",
                     "websocket receive LIVE_OPEN_PLATFORM_SUPER_CHAT");
      break;
    }
    case BiliWebCMD::LIVE_OPEN_PLATFORM_SUPER_CHAT_DEL: {
      CORE_TRACE_TAG("BiliClient",
                     "websocket receive LIVE_OPEN_PLATFORM_SUPER_CHAT_DEL");
      break;
    }
    case BiliWebCMD::LIVE_OPEN_PLATFORM_GUARD: {

      CORE_TRACE_TAG("BiliClient",
                     "websocket receive LIVE_OPEN_PLATFORM_GUARD");
      break;
    }
    case BiliWebCMD::LIVE_OPEN_PLATFORM_LIKE: {
      CORE_TRACE_TAG("BiliClient", "websocket receive LIVE_OPEN_PLATFORM_LIKE");
      break;
    }
    case BiliWebCMD::None: {
      break;
    }
    }
  }
  m_bili_websocket->CleanBuffer();
}

std::vector<std::string>
BiliClientAssist::SortHeadMap(std::map<std::string, std::string> head_map) {
  std::vector<std::string> head_list;
  for (auto &head_pair : head_map) {
    head_list.emplace_back() = head_pair.first + ":" + head_pair.second;
  }
  std::sort(head_list.begin(), head_list.end(),
            [](std::string s1, std::string s2) { return s1 < s2; });
  return head_list;
}

}; // namespace NAssist
