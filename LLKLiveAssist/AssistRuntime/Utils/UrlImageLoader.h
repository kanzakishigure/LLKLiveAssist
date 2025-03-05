#pragma once
#include <Net/HttpRequest.h>
#include <QPixmap>
#include <memory>
#include <qpixmap.h>

namespace NAssist {

class UrlImageLoader {

public:
  explicit UrlImageLoader() =default;
  ~UrlImageLoader() = default;
  // 发起图片下载请求
  void loadFromUrl(const QUrl &url);
  QPixmap getQPixmap() { return m_Pixmap; }

private:
  std::shared_ptr<HttpRequest> m_request;
  QPixmap m_Pixmap;
};
} // namespace NAssist