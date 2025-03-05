#pragma once
#include <QObject>
#include <cstddef>
#include <qobject.h>

Q_DECLARE_METATYPE(std::error_code)
namespace NAssist {

class SignalEmitter : public QObject {
  Q_OBJECT
public:
  explicit SignalEmitter(QObject* parent = nullptr);
  ~SignalEmitter() = default;
  Q_INVOKABLE void triggerLambda(const std::error_code ec) { emit getRequest(ec); }

signals:
  void getRequest(std::error_code ec);
};
} // namespace NAssist