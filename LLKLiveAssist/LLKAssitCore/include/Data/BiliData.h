#pragma once
#include "Data/JsonParser.h"

#include <boost/json.hpp>
#include <string>
#include <vector>

namespace NAssist {

struct AppStartGameInfo {
  /// <summary>
  /// 场次id,心跳key(心跳保持20s-60s)调用一次,超过60s无心跳自动关闭,长连停止推送消息
  /// </summary>
  std::string GameId;
};

struct AppStartWebsocketInfo {
  /// <summary>
  /// 长连使用的请求json体 第三方无需关注内容,建立长连时使用即可
  /// </summary>
  std::string AuthBody;
  /// <summary>
  ///  wss 长连地址
  /// </summary>
  std::vector<std::string> WssLink;
};

struct AppStartAnchorInfo {
  /// <summary>
  /// 主播房间号
  /// </summary>
  uint64_t RoomId;
  /// <summary>
  /// 主播昵称
  /// </summary>
  std::string UName;
  /// <summary>
  /// 主播头像
  /// </summary>
  std::string UFace;
  /// <summary>
  /// 主播Uid
  /// </summary>
  uint64_t Uid;
};

struct AppStartInfo : public Serializeable {

  AppStartGameInfo GameInfo;

  AppStartWebsocketInfo WebsocketInfo;

  AppStartAnchorInfo AnchorInfo;
};
} // namespace NAssist
