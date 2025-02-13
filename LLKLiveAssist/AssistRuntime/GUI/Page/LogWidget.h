#pragma once

#include <QWidget>

namespace NAssist {

class LogModel;
class LogWidget : public QWidget {
  Q_OBJECT
public:
  explicit LogWidget(QWidget *parent = nullptr);
  ~LogWidget();

  static const LogWidget &instance() {
    static LogWidget instance;
    return instance;
  }
  void setLogger();
signals:
private:
  LogModel *m_logModel{nullptr};
};
}; // namespace NAssist
