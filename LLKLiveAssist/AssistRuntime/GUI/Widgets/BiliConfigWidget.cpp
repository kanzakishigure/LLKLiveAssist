#include "BiliConfigWidget.h"

#include "GUI/Theme/LLKStyle.h"
#include <ElaLineEdit.h>
#include <ElaPushButton.h>
#include <ElaScrollPageArea.h>
#include <ElaText.h>
#include <ElaTheme.h>
#include <ModuleManager.h>
#include <QDesktopServices.h>
#include <QUrl.h>
#include <QVBoxLayout>
#include <qwidget.h>

namespace NAssist {

BiliConfigWidget::BiliConfigWidget(QWidget *parent) : QWidget(parent) {

  QVBoxLayout *bili_config_layout = new QVBoxLayout(this);
  setLayout(bili_config_layout);

  bili_config_layout->setContentsMargins(30, 0, 0, 0);
  bili_config_layout->addStretch();

  {

    ElaText *biliconfig_area_title = new ElaText("BILIBILI config", this);
    biliconfig_area_title->setTextPointSize(8);

    auto bili_config_data = ModuleManager::getInstance()
                                .getModule<BiliClientAssist>()
                                ->getUserCode();
    ElaLineEdit *uid_line_edit = new ElaLineEdit(this);
    uid_line_edit->setText(bili_config_data.c_str());
    uid_line_edit->setFixedWidth(160);
    uid_line_edit->setFixedHeight(LLKStyle::LineEditFixedHeight);
    uid_line_edit->setMaxLength(13);
    ElaScrollPageArea *uid_textbox_area = new ElaScrollPageArea(this);
    uid_textbox_area->setFixedHeight(LLKStyle::ScrollPageAreaFixedHeight);
    QHBoxLayout *uid_texbox_area_layout = new QHBoxLayout(uid_textbox_area);
    ElaText *toggle_button_text = new ElaText("用户身份码：", this);
    toggle_button_text->setTextPixelSize(15);

    uid_texbox_area_layout->addWidget(toggle_button_text);
    uid_texbox_area_layout->addSpacing(10);
    uid_texbox_area_layout->addWidget(uid_line_edit);
    uid_texbox_area_layout->addStretch();

    ElaPushButton *push_button = new ElaPushButton("获取身份码", this);
    push_button->setFixedHeight(LLKStyle::PushButtonFixedHeight);
    auto font = push_button->font();
    font.setPixelSize(13);
    push_button->setFont(font);

    push_button->setLightDefaultColor(
        ElaThemeColor(ElaThemeType::Light, PrimaryNormal));
    push_button->setLightHoverColor(
        ElaThemeColor(ElaThemeType::Light, PrimaryHover));
    push_button->setLightPressColor(
        ElaThemeColor(ElaThemeType::Light, PrimaryPress));
    push_button->setLightTextColor(Qt::white);
    push_button->setDarkDefaultColor(
        ElaThemeColor(ElaThemeType::Dark, PrimaryNormal));
    push_button->setDarkHoverColor(
        ElaThemeColor(ElaThemeType::Dark, PrimaryHover));
    push_button->setDarkPressColor(
        ElaThemeColor(ElaThemeType::Dark, PrimaryPress));
    push_button->setDarkTextColor(Qt::white);
    connect(push_button, &ElaPushButton::pressed, this, [=]() {
      QDesktopServices::openUrl(QUrl("https://play-live.bilibili.com/"));
    });

    uid_texbox_area_layout->addWidget(push_button);

    uid_texbox_area_layout->addSpacing(10);

    bili_config_layout->setSpacing(10);
    bili_config_layout->addWidget(biliconfig_area_title);
    bili_config_layout->addSpacing(10);
    bili_config_layout->addWidget(uid_textbox_area);
    bili_config_layout->addStretch();

    connect(uid_line_edit, &ElaLineEdit::focusOut, this, [=](QString text) {
      ModuleManager::getInstance().getModule<BiliClientAssist>()->setUserCode(
          text.toStdString());
    });
  }
}
} // namespace NAssist