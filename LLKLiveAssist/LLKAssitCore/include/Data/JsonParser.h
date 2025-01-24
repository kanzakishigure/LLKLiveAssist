#pragma once

#include <boost/json.hpp>
#include <boost/json/conversion.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <boost/winapi/error_codes.hpp>

#include <system_error>
#include <type_traits>
#include <variant>



namespace NAssist {

class Serializeable {
public:
  virtual  std::variant<boost::json::value, std::error_code> 
  serialize() { return std::error_code(); }
};

template <typename T> class Parser {
public:
  static std::variant<T, std::error_code> parse(const boost::json::value &jv) {
    if (std::is_base_of<Serializeable, T>()) {
      return boost::json::value_to<T>(jv);
    }
    return std::error_code();
  }
};

template <typename T> class Serializer {
public:
  static std::variant<const boost::json::value, std::error_code>
  serializer(const T &type) {
    if (std::is_base_of<Serializeable, T>()) {
      return boost::json::value_from(type);
    }
    return std::error_code();
  }
};


class AppStartInfo;
class GSoVITSModel;
class ProtoPacket;
class GSoVITSRequestBody;

} // namespace NAssist


namespace boost::json {
NAssist::AppStartInfo
tag_invoke(const boost::json::value_to_tag<NAssist::AppStartInfo> &,
           boost::json::value const &jv);
}
namespace boost::json {
NAssist::GSoVITSModel
tag_invoke(const boost::json::value_to_tag<NAssist::GSoVITSModel> &,
           boost::json::value const &jv);
}

namespace boost::json {
NAssist::ProtoPacket
tag_invoke(const boost::json::value_to_tag<NAssist::ProtoPacket> &,
           boost::json::value const &jv);
}

namespace boost::json {
	NAssist::GSoVITSRequestBody
		tag_invoke(const boost::json::value_to_tag<NAssist::GSoVITSRequestBody>&,
			boost::json::value const& jv);
    void
        tag_invoke(const value_from_tag&, value& jv, NAssist::GSoVITSRequestBody const& request);
}



