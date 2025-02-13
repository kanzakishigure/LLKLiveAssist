#pragma once
#include "PluginBase.h"
#include <string>
#include <vector>

#include "Net/WebsocketClient.h"
#include "Net/HttpRequest.h"
#include "Utils/EncryptionUtils.h"
#include "Core/PeriodicTask.h"
#include "Data/BiliData.h"

#include <boost/json.hpp>


namespace NAssist {


class BiliClientAssist : public PluginBase {
public:
 enum class BiliWebCMD{
  LIVE_OPEN_PLATFORM_DM, //当该直播间有人发送弹幕时触发
  LIVE_OPEN_PLATFORM_SEND_GIFT,//当该直播间有人赠送礼物时触发
  LIVE_OPEN_PLATFORM_SUPER_CHAT,//当该直播间有人发送付费留言时触发
  LIVE_OPEN_PLATFORM_SUPER_CHAT_DEL,//当该直播间有付费留言被下线时触发
  LIVE_OPEN_PLATFORM_GUARD,//当该直播间有人上舰时触发
  LIVE_OPEN_PLATFORM_LIKE,//当该直播间有人点赞时触发
  None,
 }; 
  
  
  
  virtual void init() override;
  virtual void start() override;
  virtual void shutdown() override;
  virtual void drawUI() override;

   virtual PluginType getType() override { return PluginType::BiliClient; }
  static PluginType getStaticType() { return PluginType::BiliClient; }
  
  std::string  getUserCode() { return m_Code;}
  void setUserCode(const std::string& user_code) { m_Code = user_code; }
private:
  
  
  void StartInteractivePlay();
  void EndInteractivePlay();
  void HeartBeatInteractivePlay(std::string gameId);
  void HeartBeatWebsocketClient();
  void WebsocketClientReceive();

  std::vector<std::string> SortHeadMap(std::map<std::string, std::string> headmap);

  boost::json::value RequestWebUTF8(std::string host, std::string uri, HttpRequestMethod request_method, std::string param, std::string cookie = "");
  void  AuthWebsocket();

  
  

  std::string m_AccessKey;        //直播创作者服务中心【个人资料】页面获取
  std::string m_AccessKeySecret;  //在直播创作者服务中心【个人资料】页面获取
  std::string m_AppId;            //在直播创作者服务中心【我的项目】页面创建应用后获取
  std::string m_Code;             //主播身份码
  

  AppStartInfo m_AppStartInfo;

  std::shared_ptr<AsyncWebsocketClient> m_bili_websocket;
  std::shared_ptr<HttpRequest> m_bili_request;

  std::thread appthread;
  std::thread gamethread;
  std::thread msgthread;
 
};
} // namespace NAssist
