#pragma once

#include "Page/ModelPage.h"
#include "Page/SettingPage.h"

#include <ElaWindow.h>
#include <QMainWindow>


class ElaContentDialog;

namespace NAssist {
class HomePage;
class SettingPage;
class AssistRuntimeWindow : public ElaWindow {

  Q_OBJECT

public:
  explicit AssistRuntimeWindow(QWidget *parent = nullptr);
  void initWindow();
  void initEdgeLayout();
  void initContent();

private:
  // ela widget
  ElaContentDialog *m_closeDialog{nullptr};
  HomePage *m_homePage{nullptr};
  ModelPage *m_modelPage{nullptr};
  SettingPage *m_settingPage{nullptr};

  QWidget *m_CentralWidget;
  QString m_settingKey;
};
} // namespace NAssist