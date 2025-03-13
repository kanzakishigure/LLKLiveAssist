#pragma once

#include "Page/ModelPage.h"
#include "Page/SettingPage.h"

#include "Config/GUIConfig.h"
#include <ElaWindow.h>
#include <QMainWindow>
#include <memory>

class ElaContentDialog;

namespace NAssist {
class HomePage;
class SettingPage;
class AssistRuntimeWindow : public ElaWindow {

  Q_OBJECT

public:
  explicit AssistRuntimeWindow(QWidget *parent = nullptr);
  ~AssistRuntimeWindow() = default;
  void initWindow();
  void initEdgeLayout();
  void initContent();
  void saveSetting();

  std::shared_ptr<RuntimeConfig> getConfig() { return m_config; }

private:
  // ela widget
  ElaContentDialog *m_closeDialog{nullptr};
  HomePage *m_homePage{nullptr};
  ModelPage *m_modelPage{nullptr};
  SettingPage *m_settingPage{nullptr};

  QWidget *m_CentralWidget;
  QString m_settingKey;

  std::shared_ptr<RuntimeConfig> m_config;
};
} // namespace NAssist