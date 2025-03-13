#include "GUI/Page/SettingPage.h"
#include <qwidget.h>

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "Config/GUIConfig.h"
#include "ElaApplication.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaLog.h"
#include "ElaRadioButton.h"
#include "ElaScrollPageArea.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaToggleSwitch.h"
#include "ElaWindow.h"
#include "GSoVITSAssist.h"
#include "GUI/AssistRuntimeWindow.h"
#include "ModuleManager.h"
namespace NAssist {
SettingPage::SettingPage(QWidget *parent) : BasePage(parent) {
  ElaWindow *window = dynamic_cast<ElaWindow *>(parent);
  setWindowTitle("Setting");

  auto runtime_config = static_cast<AssistRuntimeWindow *>(parent)->getConfig();

  ElaText *themeText = new ElaText("主题设置", this);
  themeText->setWordWrap(false);
  themeText->setTextPixelSize(18);

  _themeComboBox = new ElaComboBox(this);
  _themeComboBox->addItem("日间模式");
  _themeComboBox->addItem("夜间模式");
  _themeComboBox->setCurrentIndex(static_cast<int>(runtime_config->theme_mode));

  ElaScrollPageArea *themeSwitchArea = new ElaScrollPageArea(this);
  QHBoxLayout *themeSwitchLayout = new QHBoxLayout(themeSwitchArea);
  ElaText *themeSwitchText = new ElaText("主题切换", this);
  themeSwitchText->setWordWrap(false);
  themeSwitchText->setTextPixelSize(15);
  themeSwitchLayout->addWidget(themeSwitchText);
  themeSwitchLayout->addStretch();
  themeSwitchLayout->addWidget(_themeComboBox);
  connect(_themeComboBox, QOverload<int>::of(&ElaComboBox::currentIndexChanged),
          this, [=](int index) {
            if (index == 0) {
              eTheme->setThemeMode(ElaThemeType::Light);
              runtime_config->theme_mode = NAssist::ThemeMode::Light;
            } else {
              eTheme->setThemeMode(ElaThemeType::Dark);
              runtime_config->theme_mode = NAssist::ThemeMode::Dark;
            }
          });
  connect(eTheme, &ElaTheme::themeModeChanged, this,
          [=](ElaThemeType::ThemeMode themeMode) {
            _themeComboBox->blockSignals(true);
            if (themeMode == ElaThemeType::Light) {
              _themeComboBox->setCurrentIndex(0);
              runtime_config->theme_mode = NAssist::ThemeMode::Light;
            } else {
              _themeComboBox->setCurrentIndex(1);
              runtime_config->theme_mode = NAssist::ThemeMode::Dark;
            }

            _themeComboBox->blockSignals(false);
          });

  ElaText *helperText = new ElaText("应用程序设置", this);
  helperText->setWordWrap(false);
  helperText->setTextPixelSize(18);

  _micaSwitchButton = new ElaToggleSwitch(this);
  ElaScrollPageArea *micaSwitchArea = new ElaScrollPageArea(this);
  QHBoxLayout *micaSwitchLayout = new QHBoxLayout(micaSwitchArea);
  ElaText *micaSwitchText = new ElaText("启用云母效果(跨平台)", this);
  micaSwitchText->setWordWrap(false);
  micaSwitchText->setTextPixelSize(15);
  micaSwitchLayout->addWidget(micaSwitchText);
  micaSwitchLayout->addStretch();
  micaSwitchLayout->addWidget(_micaSwitchButton);
  _micaSwitchButton->setIsToggled(runtime_config->enable_mica);
  connect(_micaSwitchButton, &ElaToggleSwitch::toggled, this,
          [=](bool checked) {
            eApp->setIsEnableMica(checked);
            runtime_config->enable_mica = checked;
          });

  _logSwitchButton = new ElaToggleSwitch(this);
  ElaScrollPageArea *logSwitchArea = new ElaScrollPageArea(this);
  QHBoxLayout *logSwitchLayout = new QHBoxLayout(logSwitchArea);
  ElaText *logSwitchText = new ElaText("启用日志功能", this);
  _logSwitchButton->setIsToggled(runtime_config->enabel_logger);
  logSwitchText->setWordWrap(false);
  logSwitchText->setTextPixelSize(15);
  logSwitchLayout->addWidget(logSwitchText);
  logSwitchLayout->addStretch();
  logSwitchLayout->addWidget(_logSwitchButton);
  connect(_logSwitchButton, &ElaToggleSwitch::toggled, this, [=](bool checked) {
    ElaLog::getInstance()->initMessageLog(checked);
    if (checked) {
      qDebug() << "日志已启用!";
    } else {
      qDebug() << "日志已关闭!";
    }
    runtime_config->enabel_logger = checked;
  });

  ElaLineEdit *propety_value_line_edit = new ElaLineEdit(this);
  propety_value_line_edit->setText(runtime_config->remote_server_url.c_str());

  ElaScrollPageArea *remote_server_area = new ElaScrollPageArea(this);

  QHBoxLayout *textbox_area_layout = new QHBoxLayout(remote_server_area);
  ElaText *propety_name_text = new ElaText("remote_server 地址", this);
  propety_name_text->setTextPixelSize(15);
  textbox_area_layout->addWidget(propety_name_text);
  textbox_area_layout->addStretch();
  textbox_area_layout->addWidget(propety_value_line_edit);
  connect(propety_value_line_edit, &ElaLineEdit::focusOut, this,
          [=](QString text) {
            ModuleManager::getInstance()
                .getModule<GSoVITSAssist>()
                ->setGptSovitsServer(text.toStdString());
            runtime_config->remote_server_url = text.toStdString();
          });

  _nativeServerSwitchButton = new ElaToggleSwitch(this);
  _nativeServerSwitchButton->setIsToggled(
      !runtime_config->enable_remote_server);
  ElaScrollPageArea *native_server_switch_area = new ElaScrollPageArea(this);
  QHBoxLayout *native_server_switch_layout =
      new QHBoxLayout(native_server_switch_area);
  ElaText *native_server_switchtext = new ElaText("启用本地服务器", this);
  native_server_switchtext->setWordWrap(false);
  native_server_switchtext->setTextPixelSize(15);
  native_server_switch_layout->addWidget(native_server_switchtext);
  native_server_switch_layout->addStretch();
  native_server_switch_layout->addWidget(_nativeServerSwitchButton);
  connect(_nativeServerSwitchButton, &ElaToggleSwitch::toggled, this,
          [=](bool checked) {
            ModuleManager::getInstance()
                .getModule<GSoVITSAssist>()
                ->enableNativeServer(checked);
            remote_server_area->setVisible(!checked);
            runtime_config->enable_remote_server = !checked;
          });

          remote_server_area->setVisible(false);
  if (runtime_config->enable_remote_server) {
    ModuleManager::getInstance().getModule<GSoVITSAssist>()->setGptSovitsServer(
        runtime_config->remote_server_url);
        ModuleManager::getInstance().getModule<GSoVITSAssist>()->enableNativeServer(
          !runtime_config->enable_remote_server);
        remote_server_area->setVisible(runtime_config->enable_remote_server);
  }
  _minimumButton = new ElaRadioButton("Minimum", this);
  _compactButton = new ElaRadioButton("Compact", this);
  _maximumButton = new ElaRadioButton("Maximum", this);
  _autoButton = new ElaRadioButton("Auto", this);
  switch (runtime_config->navigation_displaymode) {
  case NAssist::NavigationDisplayMode::Auto:
    _autoButton->setChecked(true);
    break;
  case NAssist::NavigationDisplayMode::Minimal:
    _minimumButton->setChecked(true);
    break;
  case NAssist::NavigationDisplayMode::Maximal:
    _maximumButton->setChecked(true);
    break;
  case NAssist::NavigationDisplayMode::Compact:
    _compactButton->setChecked(true);
    break;
  }

  ElaScrollPageArea *displayModeArea = new ElaScrollPageArea(this);
  QHBoxLayout *displayModeLayout = new QHBoxLayout(displayModeArea);
  ElaText *displayModeText = new ElaText("导航栏模式选择", this);
  displayModeText->setWordWrap(false);
  displayModeText->setTextPixelSize(15);
  displayModeLayout->addWidget(displayModeText);
  displayModeLayout->addStretch();
  displayModeLayout->addWidget(_minimumButton);
  displayModeLayout->addWidget(_compactButton);
  displayModeLayout->addWidget(_maximumButton);
  displayModeLayout->addWidget(_autoButton);

  connect(_minimumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
    if (checked) {
      window->setNavigationBarDisplayMode(ElaNavigationType::Minimal);
      runtime_config->navigation_displaymode =
          NAssist::NavigationDisplayMode::Minimal;
    }
  });
  connect(_compactButton, &ElaRadioButton::toggled, this, [=](bool checked) {
    if (checked) {
      window->setNavigationBarDisplayMode(ElaNavigationType::Compact);
      runtime_config->navigation_displaymode =
          NAssist::NavigationDisplayMode::Compact;
    }
  });
  connect(_maximumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
    if (checked) {
      window->setNavigationBarDisplayMode(ElaNavigationType::Maximal);
      runtime_config->navigation_displaymode =
          NAssist::NavigationDisplayMode::Maximal;
    }
  });
  connect(_autoButton, &ElaRadioButton::toggled, this, [=](bool checked) {
    if (checked) {
      window->setNavigationBarDisplayMode(ElaNavigationType::Auto);
      runtime_config->navigation_displaymode =
          NAssist::NavigationDisplayMode::Auto;
    }
  });

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle("Setting");
  QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
  centerLayout->addSpacing(30);
  centerLayout->addWidget(themeText);
  centerLayout->addSpacing(10);
  centerLayout->addWidget(themeSwitchArea);
  centerLayout->addSpacing(15);
  centerLayout->addWidget(helperText);
  centerLayout->addSpacing(10);
  centerLayout->addWidget(logSwitchArea);
  centerLayout->addWidget(micaSwitchArea);
  centerLayout->addWidget(displayModeArea);
  centerLayout->addWidget(native_server_switch_area);
  centerLayout->addWidget(remote_server_area);
  centerLayout->addStretch();
  centerLayout->setContentsMargins(0, 0, 0, 0);
  addCentralWidget(centralWidget, true, true, 0);
}

SettingPage::~SettingPage() {}
} // namespace NAssist