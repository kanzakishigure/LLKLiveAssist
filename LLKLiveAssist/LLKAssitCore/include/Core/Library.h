#pragma once
#include "Core/FileSystem.h"
#include "Data/JsonParser.h"
#include <boost/json.hpp>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <vector>

namespace NAssist {

template <typename T>
// 处理库文件的本地存储
class Library {

public:
  struct Config {
    std::filesystem::path serilize_path;
  };
  explicit Library(Config config) : m_Config(config) { serialize(); }
  ~Library() { Deserialize(); }

private:
  void serialize() {
    if (std::filesystem::exists(m_Config.serilize_path)) {
      std::ifstream istream(m_Config.serilize_path);
      using Iterator = std::istreambuf_iterator<char>;
      std::string content(Iterator{istream}, Iterator{});
      istream.close();

      if (!content.empty()) {
        auto jv = boost::json::parse(content);
        if (jv.is_array()) {
          for (const auto &value : jv.as_array()) {
            auto result = Parser<T>::parse(value);
            if (result.index() == 1) {

              CORE_ERROR_TAG("Library", "Library serialize fail!");
            }
            m_Library.emplace_back() = get<0>(result);
          }

        } else {
          auto result = Parser<T>::parse(jv);
          if (result.index() == 1) {

            CORE_ERROR_TAG("Library", "Library serialize fail!");
          }
          m_Library.emplace_back() = get<0>(result);
        }
      }
    }
  }
  void Deserialize() {
    if (std::filesystem::exists(m_Config.serilize_path)) {
      std::ofstream ostream(m_Config.serilize_path);

      if (!ostream) {
        CORE_ERROR_TAG("Library", "Deserialize file to : {} 失败",
                       m_Config.serilize_path);
        return;
      }
      boost::json::value result = boost::json::value_from(m_Library);
      std::string json_str = boost::json::serialize(result);
      CORE_INFO_TAG("Library", "Deserialize file :\n {} ", json_str);
      ostream << json_str;
      ostream.close();
    }
  }
  void Delete(T value) { m_Library.erase(value); }
  void Add(T value) { m_Library.push_back(value); }


  std::vector<T> m_Library;
  Config m_Config;
};

} // namespace NAssist