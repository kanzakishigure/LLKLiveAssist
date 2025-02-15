#include "Data/JsonParser.h"

#include "Data/BiliData.h"
#include "Data/GSoVITSModel.h"
#include "Data/ProtoPacket.h"

#include <iostream>
#include <map>
#include <stdexcept>

namespace boost::json {
NAssist::AppStartInfo
tag_invoke(const boost::json::value_to_tag<NAssist::AppStartInfo> &,
           boost::json::value const &jv) {

  NAssist::AppStartInfo info;
  try {

    if (jv.at("data").is_null()) {
      throw std::runtime_error("invalid json_value to NAssist::AppStartInfo");
    }
    info.GameInfo.GameId =
        jv.at("data").at("game_info").at("game_id").as_string().c_str();

    info.WebsocketInfo.AuthBody =
        jv.at("data").at("websocket_info").at("auth_body").as_string().c_str();

    std::vector<std::string> wssaddr;
    for (const auto &value :
         jv.at("data").at("websocket_info").at("wss_link").as_array()) {
      wssaddr.emplace_back() = value.as_string().c_str();
    }
    info.WebsocketInfo.WssLink = wssaddr;

    info.AnchorInfo.RoomId =
        jv.at("data").at("anchor_info").at("room_id").as_int64();
    info.AnchorInfo.UName =
        jv.at("data").at("anchor_info").at("uname").as_string().c_str();
    info.AnchorInfo.UFace =
        jv.at("data").at("anchor_info").at("uface").as_string().c_str();
    info.AnchorInfo.Uid = jv.at("data").at("anchor_info").at("uid").as_int64();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return info;
}

NAssist::GSoVITSModel
tag_invoke(const boost::json::value_to_tag<NAssist::GSoVITSModel> &,
           boost::json::value const &jv) {

  NAssist::GSoVITSModel model;
  try {

    if (jv.is_null()) {
      throw std::runtime_error("invalid json_value to NAssist::AppStartInfo");
    }
    model.model_name = jv.at("model_name").as_string();
    model.gpt_weights = jv.at("gpt_weights").as_string();
    model.sovits_weights = jv.at("sovits_weights").as_string();
    model.prompt_lang = jv.at("prompt_lang").as_string();
    model.prompt_text = jv.at("prompt_text").as_string();
    model.ref_audio_path = jv.at("ref_audio_path").as_string();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return model;
}

void tag_invoke(const value_from_tag &, value &jv,
                NAssist::GSoVITSRequestBody const &request) {
  // Store the IP address as a 4-element array of octets

  jv = {
      {"text", request.text},
      {"text_lang", request.text_lang},
      {"ref_audio_path", request.ref_audio_path},
      {"prompt_text", request.prompt_text},
      {"prompt_lang", request.prompt_lang},
      {"top_k", request.top_k},
      {"top_p", request.top_p},
      {"temperature", request.temperature},
      {"text_split_method", request.text_split_method},
      {"batch_size", request.batch_size},
      {"batch_threshold", request.batch_threshold},
      {"split_bucket", request.split_bucket},
      {"return_fragment", request.return_fragment},
      {"speed_factor", request.speed_factor},
      {"streaming_mode", request.streaming_mode},
      {"seed", request.seed},
      {"parallel_infer", request.parallel_infer},
      {"repetition_penalty", request.repetition_penalty},
  };
}

} // namespace boost::json