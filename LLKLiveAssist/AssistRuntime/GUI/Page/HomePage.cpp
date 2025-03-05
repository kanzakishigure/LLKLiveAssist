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

#include "BiliClientAssist.h"
#include "Data/GSoVITSModel.h"
#include "Def.h"
#include "ElaAcrylicUrlCard.h"
#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaFlowLayout.h"
#include "ElaIcon.h"
#include "ElaIconButton.h"
#include "ElaImageCard.h"
#include "ElaLineEdit.h"
#include "ElaLog.h"
#include "ElaMenu.h"
#include "ElaMessageBar.h"
#include "ElaNavigationRouter.h"
#include "ElaPivot.h"
#include "ElaPopularCard.h"
#include "ElaPushButton.h"
#include "ElaScrollArea.h"
#include "ElaScrollPageArea.h"
#include "ElaSpinBox.h"
#include "ElaText.h"
#include "ElaToggleButton.h"
#include "ElaToggleSwitch.h"
#include "ElaToolTip.h"
#include "GSoVITSAssist.h"
#include "GUI/Widgets/LLKPopularCard.h"

#include "GUI/Widgets/LLKAnimationCard.h"
#include "Utils/SignalEmitter.h"
#include "Utils/UrlImageLoader.h"
#include <ModuleManager.h>
#include <vector>

#include "GUI/Widgets/LLKUrlCard.h"
#include "Net/HttpRequest.h"
#include "Runtime/AssistRuntime.h"

#define TEST_ENV true

namespace NAssist {

HomePage::HomePage(QWidget *parent) : BasePage(parent) {

  ////////////////////////////////////////////////////////////////////////////////////
  // 预览窗口标题
  setWindowTitle("Home");

  setTitleVisible(false);
  setContentsMargins(2, 2, 0, 0);
  // 标题卡片区域
  ElaText *desText = new ElaText("LLK Live Assist", this);
  desText->setTextPixelSize(38);

  ElaText *titleText =
      new ElaText("欢迎使用露露卡直播小助手  ✿✿ヽ(°▽°)ノ✿", this);
  titleText->setTextPixelSize(24);

  QVBoxLayout *titleLayout = new QVBoxLayout();
  titleLayout->setContentsMargins(30, 60, 0, 0);
  titleLayout->addWidget(desText);
  titleLayout->addWidget(titleText);

  ElaImageCard *backgroundCard = new ElaImageCard(this);
  backgroundCard->setBorderRadius(10);
  backgroundCard->setFixedHeight(400);
  backgroundCard->setMaximumAspectRatio(1.5);
  backgroundCard->setCardImage(QImage(":/Resource/Image/Home_Background.png"));

  ElaAcrylicUrlCard *urlCard1 = new ElaAcrylicUrlCard(this);
  urlCard1->setCardPixmapSize(QSize(62, 62));
  urlCard1->setFixedSize(195, 225);
  urlCard1->setTitlePixelSize(17);
  urlCard1->setTitleSpacing(25);
  urlCard1->setSubTitleSpacing(13);
  urlCard1->setUrl("https://github.com/kanzakishigure/LLKLiveAssist");
  urlCard1->setCardPixmap(QPixmap(":/Resource/Image/github.png"));
  urlCard1->setTitle("LLK Live Assist");
  urlCard1->setSubTitle("使用璐璐卡直播小助手");
  ElaToolTip *urlCard1ToolTip = new ElaToolTip(urlCard1);
  urlCard1ToolTip->setToolTip(
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
  ElaToggleSwitch *start_toggleSwitch = new ElaToggleSwitch(this);
  ElaScrollPageArea *start_toggleSwitchArea = new ElaScrollPageArea(this);
  start_toggleSwitchArea->setFixedWidth(280);
  QHBoxLayout *start_toggleSwitchLayout =
      new QHBoxLayout(start_toggleSwitchArea);
  ElaText *start_toggleSwitchText = new ElaText("Assist Core 已停止", this);

  start_toggleSwitchText->setTextPixelSize(18);
  start_toggleSwitchLayout->addWidget(start_toggleSwitchText);
  start_toggleSwitchLayout->addSpacing(30);
  start_toggleSwitchLayout->addWidget(start_toggleSwitch);

  start_toggleSwitchLayout->addStretch();

  SignalEmitter *open_emitter = new SignalEmitter(this);
  SignalEmitter *close_emitter = new SignalEmitter(this);
  connect(open_emitter, &SignalEmitter::getRequest, this,
          [=](std::error_code ec) {
            start_toggleSwitch->setDisabled(false);
            if (!ec) {

              auto data = ModuleManager::getInstance()
                              .getModule<BiliClientAssist>()
                              ->getAppStartInfo();
              std::string auther_id = fmt::format(
                  "https://live.bilibili.com/{}", data.AnchorInfo.RoomId);
              std::string auther_name = data.AnchorInfo.UName;
              UrlImageLoader loader;
              
              loader.loadFromUrl(QUrl(data.AnchorInfo.UFace.c_str()));
              anchor_card->setCardPixmap(loader.getQPixmap().copy());
              anchor_card->setUrl(auther_id.c_str());
              anchor_card->setTitle(auther_name.c_str());
              anchor_card->setSubTitle("璐璐卡直播小助手已在监听弹幕 ✿✿ヽ(°▽°)ノ✿");
              
              start_toggleSwitchText->setText("Assist Core 已启动");
              anchor_card->showCard();

            } else {
              start_toggleSwitchText->setText("Assist Core 启动失败,已停止");
            }
          });
  connect(close_emitter, &SignalEmitter::getRequest, this,
          [=](std::error_code ec) {
            start_toggleSwitch->setDisabled(false);
            start_toggleSwitchText->setText("Assist Core 已停止");
            anchor_card->hideCard();
            if (ec) {
              // ElaMessageBar::error(ElaMessageBarType::Right, "Assist
              // Core Error", ec.message().c_str(),0 );
            }
          });
  connect(start_toggleSwitch, &ElaToggleSwitch::toggled, this,
          [=](bool checked) {
            if (checked) {
              auto audio_assist =
                  ModuleManager::getInstance().getModule<AudioAssist>();
              auto sovits_assist =
                  ModuleManager::getInstance().getModule<GSoVITSAssist>();
              audio_assist->setAudioConfig(m_audio_config_data);
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
              start_toggleSwitchText->setText("Assist Core 启动中");
              start_toggleSwitch->setDisabled(true);

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
              start_toggleSwitchText->setText("Assist Core 停止中");
              start_toggleSwitch->setDisabled(true);
            }
          });
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // github ulr card for llk live assist
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  ElaScrollArea *cardScrollArea = new ElaScrollArea(this);
  cardScrollArea->setWidgetResizable(true);
  cardScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  cardScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  cardScrollArea->setIsGrabGesture(true, 0);
  cardScrollArea->setIsOverShoot(Qt::Horizontal, true);
  QWidget *cardScrollAreaWidget = new QWidget(this);
  cardScrollAreaWidget->setStyleSheet("background-color:transparent;");
  cardScrollArea->setWidget(cardScrollAreaWidget);
  QHBoxLayout *urlCardLayout = new QHBoxLayout();
  urlCardLayout->setSpacing(15);
  urlCardLayout->setContentsMargins(30, 0, 0, 6);
  urlCardLayout->addWidget(urlCard1);
  urlCardLayout->addWidget(start_toggleSwitchArea);
  urlCardLayout->addWidget(anchor_card);
  urlCardLayout->addStretch();

  QVBoxLayout *cardScrollAreaWidgetLayout =
      new QVBoxLayout(cardScrollAreaWidget);
  cardScrollAreaWidgetLayout->setContentsMargins(0, 0, 0, 0);
  cardScrollAreaWidgetLayout->addStretch();
  cardScrollAreaWidgetLayout->addLayout(urlCardLayout);

  QVBoxLayout *backgroundLayout = new QVBoxLayout(backgroundCard);
  backgroundLayout->setContentsMargins(0, 0, 0, 0);
  backgroundLayout->addLayout(titleLayout);
  backgroundLayout->addWidget(cardScrollArea);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // plugings config
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  ElaText *flowText = new ElaText("LLK Core plugings", this);
  flowText->setTextPixelSize(20);
  QHBoxLayout *flowTextLayout = new QHBoxLayout();
  flowTextLayout->setContentsMargins(33, 0, 0, 0);
  flowTextLayout->addWidget(flowText);

  // ElaFlowLayout

  ElaFlowLayout *flowLayout = new ElaFlowLayout(0, 5, 5);
  flowLayout->setContentsMargins(30, 0, 0, 0);
  flowLayout->setIsAnimation(true);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // llk core config pivot
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  QVBoxLayout *pivotLayout = new QVBoxLayout();
  pivotLayout->setContentsMargins(30, 0, 0, 0);
  ElaScrollPageArea *pivotArea = new ElaScrollPageArea(this);
  m_pivot = new ElaPivot(pivotArea);
  m_pivot->setPivotSpacing(8);
  m_pivot->setMarkWidth(75);
  m_pivot->appendPivot("BiliBili");
  m_pivot->appendPivot("GPT-Sovits");
  m_pivot->appendPivot("MiniAudio");
  m_pivot->setCurrentIndex(0);
  m_pivot->setTextPixelSize(16);
  QVBoxLayout *pivotAreaLayout = new QVBoxLayout(pivotArea);
  pivotAreaLayout->addWidget(m_pivot);
  pivotLayout->addWidget(pivotArea);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // biliconfig
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  int line_editor_hight = 35;
  QWidget *bili_config = new QWidget(this);
  QVBoxLayout *bili_config_layout = new QVBoxLayout();
  bili_config->setLayout(bili_config_layout);

  bili_config_layout->setContentsMargins(30, 0, 0, 0);
  bili_config_layout->addStretch();

  {

    ElaText *biliconfig_area_title = new ElaText("BILIBILI config", this);
    biliconfig_area_title->setTextPointSize(8);

    auto bili_config_data = ModuleManager::getInstance()
                                .getModule<BiliClientAssist>()
                                ->getUserCode();
    auto uid_line_edit = new ElaLineEdit(this);
    uid_line_edit->setText(bili_config_data.c_str());
    uid_line_edit->setFixedWidth(160);
    uid_line_edit->setFixedHeight(line_editor_hight);
    uid_line_edit->setMaxLength(13);
    ElaScrollPageArea *uid_textbox_area = new ElaScrollPageArea(this);
    QHBoxLayout *uid_texbox_area_Layout = new QHBoxLayout(uid_textbox_area);
    ElaText *toggleButtonText = new ElaText("用户身份码：", this);
    toggleButtonText->setTextPixelSize(15);

    uid_texbox_area_Layout->addWidget(toggleButtonText);
    uid_texbox_area_Layout->addWidget(uid_line_edit);
    uid_texbox_area_Layout->addStretch();

    ElaPushButton *push_button = new ElaPushButton("获取身份码", this);

    connect(push_button, &ElaPushButton::pressed, this, [=]() {
      QDesktopServices::openUrl(QUrl("https://play-live.bilibili.com/"));
    });

    uid_texbox_area_Layout->addWidget(push_button);

    uid_texbox_area_Layout->addSpacing(10);

    bili_config_layout->setSpacing(10);
    bili_config_layout->addWidget(biliconfig_area_title);
    bili_config_layout->addSpacing(10);
    bili_config_layout->addWidget(uid_textbox_area);
    bili_config_layout->addStretch();

    connect(uid_line_edit, &ElaLineEdit::focusOut, this, [=](QString text) {
      ModuleManager::getInstance().getModule<BiliClientAssist>()->setUserCode(
          text.toStdString());
      qDebug() << ModuleManager::getInstance()
                      .getModule<BiliClientAssist>()
                      ->getUserCode()
                      .c_str();
    });
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // sovist config
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  QWidget *sovits_config = new QWidget(this);
  QVBoxLayout *sovits_config_layout = new QVBoxLayout();
  sovits_config->setLayout(sovits_config_layout);
  sovits_config->hide();

  sovits_config_layout->setContentsMargins(30, 0, 0, 0);
  sovits_config_layout->addStretch();

  {

    ElaText *sovits_config_title = new ElaText("GPT-Sovits config", this);
    sovits_config_title->setTextPointSize(8);

    sovits_config_layout->setSpacing(10);
    sovits_config_layout->addWidget(sovits_config_title);

    auto *model_card_flow_area = new QWidget(this);
    model_card_flow_layout = new ElaFlowLayout(model_card_flow_area);

    sovits_config_layout->addWidget(model_card_flow_area);

    flushModelCard(
        ModuleManager::getInstance().getModule<GSoVITSAssist>()->getModelLib());
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // audio config
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  QWidget *audio_config = new QWidget(this);
  QVBoxLayout *audio_config_layout = new QVBoxLayout();
  audio_config->setLayout(audio_config_layout);
  audio_config->hide();

  audio_config_layout->setContentsMargins(30, 0, 0, 0);
  audio_config_layout->addStretch();

  {
    ElaText *audio_config_title = new ElaText("Miniaudio config", this);
    audio_config_title->setTextPointSize(8);

    audio_config_layout->setSpacing(10);
    audio_config_layout->addWidget(audio_config_title);

    ma_format Dcoderformat = ma_format_s16;
    int DcoderSampleRate = 16000;
    int AudioSampleRate = 16000;

    m_audio_config_data =
        ModuleManager::getInstance().getModule<AudioAssist>()->getAudioConfig();

    {
      QString propety_name = "Dcoderformat";
      ma_format format = m_audio_config_data.Dcoderformat;

      m_comboBox = new ElaComboBox(this);
      QStringList comboList{
          "ma_format_u8",  "ma_format_s16", "ma_format_s24",
          "ma_format_s32", "ma_format_f32",
      };
      m_comboBox->addItems(comboList);
      m_comboBox->setCurrentIndex(static_cast<int>(Dcoderformat) - 1);
      m_comboBox->setFixedWidth(165);
      ElaScrollPageArea *comboBoxArea = new ElaScrollPageArea(this);
      QHBoxLayout *comboBoxLayout = new QHBoxLayout(comboBoxArea);
      ElaText *comboBoxText = new ElaText(propety_name, this);
      comboBoxText->setTextPixelSize(15);
      comboBoxLayout->addWidget(comboBoxText);
      comboBoxLayout->addWidget(m_comboBox);
      comboBoxLayout->addStretch();

      connect(m_comboBox, QOverload<int>::of(&ElaComboBox::activated), this,
              [this](int index) {
                m_audio_config_data.Dcoderformat =
                    static_cast<ma_format>(index + 1);
                auto audio_assist =
                    ModuleManager::getInstance().getModule<AudioAssist>();
                audio_assist->setAudioConfig(m_audio_config_data);
              });

      audio_config_layout->addSpacing(10);
      audio_config_layout->addWidget(comboBoxArea);
    }
    {
      QString propety_name = "DcoderSampleRate";
      int propety_value = m_audio_config_data.DcoderSampleRate;
      auto propety_value_line_edit = new ElaLineEdit(this);
      propety_value_line_edit->setText(QString::number(propety_value));
      propety_value_line_edit->setValidator(new QIntValidator());
      propety_value_line_edit->setFixedHeight(line_editor_hight);
      propety_value_line_edit->setFixedWidth(100);
      propety_value_line_edit->setMaxLength(5);

      ElaScrollPageArea *textbox_area = new ElaScrollPageArea(this);
      QHBoxLayout *texbox_area_Layout = new QHBoxLayout(textbox_area);
      ElaText *propety_name_text = new ElaText(propety_name, this);
      propety_name_text->setTextPixelSize(15);
      propety_name_text->setFixedWidth(150);
      texbox_area_Layout->addWidget(propety_name_text);
      texbox_area_Layout->addWidget(propety_value_line_edit);
      texbox_area_Layout->addStretch();

      connect(propety_value_line_edit, &ElaLineEdit::focusOut, this,
              [this](QString text) {
                m_audio_config_data.DcoderSampleRate = text.toInt();
                auto audio_assist =
                    ModuleManager::getInstance().getModule<AudioAssist>();
                audio_assist->setAudioConfig(m_audio_config_data);
              });

      audio_config_layout->addSpacing(10);
      audio_config_layout->addWidget(textbox_area);
    }

    {
      QString propety_name = "AudioSampleRate";
      int propety_value = m_audio_config_data.AudioSampleRate;
      auto propety_value_line_edit = new ElaLineEdit(this);
      propety_value_line_edit->setText(QString::number(propety_value));
      propety_value_line_edit->setValidator(new QIntValidator());
      propety_value_line_edit->setFixedHeight(line_editor_hight);
      propety_value_line_edit->setFixedWidth(100);
      propety_value_line_edit->setMaxLength(5);

      ElaScrollPageArea *textbox_area = new ElaScrollPageArea(this);
      QHBoxLayout *texbox_area_Layout = new QHBoxLayout(textbox_area);
      ElaText *propety_name_text = new ElaText(propety_name, this);
      propety_name_text->setTextPixelSize(15);
      propety_name_text->setFixedWidth(150);
      texbox_area_Layout->addWidget(propety_name_text);

      texbox_area_Layout->addWidget(propety_value_line_edit);
      texbox_area_Layout->addStretch();

      connect(propety_value_line_edit, &ElaLineEdit::focusOut, this,
              [this](QString text) {
                m_audio_config_data.DcoderSampleRate = text.toInt();
                auto audio_assist =
                    ModuleManager::getInstance().getModule<AudioAssist>();
                audio_assist->setAudioConfig(m_audio_config_data);
              });

      audio_config_layout->addSpacing(10);
      audio_config_layout->addWidget(textbox_area);
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // 菜单
  _homeMenu = new ElaMenu(this);

  // QKeySequence key = QKeySequence(Qt::CTRL | Qt::Key_S);

  _homeMenu->addSeparator();
  _homeMenu->addElaIconAction(ElaIconType::BoxCheck, "排序方式",
                              QKeySequence::Save);
  _homeMenu->addElaIconAction(ElaIconType::ArrowRotateRight, "刷新");
  QAction *action =
      _homeMenu->addElaIconAction(ElaIconType::ArrowRotateLeft, "撤销");
  connect(action, &QAction::triggered, this,
          [=]() { ElaNavigationRouter::getInstance()->navigationRouteBack(); });

  _homeMenu->addElaIconAction(ElaIconType::Copy, "复制");
  _homeMenu->addElaIconAction(ElaIconType::MagnifyingGlassPlus, "显示设置");

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle("Home");
  QVBoxLayout *centerVLayout = new QVBoxLayout(centralWidget);
  centerVLayout->setSpacing(0);

  centerVLayout->setContentsMargins(0, 0, 0, 0);
  centerVLayout->addWidget(backgroundCard);
  centerVLayout->addSpacing(20);
  centerVLayout->addLayout(flowTextLayout);
  centerVLayout->addSpacing(10);
  centerVLayout->addLayout(pivotLayout);
  centerVLayout->addSpacing(10);
  centerVLayout->addLayout(flowLayout);

  centerVLayout->addSpacing(10);
  centerVLayout->addWidget(bili_config);
  centerVLayout->addWidget(sovits_config);
  centerVLayout->addWidget(audio_config);
  centerVLayout->addStretch();
  addCentralWidget(centralWidget);

  // signal

  connect(m_pivot, &ElaPivot::pivotClicked, this, [=](int index) {
    enum class datatype : uint32_t {

      bilibili,
      Sovits,
      MiniAudio,
    };
    datatype t = static_cast<datatype>(index);
    switch (t) {
    case datatype::bilibili:

      bili_config->show();
      sovits_config->hide();
      audio_config->hide();
      break;
    case datatype::Sovits:

      bili_config->hide();
      sovits_config->show();
      audio_config->hide();
      break;
    case datatype::MiniAudio:
      bili_config->hide();
      sovits_config->hide();
      audio_config->show();

      break;
    }
  });

  // 初始化提示
  ElaMessageBar::success(ElaMessageBarType::BottomRight, "Success",
                         "初始化成功!", 2000);
}

HomePage::~HomePage() {}

void HomePage::flushModelCard(
    std::shared_ptr<std::vector<GSoVITSModel>> sovits_models) {
  while (auto *item = model_card_flow_layout->takeAt(0)) {
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
      for (size_t i = 0; i < model_card_flow_layout->count(); i++) {
        auto *item = model_card_flow_layout->itemAt(i);
        if (QWidget *widget = item->widget()) {
          static_cast<LLKPopularCard *>(widget)->setSelected(i == index);
        }
      }
    });
    model_card->setCardPixmap(QPixmap(model.model_img.c_str()));
    model_card->setTitle(model.model_name.c_str());
    model_card->setSubTitle(
        fmt::format("Author : {0}", model.model_author).c_str());
    model_card->setDetailedText(model.model_description.c_str());
    model_card->setInteractiveTips(model.model_category.c_str());
    model_card->setCardButtontext("启用");
    model_card->setSelected(model == defualt_model);
    model_card_flow_layout->addWidget(model_card);
  };
  for (size_t index = 0; index < sovits_models->size(); index++) {
    create_model_card(index, sovits_models->at(index));
  }
}
void HomePage::mouseReleaseEvent(QMouseEvent *event) {
  switch (event->button()) {
  case Qt::RightButton: {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    _homeMenu->popup(event->globalPosition().toPoint());
#else
    _homeMenu->popup(event->globalPos());
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
