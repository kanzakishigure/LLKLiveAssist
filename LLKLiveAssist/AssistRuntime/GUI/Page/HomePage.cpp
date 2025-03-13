#include "HomePage.h"

#include <QDebug>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QVBoxLayout>

#include <cstddef>
#include <filesystem>
#include <format>
#include <memory>
#include <qchar.h>
#include <qdatetime.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qwidget.h>
#include <system_error>

#include "Def.h"
#include "ElaAcrylicUrlCard.h"

#include "ElaFlowLayout.h"

#include "ElaImageCard.h"

#include "ElaMenu.h"
#include "ElaMessageBar.h"
#include "ElaNavigationRouter.h"
#include "ElaPivot.h"

#include "ElaScrollArea.h"
#include "ElaScrollPageArea.h"

#include "ElaText.h"
#include "ElaToggleSwitch.h"
#include "ElaToolTip.h"
#include "GSoVITSAssist.h"

#include "GUI/Widgets/TTSConfigWidget.h"
#include "Utils/SignalEmitter.h"
#include "Utils/UrlImageLoader.h"
#include <BiliClientAssist.h>
#include <Data/GSoVITSModel.h>
#include <ModuleManager.h>

#include "GUI/Widgets/LLKPopularCard.h"
#include "GUI/Widgets/LLKUrlCard.h"
#include "Runtime/AssistRuntime.h"
#include <Net/HttpRequest.h>
#include <thread>

#define TEST_ENV true

namespace NAssist {

HomePage::HomePage(QWidget *parent) : BasePage(parent) {

  ////////////////////////////////////////////////////////////////////////////////////
  // 预览窗口标题
  setWindowTitle("Home");

  setTitleVisible(false);
  setContentsMargins(2, 2, 0, 0);
  // 标题卡片区域
  ElaText *des_text = new ElaText("LLK Live Assist", this);
  des_text->setTextPixelSize(38);

  ElaText *title_text =
      new ElaText("欢迎使用露露卡直播小助手  ✿✿ヽ(°▽°)ノ✿", this);
  title_text->setTextPixelSize(24);

  QVBoxLayout *title_layout = new QVBoxLayout();
  title_layout->setContentsMargins(30, 60, 0, 0);
  title_layout->addWidget(des_text);
  title_layout->addWidget(title_text);

  ElaImageCard *background_card = new ElaImageCard(this);
  background_card->setBorderRadius(10);
  background_card->setFixedHeight(400);
  background_card->setMaximumAspectRatio(1.5);
  background_card->setCardImage(QImage(":/Resource/Image/Home_Background.png"));

  ElaAcrylicUrlCard *url_card1 = new ElaAcrylicUrlCard(this);
  url_card1->setCardPixmapSize(QSize(62, 62));
  url_card1->setFixedSize(195, 225);
  url_card1->setTitlePixelSize(17);
  url_card1->setTitleSpacing(25);
  url_card1->setSubTitleSpacing(13);
  url_card1->setUrl("https://github.com/kanzakishigure/LLKLiveAssist");
  url_card1->setCardPixmap(QPixmap(":/Resource/Image/github.png"));
  url_card1->setTitle("LLK Live Assist");
  url_card1->setSubTitle("使用璐璐卡直播小助手");
  ElaToolTip *url_card1_tool_tip = new ElaToolTip(url_card1);
  url_card1_tool_tip->setToolTip(
      "https://github.com/kanzakishigure/LLKLiveAssist");

  LLKUrlCard *anchor_card = new LLKUrlCard(this);
  anchor_card->setCardPixmapSize(QSize(62, 62));
  anchor_card->setFixedSize(195, 225);
  anchor_card->setTitlePixelSize(17);
  anchor_card->setTitleSpacing(25);
  anchor_card->setSubTitleSpacing(13);
  anchor_card->setVisible(false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // toggleSwitch to setup LLK Assist Core
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  ElaToggleSwitch *start_toggle_switch = new ElaToggleSwitch(this);
  ElaScrollPageArea *start_toggle_switch_area = new ElaScrollPageArea(this);
  start_toggle_switch_area->setFixedWidth(280);
  QHBoxLayout *start_toggle_switch_layout =
      new QHBoxLayout(start_toggle_switch_area);
  ElaText *start_toggle_switch_text = new ElaText("Assist Core 已停止", this);

  start_toggle_switch_text->setTextPixelSize(18);
  start_toggle_switch_layout->addWidget(start_toggle_switch_text);
  start_toggle_switch_layout->addSpacing(30);
  start_toggle_switch_layout->addWidget(start_toggle_switch);

  start_toggle_switch_layout->addStretch();

  SignalEmitter *open_emitter = new SignalEmitter(this);
  SignalEmitter *close_emitter = new SignalEmitter(this);
  connect(
      open_emitter, &SignalEmitter::getRequest, this, [=](std::error_code ec) {
        start_toggle_switch->setDisabled(false);
        if (!ec) {

          auto data = ModuleManager::getInstance()
                          .getModule<BiliClientAssist>()
                          ->getAppStartInfo();
          std::string auther_id = fmt::format("https://live.bilibili.com/{}",
                                              data.AnchorInfo.RoomId);
          std::string auther_name = data.AnchorInfo.UName;
          UrlImageLoader loader;

          loader.loadFromUrl(QUrl(data.AnchorInfo.UFace.c_str()));
          anchor_card->setCardPixmap(loader.getQPixmap().copy());
          anchor_card->setUrl(auther_id.c_str());
          anchor_card->setTitle(auther_name.c_str());
          anchor_card->setSubTitle("璐璐卡直播小助手已在监听弹幕 ✿✿ヽ(°▽°)ノ✿");

          start_toggle_switch_text->setText("Assist Core 已启动");
          anchor_card->showCard();

        } else {
          start_toggle_switch_text->setText("Assist Core 启动失败,已停止");
        }
      });
  connect(close_emitter, &SignalEmitter::getRequest, this,
          [=](std::error_code ec) {
            start_toggle_switch->setDisabled(false);
            start_toggle_switch_text->setText("Assist Core 已停止");
            anchor_card->hideCard();
            if (ec) {
              // ElaMessageBar::error(ElaMessageBarType::Right, "Assist
              // Core Error", ec.message().c_str(),0 );
            }
          });
  connect(start_toggle_switch, &ElaToggleSwitch::toggled, this,
          [=](bool checked) {
            if (checked) {
      // code in another thread
#if TEST_ENV
              AssistRuntime::getInstance()->stratAllModule(
                  [open_emitter](const std::error_code ec) {
                    QMetaObject::invokeMethod(open_emitter, "triggerLambda",
                                              Qt::QueuedConnection,
                                              Q_ARG(const std::error_code, ec));
                  });
#else
      AssistRuntime::getInstance()->stratAllModule(
          [start_toggleSwitch, start_toggleSwitchText,
           anchor_card](const std::error_code ec) {
            QMetaObject::invokeMethod(start_toggleSwitch, "setDisabled",
                                      Qt::QueuedConnection, Q_ARG(bool, false));
            if (!ec) {
              QMetaObject::invokeMethod(start_toggleSwitchText, "setText",
                                        Qt::QueuedConnection,
                                        Q_ARG(QString, "Assist Core 已启动"));
              QMetaObject::invokeMethod(anchor_card, "showCard",
                                        Qt::QueuedConnection);

            } else {
              QMetaObject::invokeMethod(
                  start_toggleSwitchText, "setText", Qt::QueuedConnection,
                  Q_ARG(QString, "Assist Core 启动失败,已停止"));
            }
          });
#endif
              // code in another thread
              start_toggle_switch_text->setText("Assist Core 启动中");
              start_toggle_switch->setDisabled(true);

            } else {
      // code in another thread
#if TEST_ENV
              AssistRuntime::getInstance()->stopAllModule(
                  [=](const std::error_code ec) {
                    QMetaObject::invokeMethod(close_emitter, "triggerLambda",
                                              Qt::QueuedConnection,
                                              Q_ARG(const std::error_code, ec));
                  });
#else
      {
        AssistRuntime::getInstance()->stopAllModule(
            [=](const std::error_code ec) {
              QMetaObject::invokeMethod(start_toggleSwitch, "setDisabled",
                                        Qt::QueuedConnection,
                                        Q_ARG(bool, false));
              QMetaObject::invokeMethod(start_toggleSwitchText, "setText",
                                        Qt::QueuedConnection,
                                        Q_ARG(QString, "Assist Core 已停止"));
              QMetaObject::invokeMethod(anchor_card, "hideCard",
                                        Qt::QueuedConnection);
              if (ec) {
                // ElaMessageBar::error(ElaMessageBarType::Right, "Assist Core
                // Error", ec.message().c_str(),0 );
              }
            });
      }
#endif
              // code in another thread
              start_toggle_switch_text->setText("Assist Core 停止中");
              start_toggle_switch->setDisabled(true);
            }
          });
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // github ulr card for llk live assist
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  ElaScrollArea *card_scroll_area = new ElaScrollArea(this);
  card_scroll_area->setWidgetResizable(true);
  card_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  card_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  card_scroll_area->setIsGrabGesture(true, 0);
  card_scroll_area->setIsOverShoot(Qt::Horizontal, true);
  QWidget *card_scroll_area_widget = new QWidget(this);
  card_scroll_area_widget->setStyleSheet("background-color:transparent;");
  card_scroll_area->setWidget(card_scroll_area_widget);
  QHBoxLayout *url_card_layout = new QHBoxLayout();
  url_card_layout->setSpacing(15);
  url_card_layout->setContentsMargins(30, 0, 0, 6);
  url_card_layout->addWidget(url_card1);
  url_card_layout->addWidget(start_toggle_switch_area);
  url_card_layout->addWidget(anchor_card);
  url_card_layout->addStretch();

  QVBoxLayout *card_scroll_area_widget_layout =
      new QVBoxLayout(card_scroll_area_widget);
  card_scroll_area_widget_layout->setContentsMargins(0, 0, 0, 0);
  card_scroll_area_widget_layout->addStretch();
  card_scroll_area_widget_layout->addLayout(url_card_layout);

  QVBoxLayout *background_layout = new QVBoxLayout(background_card);
  background_layout->setContentsMargins(0, 0, 0, 0);
  background_layout->addLayout(title_layout);
  background_layout->addWidget(card_scroll_area);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // plugings config
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  ElaText *flow_text = new ElaText("LLK Core plugings", this);
  flow_text->setTextPixelSize(20);
  QHBoxLayout *flow_text_layout = new QHBoxLayout();
  flow_text_layout->setContentsMargins(33, 0, 0, 0);
  flow_text_layout->addWidget(flow_text);

  // ElaFlowLayout
  ElaFlowLayout *flow_layout = new ElaFlowLayout(0, 5, 5);
  flow_layout->setContentsMargins(30, 0, 0, 0);
  flow_layout->setIsAnimation(true);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // llk core config pivot
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  QVBoxLayout *pivot_layout = new QVBoxLayout();
  pivot_layout->setContentsMargins(30, 0, 0, 0);
  ElaScrollPageArea *pivot_area = new ElaScrollPageArea(this);
  m_pivot = new ElaPivot(pivot_area);
  m_pivot->setPivotSpacing(8);
  m_pivot->setMarkWidth(75);
  m_pivot->appendPivot("BiliBili");
  m_pivot->appendPivot("Models");
  m_pivot->appendPivot("Infer Setting");
  m_pivot->setCurrentIndex(0);
  m_pivot->setTextPixelSize(16);
  QVBoxLayout *pivot_area_layout = new QVBoxLayout(pivot_area);
  pivot_area_layout->addWidget(m_pivot);
  pivot_layout->addWidget(pivot_area);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // biliconfig
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  m_BiliConfigWidget = new BiliConfigWidget(this);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // sovist config
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  m_SovitsConfigWidget = new SovitsConfigWidget(this);
  flushModelCard(
      ModuleManager::getInstance().getModule<GSoVITSAssist>()->getModelLib());
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // audio config
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  m_TTSConfigWidget = new TTSConfigWidget(this);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // 菜单
  m_homeMenu = new ElaMenu(this);

  // QKeySequence key = QKeySequence(Qt::CTRL | Qt::Key_S);

  m_homeMenu->addSeparator();
  m_homeMenu->addElaIconAction(ElaIconType::BoxCheck, "排序方式",
                               QKeySequence::Save);
  m_homeMenu->addElaIconAction(ElaIconType::ArrowRotateRight, "刷新");
  QAction *action =
      m_homeMenu->addElaIconAction(ElaIconType::ArrowRotateLeft, "撤销");
  connect(action, &QAction::triggered, this,
          [=]() { ElaNavigationRouter::getInstance()->navigationRouteBack(); });

  m_homeMenu->addElaIconAction(ElaIconType::Copy, "复制");
  m_homeMenu->addElaIconAction(ElaIconType::MagnifyingGlassPlus, "显示设置");

  QWidget *central_widget = new QWidget(this);
  central_widget->setWindowTitle("Home");
  QVBoxLayout *center_vlayout = new QVBoxLayout(central_widget);
  center_vlayout->setSpacing(0);

  center_vlayout->setContentsMargins(0, 0, 0, 0);
  center_vlayout->addWidget(background_card);
  center_vlayout->addSpacing(20);
  center_vlayout->addLayout(flow_text_layout);
  center_vlayout->addSpacing(10);
  center_vlayout->addLayout(pivot_layout);
  center_vlayout->addSpacing(10);
  center_vlayout->addLayout(flow_layout);

  center_vlayout->addSpacing(10);
  center_vlayout->addWidget(m_BiliConfigWidget);
  center_vlayout->addWidget(m_SovitsConfigWidget);
  center_vlayout->addWidget(m_TTSConfigWidget);
  center_vlayout->addStretch();
  addCentralWidget(central_widget);

  // signal

  connect(m_pivot, &ElaPivot::pivotClicked, this, [=](int index) {
    enum class Datatype : uint32_t {

      bilibili,
      Sovits,
      MiniAudio,
    };
    Datatype t = static_cast<Datatype>(index);
    switch (t) {
    case Datatype::bilibili:

      m_BiliConfigWidget->show();
      m_SovitsConfigWidget->hide();
      m_TTSConfigWidget->hide();
      break;
    case Datatype::Sovits:

      m_BiliConfigWidget->hide();
      m_SovitsConfigWidget->show();
      m_TTSConfigWidget->hide();
      break;
    case Datatype::MiniAudio:
      m_BiliConfigWidget->hide();
      m_SovitsConfigWidget->hide();
      m_TTSConfigWidget->show();

      break;
    }
  });

  // 初始化提示
  ElaMessageBar::success(ElaMessageBarType::BottomRight, "Success",
                         "初始化成功!", 2000);
}

HomePage::~HomePage() = default;

void HomePage::flushModelCard(
    std::shared_ptr<std::vector<GSoVITSModel>> sovits_models) {
  while (auto *item =
             m_SovitsConfigWidget->getModelCardFlowLayout()->takeAt(0)) {
    if (QWidget *widget = item->widget()) {
      widget->deleteLater();
    }
    delete item;
  }

  auto defualt_model = ModuleManager::getInstance()
                           .getModule<GSoVITSAssist>()
                           ->getDefaultModel();
  auto create_model_card = [this, defualt_model](size_t index,
                                                 GSoVITSModel model) {
    LLKPopularCard *model_card = new LLKPopularCard(this);

    connect(model_card, &LLKPopularCard::popularCardButtonClicked, this, [=]() {
      ModuleManager::getInstance().getModule<GSoVITSAssist>()->setDefaultModel(
          index);
      model_card->setSelected(true);
      GUI_INFO("model {} is selected", model.model_name);
      for (size_t i = 0;
           i < m_SovitsConfigWidget->getModelCardFlowLayout()->count(); i++) {
        auto *item = m_SovitsConfigWidget->getModelCardFlowLayout()->itemAt(i);
        if (QWidget *widget = item->widget()) {
          static_cast<LLKPopularCard *>(widget)->setSelected(i == index);
        }
      }
    });

    connect(model_card, &LLKPopularCard::popularCardTestButtonClicked, this,
            [=]() {
              auto model_lib = ModuleManager::getInstance()
                                   .getModule<GSoVITSAssist>()
                                   ->getModelLib();
              auto model = model_lib->at(index);
              ModuleManager::getInstance()
                  .getModule<GSoVITSAssist>()
                  ->commitRequest2GSoVits(model, model.prompt_text);
            });

    connect(model_card, &LLKPopularCard::popularCardVolumeSliderValueChanged,
            this, [=](int value) {
              auto model_lib = ModuleManager::getInstance()
                                   .getModule<GSoVITSAssist>()
                                   ->getModelLib();
              auto &model = model_lib->at(index);
              model.model_volume_factor = value / 100.0f;
            });
    model_card->setCardPixmap(QPixmap(model.model_img.c_str()));
    model_card->setTitle(model.model_name.c_str());
    model_card->setSubTitle(
        fmt::format("Author : {0}", model.model_author).c_str());
    model_card->setDetailedText(model.model_description.c_str());
    model_card->setInteractiveTips(model.model_category.c_str());
    model_card->setCardButtontext("启用");
    model_card->setSelected(model == defualt_model);
    model_card->setCardSliderValue(model.model_volume_factor * 100);
    m_SovitsConfigWidget->getModelCardFlowLayout()->addWidget(model_card);
  };
  for (size_t index = 0; index < sovits_models->size(); index++) {
    create_model_card(index, sovits_models->at(index));
  }
}
void HomePage::mouseReleaseEvent(QMouseEvent *event) {
  switch (event->button()) {
  case Qt::RightButton: {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_homeMenu->popup(event->globalPosition().toPoint());
#else
    m_homeMenu->popup(event->globalPos());
#endif
    break;
  }
  default: {
    break;
  }
  }
  ElaScrollPage::mouseReleaseEvent(event);
}
void HomePage::onGSoVITSModelChanged() {
  flushModelCard(
      ModuleManager::getInstance().getModule<GSoVITSAssist>()->getModelLib());
}

} // namespace NAssist
