#pragma once

#include <system_error>
namespace NAssist {
enum class net_errc {
  success = 0,
  ssl_link_fail,

};
std::error_code make_error_code(net_errc ec);

enum class bili_client_errc {
  success = 0,
};
std::error_code make_error_code(bili_client_errc ec);
enum class audio_engine_errc {
  success = 0,
};
std::error_code make_error_code(audio_engine_errc ec);

enum class gpt_sovits_errc {
  success = 0,
};
std::error_code make_error_code(gpt_sovits_errc ec);

} // namespace NAssist