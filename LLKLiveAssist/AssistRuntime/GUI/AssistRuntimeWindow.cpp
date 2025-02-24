#include "AssistRuntimeWindow.h"
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
#include <qglobal.h>
#include <qnamespace.h>
#include <qtextedit.h>

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
  eTheme->setThemeMode(ElaThemeType::Dark);
  // setIsNavigationBarEnable(false);
  // setNavigationBarDisplayMode(ElaNavigationType::Compact);
  // setWindowButtonFlag(ElaAppBarType::MinimizeButtonHint, false);
  setUserInfoCardPixmap(QPixmap(":/Resource/Image/LLK.png"));
  setUserInfoCardTitle("LLK Assist");
  setUserInfoCardSubTitle("基于Soviet的弹幕姬");
  setWindowTitle("LLK Assist");
  // setIsStayTop(true);
  // setUserInfoCardVisible(false);
}

void AssistRuntimeWindow::initEdgeLayout() {
  // 工具栏
  ElaToolBar *toolBar = new ElaToolBar("工具栏", this);
  toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  toolBar->setToolBarSpacing(3);
  toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toolBar->setIconSize(QSize(25, 25));
  // Set logger
  /*
  ElaDockWidget *logDockWidget = new ElaDockWidget("日志信息", this);
  logDockWidget->setWidget(new LogWidget(this));
  this->addDockWidget(Qt::RightDockWidgetArea, logDockWidget);
  resizeDocks({logDockWidget}, {200}, Qt::Horizontal);

  */

  ElaDockWidget *log_dock_widget = new ElaDockWidget("日志信息", this);
  QPlainTextEdit *log_editor = new ElaPlainTextEdit(this);
  log_editor->setReadOnly(true);
  log_dock_widget->setWidget(log_editor);
  this->addDockWidget(Qt::BottomDockWidgetArea, log_dock_widget);
  resizeDocks({log_dock_widget}, {300}, Qt::Horizontal);
  LLKLogger::instance()->addLogger(
      spdlog::qt_logger_mt("qt_logger", log_editor), LLKLogger::Type::GUI);
  GUI_INFO("LLKLogger init success");
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