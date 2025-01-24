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
  virtual void init() override;
  virtual void start() override;
  virtual void shutdown() override;
  virtual void drawUI() override;

private:
  virtual PluginType getStaticType() override { return m_type; }

  void StartInteractivePlay();
  void EndInteractivePlay();
  void HeartBeatInteractivePlay(std::string gameId);
  void HeartBeatWebsocketClient();
  void WebsocketClientReceive();

  std::vector<std::string> SortHeadMap(std::map<std::string, std::string> headmap);

  boost::json::value RequestWebUTF8(std::string host, std::string uri, HttpRequestMethod request_method, std::string param, std::string cookie = "");
  void  AuthWebsocket();


  PluginType m_type = PluginType::BiliClient;

  std::string m_AccessKey;        //直播创作者服务中心【个人资料】页面获取
  std::string m_AccessKeySecret;  //在直播创作者服务中心【个人资料】页面获取
  std::string m_AppId;            //在直播创作者服务中心【我的项目】页面创建应用后获取
  std::string m_Code;             //主播身份码
  

  AppStartInfo m_AppStartInfo;

  std::shared_ptr<AsyncWebsocketClient> m_bili_websocket;
  std::shared_ptr<HttpRequest> m_bili_request;

  std::shared_ptr<PeriodicTask> m_AppHeartPeriodicTask;
  std::shared_ptr<PeriodicTask> m_GameHeartPeriodicTask;

  std::thread appthread;
  std::thread gamethread;
  std::thread msgthread;
 
};
} // namespace NAssist
