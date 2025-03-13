#include "AssistRuntimeWindow.h"
#include "Config/GUIConfig.h"
#include "Core/logger.h"
#include "Def.h"
#include "ElaPlainTextEdit.h"
#include "ElaWindow.h"

#include <QDebug>
#include <QFontDatabase>
#include <QGraphicsView>
#include <QHBoxLayout>

#include <QApplication>
#include <QCursor>
#include <QFontDatabase>
#include <QWidget>
#include <memory>
#include <qglobal.h>
#include <qnamespace.h>
#include <qtextedit.h>

#include "ElaApplication.h"
#include "ElaContentDialog.h"
#include "ElaDockWidget.h"
#include "ElaEventBus.h"
#include "ElaLog.h"
#include "ElaMenu.h"
#include "ElaMenuBar.h"
#include "ElaProgressBar.h"
#include "ElaStatusBar.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaToolBar.h"
#include "ElaToolButton.h"
#include "Page/HomePage.h"
#include "Page/ModelPage.h"
#include "Page/SettingPage.h"
#include "Widgets/LogWidget.h"

#include "Page/SettingPage.h"
#include "spdlog/sinks/qt_sinks.h"
#include "spdlog/spdlog.h"

namespace NAssist {

AssistRuntimeWindow::AssistRuntimeWindow(QWidget *parent) : ElaWindow(parent) {
  setWindowTitle("LLKAssit");

  if (std::filesystem::exists(RUNTIME_SETTING_PATH)) {
    std::ifstream istream(RUNTIME_SETTING_PATH);
    using Iterator = std::istreambuf_iterator<char>;
    std::string content(Iterator{istream}, Iterator{});
    istream.close();
    if (!content.empty()) {
      auto jv = boost::json::parse(content);
      auto result = Parser<RuntimeConfig>::parse(jv);
      if (result.index() == 1) {

        RUNTIME_ERROR_TAG("AssistRuntimeWindow",
                          "AssistRuntimeWindow load RuntimeConfig fail!");
      }
      auto config = std::get<0>(result);
      m_config = std::make_shared<RuntimeConfig>(config);
    }
  }

  initWindow();

  // 额外布局
  initEdgeLayout();

  // 中心窗口
  initContent();

  // 拦截默认关闭事件
  m_closeDialog = new ElaContentDialog(this);
  connect(m_closeDialog, &ElaContentDialog::rightButtonClicked, this,
          &AssistRuntimeWindow::closeWindow);
  connect(m_closeDialog, &ElaContentDialog::middleButtonClicked, this, [=]() {
    m_closeDialog->close();
    showMinimized();
  });
  this->setIsDefaultClosed(false);
  connect(this, &AssistRuntimeWindow::closeButtonClicked, this,
          [=]() { m_closeDialog->exec(); });

  // 移动到中心
  moveToCenter();
}

void AssistRuntimeWindow::saveSetting() {


  if (std::filesystem::exists(RUNTIME_SETTING_PATH)) {
    std::ofstream ostream(RUNTIME_SETTING_PATH);

    if (!ostream) {
      RUNTIME_ERROR_TAG("AssistRuntimeWindow", "保存设置到 : {} 失败",
                        RUNTIME_SETTING_PATH);
      return;
    }
    auto config = *m_config;
    boost::json::value result = boost::json::value_from(config);
    std::string json_str = boost::json::serialize(result);
    ostream << json_str;
    ostream.close();
    CORE_INFO_TAG("AssistRuntimeWindow", "保存设置 :\n {} ", json_str);
  }
}

void AssistRuntimeWindow::initWindow() {

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // 注册自定义类型
  ///////////////////////////////////////////////////////////////////////////////////////////////
  qRegisterMetaType<std::vector<GSoVITSModel>>("std::vector<GSoVITSModel>");
  ///////////////////////////////////////////////////////////////////////////////////////////////
  QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
  QFontDatabase::addApplicationFont(":/Resource/Font/ElaAwesome.ttf");
  QFontDatabase::addApplicationFont(":/Resource/Font/Roboto.ttf");
  QFontDatabase::addApplicationFont(":/Resource/Font/PingFang-SC-Regular.ttf");
  // 默认字体
  QFont font = qApp->font();
  font.setPixelSize(13);
  font.setFamily("Roboto");

  font.setHintingPreference(QFont::PreferNoHinting);
  qApp->setFont(font);

  // setIsEnableMica(true);
  // setIsCentralStackedWidgetTransparent(true);
  setWindowIcon(QIcon(":/Resource/Image/LLK.jpg"));
  resize(1200, 740);
  // ElaLog::getInstance()->initMessageLog(true);

  // setIsNavigationBarEnable(false);
  // setNavigationBarDisplayMode(ElaNavigationType::Compact);
  // setWindowButtonFlag(ElaAppBarType::MinimizeButtonHint, false);
  setUserInfoCardPixmap(QPixmap(":/Resource/Image/LLK.png"));
  setUserInfoCardTitle("LLK Assist");
  setUserInfoCardSubTitle("基于Soviet的弹幕姬");
  setWindowTitle("LLK Assist");
  // setIsStayTop(true);
  // setUserInfoCardVisible(false);

  switch (m_config->theme_mode) {
  case NAssist::ThemeMode::Dark:
    eTheme->setThemeMode(ElaThemeType::Dark);
    break;
  case NAssist::ThemeMode::Light:
    eTheme->setThemeMode(ElaThemeType::Light);
    break;
  }
  eApp->setIsEnableMica(m_config->enable_mica);

  switch (m_config->navigation_displaymode) {
  case NAssist::NavigationDisplayMode::Auto:
    setNavigationBarDisplayMode(ElaNavigationType::NavigationDisplayMode::Auto);
    break;
  case NAssist::NavigationDisplayMode::Minimal:
    setNavigationBarDisplayMode(
        ElaNavigationType::NavigationDisplayMode::Minimal);
    break;
  case NAssist::NavigationDisplayMode::Maximal:
    setNavigationBarDisplayMode(
        ElaNavigationType::NavigationDisplayMode::Maximal);
    break;
  case NAssist::NavigationDisplayMode::Compact:
    setNavigationBarDisplayMode(
        ElaNavigationType::NavigationDisplayMode::Compact);
    break;
  }
  ElaLog::getInstance()->initMessageLog(m_config->enabel_logger);
}

void AssistRuntimeWindow::initEdgeLayout() {
  // 工具栏
  ElaToolBar *toolBar = new ElaToolBar("工具栏", this);
  toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  toolBar->setToolBarSpacing(3);
  toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toolBar->setIconSize(QSize(25, 25));

  ElaToolButton *logger_button = new ElaToolButton(this);
  logger_button->setElaIcon(ElaIconType::Terminal);

  toolBar->addWidget(logger_button);
  this->addToolBar(Qt::TopToolBarArea, toolBar);

  ElaDockWidget *log_dock_widget = new ElaDockWidget("日志信息", this);
  QPlainTextEdit *log_editor = new ElaPlainTextEdit(this);
  log_editor->setReadOnly(true);
  log_dock_widget->setWidget(log_editor);
  this->addDockWidget(Qt::BottomDockWidgetArea, log_dock_widget);
  resizeDocks({log_dock_widget}, {300}, Qt::Horizontal);
  LLKLogger::instance()->addLogger(
      spdlog::qt_logger_mt("qt_logger", log_editor), LLKLogger::Type::GUI);
  GUI_INFO("LLKLogger init success");

  connect(logger_button, &ElaToolButton::triggered, [log_dock_widget]() {
    if (log_dock_widget->isHidden()) {
      log_dock_widget->show();
    }
  });
}

void AssistRuntimeWindow::initContent() {

  m_homePage = new HomePage(this);
  addPageNode("HOME", m_homePage, ElaIconType::House);
  m_modelPage = new ModelPage(this);
  addPageNode("Model", m_modelPage, ElaIconType::DiceD12);
  m_settingPage = new SettingPage(this);
  addFooterNode("Setting", m_settingPage, m_settingKey, 0,
                ElaIconType::GearComplex);

  connect(this, &AssistRuntimeWindow::userInfoCardClicked, this, [=]() {
    this->navigation(m_homePage->property("ElaPageKey").toString());
  });
  connect(m_homePage, &HomePage::llkModelNavigation, this, [=]() {
    this->navigation(m_modelPage->property("ElaPageKey").toString());
  });
  connect(m_modelPage, &ModelPage::gSovitsmodelChanged, m_homePage,
          &HomePage::onGSoVITSModelChanged);
  qDebug() << ElaEventBus::getInstance()->getRegisteredEventsName();
}

} // namespace NAssist