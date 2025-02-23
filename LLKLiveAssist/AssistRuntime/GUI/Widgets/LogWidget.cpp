#include "LogWidget.h"

#include <ElaListView.h>

#include <QVBoxLayout>

#include "ElaLog.h"
#include "GUI/ModelView/LogModel.h"

namespace NAssist {
LogWidget::LogWidget(QWidget *parent) : QWidget{parent} {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 5, 5, 0);
  ElaListView *logView = new ElaListView(this);
  logView->setIsTransparent(true);
  m_logModel = new LogModel(this);
  logView->setModel(m_logModel);
  mainLayout->addWidget(logView);
  connect(ElaLog::getInstance(), &ElaLog::logMessage, this,
          [=](QString log) { m_logModel->appendLogList(log); });
  size_t index = 0;
}
void LogWidget::setLogger() {}
LogWidget::~LogWidget() {}
} // namespace NAssist
