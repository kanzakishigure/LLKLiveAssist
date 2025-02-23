#include "Data/JsonParser.h"

#include "Data/BiliData.h"
#include "Data/GSoVITSModel.h"
#include "Data/ProtoPacket.h"
#include "GSoVITSAssist.h"

#include <boost/json/parser.hpp>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>


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
      throw std::runtime_error("invalid json_value to NAssist::GSoVITSModel");
    }
    model.model_name = jv.at("model_name").as_string();
    model.gpt_weights = jv.at("gpt_weights").as_string();
    model.sovits_weights = jv.at("sovits_weights").as_string();
    model.prompt_lang = jv.at("prompt_lang").as_string();
    model.prompt_text = jv.at("prompt_text").as_string();
    model.ref_audio_path = jv.at("ref_audio_path").as_string();
    model.model_img = jv.at("model_img").as_string();
    model.model_description = jv.at("model_description").as_string();
    model.model_author = jv.at("model_author").as_string();
    model.model_category = jv.at("model_category").as_string();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return model;
}
void tag_invoke(const value_from_tag &, value &jv,
                NAssist::GSoVITSModel const &model) {

  jv = {

      {"model_name", model.model_name},
      {"sovits_weights", model.sovits_weights},
      {"gpt_weights", model.gpt_weights},

      {"ref_audio_path", model.ref_audio_path},
      {"prompt_text", model.prompt_text},
      {"prompt_lang", model.prompt_lang},

      {"model_img", model.model_img},
      {"model_description", model.model_description},
      {"model_author", model.model_author},
      {"model_category", model.model_category},
  };
}

std::vector<NAssist::GSoVITSModel>
tag_invoke(const boost::json::value_to_tag<std::vector<NAssist::GSoVITSModel>> &,
           boost::json::value const &jv) {

  std::vector<NAssist::GSoVITSModel> models;
  try {

    if (jv.is_null()||!jv.is_array()) {
      throw std::runtime_error("invalid json_value to std::vector<NAssist::GSoVITSModel>");
    }
    auto array = jv.as_array();

    for(const auto& model_json : array)
    {
      auto res = NAssist::Parser<NAssist::GSoVITSModel>().parse(model_json);
      if(res.index()==1)
      {
        throw std::runtime_error("parse json value fail");
      }
      models.emplace_back()=std::get<0>(res);
    }
    

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return models;
}
void tag_invoke(const value_from_tag &, value &jv,
                std::vector<NAssist::GSoVITSModel> const &models) {

  boost::json::array arr;
  for (const auto &model : models) {
    auto res = std::get<0>(
        NAssist::Serializer<NAssist::GSoVITSModel>().serializer(model));
    arr.push_back(res);
  }
  jv = arr;
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

namespace NAssist {

  std::string pretty_json_string( boost::json::value const &jv,
                    std::string *indent ) {
    std::string indent_;
    std::ostringstream os;
    if (!indent)
      indent = &indent_;
    switch (jv.kind()) {
    case boost::json::kind::object: {
      os << "{\n";
      indent->append(4, ' ');
      auto const &obj = jv.get_object();
      if (!obj.empty()) {
        auto it = obj.begin();
        for (;;) {
          os << *indent << boost::json::serialize(it->key()) << " : ";
          os<<pretty_json_string( it->value(), indent);
          if (++it == obj.end())
            break;
          os << ",\n";
        }
      }
      os << "\n";
      indent->resize(indent->size() - 4);
      os << *indent << "}";
      break;
    }
  
    case boost::json::kind::array: {
      os << "[\n";
      indent->append(4, ' ');
      auto const &arr = jv.get_array();
      if (!arr.empty()) {
        auto it = arr.begin();
        for (;;) {
          os << *indent;
          os << pretty_json_string(*it, indent);
          if (++it == arr.end())
            break;
          os << ",\n";
        }
      }
      os << "\n";
      indent->resize(indent->size() - 4);
      os << *indent << "]";
      break;
    }
  
    case boost::json::kind::string: {
      os << boost::json::serialize(jv.get_string());
      break;
    }
  
    case boost::json::kind::uint64:
      os << jv.get_uint64();
      break;
  
    case boost::json::kind::int64:
      os << jv.get_int64();
      break;
  
    case boost::json::kind::double_:
      os << jv.get_double();
      break;
  
    case boost::json::kind::bool_:
      if (jv.get_bool())
        os << "true";
      else
        os << "false";
      break;
  
    case boost::json::kind::null:
      os << "null";
      break;
    }
  
    if (indent->empty())
      os << "\n";
    return os.str();
  }
}