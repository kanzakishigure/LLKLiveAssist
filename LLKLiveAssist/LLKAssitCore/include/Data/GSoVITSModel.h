#pragma once
#include <boost/json.hpp>
#include <boost/json/value.hpp>
#include <sstream>
#include <string>

#include "Data/JsonParser.h"

namespace NAssist {
struct GSoVITSModel : public Serializeable {

  std::string model_name;
  std::string model_author;
  std::string model_img;
  std::string model_category;

  std::string sovits_weights; // Sovits模型 .pth
  std::string gpt_weights;    // GPT模型 .ckpt

  std::string prompt_text;
  std::string prompt_lang;
  std::string ref_audio_path;
  std::string model_description;

  GSoVITSModel() =default;
  GSoVITSModel(const GSoVITSModel &other) {
    model_name = other.model_name;
    model_author = other.model_author;
    model_img = other.model_img;
    model_category = other.model_category;

    sovits_weights = other.sovits_weights;
    gpt_weights = other.gpt_weights;

    prompt_text = other.prompt_text;
    prompt_lang = other.prompt_lang;
    ref_audio_path = other.ref_audio_path;
    model_description = other.model_description;
  }
  bool operator==(const GSoVITSModel &other) const {
    auto result =
        other.model_name == model_name && other.model_author == model_author &&
        other.model_img == model_img &&
        other.model_category == model_category &&

        other.sovits_weights == sovits_weights &&
        other.gpt_weights == gpt_weights &&

        other.model_description == model_description &&
        other.prompt_lang == prompt_lang && other.prompt_text == prompt_text &&
        other.ref_audio_path == ref_audio_path;
    return result;
  }
};

struct GSoVITSRequestBody : public Serializeable {
  std::string text;
  std::string text_lang;
  std::string ref_audio_path;
  std::string prompt_text;
  std::string prompt_lang;
  int top_k = 5;
  float top_p = 1;
  float temperature = 1;
  std::string text_split_method = "cut0";
  int batch_size = 1;
  float batch_threshold = 0.75;
  bool split_bucket = true;
  bool return_fragment = true;
  float speed_factor = 1.0f;
  bool streaming_mode = false;
  int seed = -1;
  bool parallel_infer = true;
  float repetition_penalty = 1.35;
};
inline std::string GSoVITSRequestBody2JsonString(GSoVITSRequestBody body) {
  std::stringstream ss;
  ss << "{" << R"("text":")" << body.text << "\"";
  ss << "," << R"("text_lang":")" << body.text_lang << "\"";
  ss << "," << R"("ref_audio_path":")" << body.ref_audio_path << "\"";
  ss << "," << R"("prompt_text":")" << body.prompt_text << "\"";
  ss << "," << R"("prompt_lang":")" << body.prompt_lang << "\"";
  ss << "," << "\"top_k\":" << body.top_k;
  ss << "," << "\"top_p\":" << body.top_p;
  ss << "," << "\"temperature\":" << body.temperature;
  ss << "," << R"("text_split_method":")" << body.text_split_method << "\"";
  ss << "," << "\"batch_size\":" << body.batch_size;
  ss << "," << "\"batch_threshold\":" << body.batch_threshold;
  ss << "," << "\"split_bucket\":" << body.split_bucket;
  ss << "," << "\"return_fragment\":" << body.return_fragment;
  ss << "," << "\"speed_factor\":" << body.speed_factor;
  ss << "," << "\"streaming_mode\":" << body.streaming_mode;
  ss << "," << "\"seed\":" << body.seed;
  ss << "," << "\"parallel_infer\":" << body.parallel_infer;
  ss << "," << "\"repetition_penalty\":" << body.repetition_penalty << "}";

  return ss.str();
}
} // namespace NAssist