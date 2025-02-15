#include "AssistRuntimeWindow.h"
#include "ElaWindow.h"

#include <QDebug>
#include <QFontDatabase>
#include <QGraphicsView>
#include <QHBoxLayout>

#include <QApplication>
#include <QCursor>
#include <QFontDatabase>
#include <QWidget>
#include <qnamespace.h>

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
#include "Page/LogWidget.h"


namespace NAssist {

AssistRuntimeWindow::AssistRuntimeWindow(QWidget *parent) : ElaWindow(parent) {
  setWindowTitle("LLKAssit");

  initWindow();

  // 额外布局
  initEdgeLayout();

  // 中心窗口
  initContent();

  // 拦截默认关闭事件
  _closeDialog = new ElaContentDialog(this);
  connect(_closeDialog, &ElaContentDialog::rightButtonClicked, this,
          &AssistRuntimeWindow::closeWindow);
  connect(_closeDialog, &ElaContentDialog::middleButtonClicked, this, [=]() {
    _closeDialog->close();
    showMinimized();
  });
  this->setIsDefaultClosed(false);
  connect(this, &AssistRuntimeWindow::closeButtonClicked, this,
          [=]() { _closeDialog->exec(); });

  // 移动到中心
  moveToCenter();
}

void AssistRuntimeWindow::initWindow() {

  // setIsEnableMica(true);
  // setIsCentralStackedWidgetTransparent(true);
  setWindowIcon(QIcon(":/Resource/Image/LLK.jpg"));
  resize(1200, 740);
  // ElaLog::getInstance()->initMessageLog(true);
  eTheme->setThemeMode(ElaThemeType::Dark);
  // setIsNavigationBarEnable(false);
  // setNavigationBarDisplayMode(ElaNavigationType::Compact);
  // setWindowButtonFlag(ElaAppBarType::MinimizeButtonHint, false);
  setUserInfoCardPixmap(QPixmap(":/Resource/Image/LLK.jpg"));
  setUserInfoCardTitle("LLK Assist");
  setUserInfoCardSubTitle("基于Soviet的弹幕姬");
  setWindowTitle("LLK Assist");
  // setIsStayTop(true);
  // setUserInfoCardVisible(false);

  // 默认字体
  QFont font = qApp->font();
  font.setPixelSize(13);
  font.setFamily(" ");
  font.setHintingPreference(QFont::PreferNoHinting);
  qApp->setFont(font);
}

void AssistRuntimeWindow::initEdgeLayout() {
  // 工具栏
  ElaToolBar *toolBar = new ElaToolBar("工具栏", this);
  toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  toolBar->setToolBarSpacing(3);
  toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toolBar->setIconSize(QSize(25, 25));
  // Set logger
  ElaDockWidget *logDockWidget = new ElaDockWidget("日志信息", this);
  logDockWidget->setWidget(new LogWidget(this));
  this->addDockWidget(Qt::RightDockWidgetArea, logDockWidget);
  resizeDocks({logDockWidget}, {200}, Qt::Horizontal);
}

void AssistRuntimeWindow::initContent() {

  m_homePage = new HomePage(this);
  addPageNode("HOME", m_homePage, ElaIconType::House);
}

} // namespace NAssist