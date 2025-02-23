#include "Core/ErrorCode.h"

namespace NAssist {
std::error_code make_error_code(net_errc ec) {
  static const struct : std::error_category {
    virtual std::string message(int val) const override {
      switch (static_cast<net_errc>(val)) {

      case net_errc::success:
        return "success";
      case net_errc::ssl_link_fail:
        return "create boost ssl link fail";
      default:
        return "undefined error";
      }
    }
    virtual const char *name() const noexcept override { return "net_errc"; }
  } instance;

  return std::error_code(std::error_code(static_cast<int>(ec), instance));
}

std::error_code make_error_code(bili_client_errc ec) {
  static const struct : std::error_category {
    virtual std::string message(int val) const override {
      switch (static_cast<bili_client_errc>(val)) {

      case bili_client_errc::success:
        return "success";
      case bili_client_errc::auth_websocket_fail:
      return "auth_websocket_fail";
      default:
        return "undefined error";
      }
    }
    virtual const char *name() const noexcept override {
      return "bili_client_errc";
    }
  } instance;

  return std::error_code(std::error_code(static_cast<int>(ec), instance));
}

std::error_code make_error_code(audio_engine_errc ec) {
  static const struct : std::error_category {
    virtual std::string message(int val) const override {
      switch (static_cast<audio_engine_errc>(val)) {

      case audio_engine_errc::success:
        return "success";
      default:
        return "undefined error";
      }
    }
    virtual const char *name() const noexcept override {
      return "audio_engine_errc";
    }
  } instance;

  return std::error_code(std::error_code(static_cast<int>(ec), instance));
}
std::error_code make_error_code(gpt_sovits_errc ec) {
  static const struct : std::error_category {
    virtual std::string message(int val) const override {
      switch (static_cast<gpt_sovits_errc>(val)) {

      case gpt_sovits_errc::success :
        return "success";
      case gpt_sovits_errc::gsovist_not_found :
        return "gpt-sovits 路径错误或未安装GPT-Sovits";
      case NAssist::gpt_sovits_errc::gsovist_process_dump:
        return "结束 gpt-sovits 进程失败";
      case NAssist::gpt_sovits_errc::sovist_model_path_invalid:
        return "sovist模型 路径不存在";
      case NAssist::gpt_sovits_errc::gpt_model_path_invalid:
        return "gpt模型 路径不存在";
      case NAssist::gpt_sovits_errc::ref_audio_path_invalid:
        return "参考语音 文件路径不存在";
      default:
        return "undefined error";
      }
    }
    virtual const char *name() const noexcept override {
      return "gpt_sovits_errc";
    }
  } instance;

  return std::error_code(std::error_code(static_cast<int>(ec), instance));
}
} // namespace NAssist