#include "UrlImageLoader.h"
#include "Net/HttpRequest.h"
#include <QURL>
#include <string>
#include <vector>

namespace NAssist {

void UrlImageLoader::loadFromUrl(const QUrl &url) {

  if (m_request) {
    m_request.reset();
  }

  std::string host = "https://"+url.host().toStdString();
  std::string path = url.path().toStdString();
  m_request = HttpRequest::CreateRequest(host, path, HttpRequestMethod::get);
  auto res = m_request->Receive();

  QByteArray image_data(res.data(), res.size());
  m_Pixmap.loadFromData(image_data);
}

} // namespace NAssist