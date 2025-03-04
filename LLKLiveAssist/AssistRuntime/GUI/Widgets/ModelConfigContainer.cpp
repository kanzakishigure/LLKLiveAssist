#include "ModelConfigContainer.h"
#include "Core/FileSystem.h"
#include "Core/logger.h"
#include "Data/GSoVITSModel.h"
#include "ElaIcon.h"
#include "ElaIconButton.h"
#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaScrollArea.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaWidget.h"
#include <ElaIconButton.h>
#include <ElaImageCard.h>
#include <ElaPlainTextEdit.h>
#include <ElaScrollPageArea.h>
#include <ElaToolButton.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
#include <filesystem>
#include <functional>
#include <qboxlayout.h>
#include <qchar.h>
#include <qgridlayout.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <string>

namespace NAssist {

void ModelConfigContainer::checkPath(std::filesystem::path path) {
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    if (entry.is_directory()) {
      checkPath(entry.path());
    } else if (entry.is_regular_file()) {

      GUI_INFO("  Type: {0}, Size: {1} bytes",
               entry.path().extension().string(), entry.file_size());
    }

    if (entry.path().extension() == ".jpg" ||
        entry.path().extension() == ".png") {
      setmodel_img_path(entry.path().string().c_str());
      Q_EMIT valuechanged("model_img_path", entry.path().string().c_str());
    }
    if (entry.path().extension() == ".pth") {
      setsovits_weights(entry.path().string().c_str());
      Q_EMIT valuechanged("sovits_weights", entry.path().string().c_str());
    }
    if (entry.path().extension() == ".ckpt") {
      setgpt_weights(entry.path().string().c_str());
      Q_EMIT valuechanged("gpt_weights", entry.path().string().c_str());
    }
    if (entry.path().extension() == ".wav") {
      setref_audio_path(entry.path().string().c_str());
      setprompt_text(entry.path().filename().string().c_str());
      Q_EMIT valuechanged("ref_audio_path", entry.path().string().c_str());
      Q_EMIT valuechanged("prompt_text",
                          entry.path().filename().string().c_str());
    }
  }
}
ModelConfigContainer::ModelConfigContainer(QWidget *parent) : QWidget(parent) {

  setStyleSheet("#ModelConfigContainer{background-color:transparent;}");
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  QFont textFont = font();
  textFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
  textFont.setPixelSize(16);
  setFont(textFont);
  _themeMode = eTheme->getThemeMode();
  _themeCard = _themeMode == ElaThemeType::ThemeMode::Dark
                   ? ":/Resource/Image/Home_Background_Dark.png"
                   : ":/Resource/Image/Home_Background.png";
  connect(eTheme, &ElaTheme::themeModeChanged, this,
          [=](ElaThemeType::ThemeMode themeMode) {
            this->_themeMode = themeMode;
            this->_themeCard = themeMode == ElaThemeType::ThemeMode::Dark
                                   ? ":/Resource/Image/Home_Background_Dark.png"
                                   : ":/Resource/Image/Home_Background.png";
            this->m_header_area->setCardImage(QImage(_themeCard));
          });

  setFixedSize(720, 540);
  auto *content_area_layout = new QVBoxLayout(this);
  content_area_layout->setAlignment(Qt::AlignTop);
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // model name author category image
  //////////////////////////////////////////////////////////////////////////////////////////////////
  {
    m_header_area = new ElaImageCard(this);
    m_header_area->setCardImage(QImage(_themeCard));
    m_header_area->setFixedHeight(150);
    auto *header_area_layout = new QVBoxLayout(m_header_area);
    header_area_layout->setContentsMargins(20, 0, 20, 0);
    {
      auto *property_area = new QWidget(this);
      auto *property_area_layout = new QGridLayout(property_area);

      auto create_property = [this](QString property_name,
                                    QString property_type,
                                    std::function<void(QString)> bindfunc) {
        QWidget *property_set_area = new QWidget(this);
        QHBoxLayout *property_set_area_layout =
            new QHBoxLayout(property_set_area);
        ElaText *property_text = new ElaText(property_name, this);
        property_text->setTextPixelSize(14);
        property_text->setFixedWidth(80);
        ElaLineEdit *property_edit = new ElaLineEdit(this);
        property_edit->setFixedWidth(150);
        property_set_area_layout->addWidget(property_text);
        property_set_area_layout->addWidget(property_edit);
        connect(property_edit, &ElaLineEdit::focusOut,
                [bindfunc](QString text) { bindfunc(text); });

        connect(this, &ModelConfigContainer::valuechanged,
                [=](QString name, QString value) {
                  if (property_type == name) {
                    property_edit->setText(value);
                  }
                });
        return property_set_area;
      };
      auto *name_property =
          create_property("Name : ", "model_name",
                          [this](QString text) { setmodel_name(text); });
      auto *author_property =
          create_property("Author : ", "model_author",
                          [this](QString text) { setmodel_author(text); });
      auto *category_property =
          create_property("Category : ", "model_category",
                          [this](QString text) { setmodel_category(text); });
      // icon

      ElaIconButton *icon_image =
          new ElaIconButton(QPixmap(":/Resource/Image/model.png"), this);
      icon_image->setFixedSize(90, 90);
      connect(icon_image, &ElaIconButton::clicked, [this, icon_image]() {
        auto path = FileSystem::OpenFileDialog((HWND)(this->window()->winId()));
        if (path.extension() == ".jpg" || path.extension() == ".png") {
          icon_image->setPixmap(QString(path.string().c_str()));
        }
        this->setmodel_img_path(path.string().c_str());
      });
      connect(this, &ModelConfigContainer::valuechanged,
              [=](QString name, QString value) {
                if ("model_img_path" == name) {
                  value.isEmpty() ? icon_image->setPixmap(
                    QPixmap(":/Resource/Image/model.png").copy())
                                  : icon_image->setPixmap(value);
                }
                GUI_INFO("model_img_path is {}",value.isEmpty());
              });
      property_area_layout->addWidget(icon_image, 0, 0);
      property_area_layout->addWidget(author_property, 0, 1);
      property_area_layout->addWidget(name_property, 1, 0);
      property_area_layout->addWidget(category_property, 1, 1);

      header_area_layout->addWidget(property_area);
    }
    content_area_layout->addWidget(m_header_area);
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////
  {
    ElaScrollArea *scoll_area = new ElaScrollArea(this);
    QWidget *scoll_content = new QWidget(scoll_area);
    QVBoxLayout *scoll_content_laytout = new QVBoxLayout(scoll_content);
    content_area_layout->addWidget(scoll_area);
    scoll_area->setWidgetResizable(true);
    scoll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scoll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scoll_area->setIsGrabGesture(true, 0);
    scoll_area->setIsOverShoot(Qt::Vertical, true);
    scoll_area->setWidget(scoll_content);
    scoll_content->setStyleSheet("background-color:transparent;");

    auto *detail_area = new QWidget(this);
    auto *detail_area_laytout = new QHBoxLayout(detail_area);
    ElaText *detail_area_title = new ElaText("Model Config ", this);
    detail_area_title->setTextPixelSize(16);

    ElaToolButton *file_icon_button = new ElaToolButton(this);
    file_icon_button->setElaIcon(ElaIconType::FileImport);
    file_icon_button->setText("从文件夹导入");
    file_icon_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    detail_area_laytout->addWidget(detail_area_title);
    detail_area_laytout->addSpacing(30);
    detail_area_laytout->addWidget(file_icon_button);

    scoll_content_laytout->addWidget(detail_area);
    ElaScrollPageArea* content_separator = new ElaScrollPageArea(this);
    content_separator->setFixedHeight(3);
    scoll_content_laytout->addWidget(content_separator);
    scoll_content_laytout->addSpacing(20);

    connect(file_icon_button, &ElaToolButton::clicked, [this]() {
      auto path = FileSystem::OpenFolderDialog();
      checkPath(path);
    });
    auto create_line_edit_property =
        [this](QString property_name, std::function<void(QString)> bindfunc) {
          ElaScrollPageArea *property_set_area = new ElaScrollPageArea(this);
          QHBoxLayout *property_set_area_layout =
              new QHBoxLayout(property_set_area);
          property_set_area->setFixedHeight(40);
          ElaText *property_text = new ElaText(property_name, this);
          property_text->setTextPixelSize(14);

          ElaLineEdit *property_edit = new ElaLineEdit(this);
          property_edit->setFixedWidth(200);
          property_edit->setFixedHeight(20);
          property_set_area_layout->addWidget(property_text);
          property_set_area_layout->addWidget(property_edit);
          connect(property_edit, &ElaLineEdit::focusOut,
                  [this, bindfunc](QString text) { bindfunc(text); });

          connect(this, &ModelConfigContainer::valuechanged,
                  [=](QString name, QString text) {
                    if (property_name == name)
                      property_edit->setText(text);
                  });

          return property_set_area;
        };

    auto create_text_edit_property =
        [this](QString property_name,
               std::function<void(ElaPlainTextEdit *)> bindfunc) {
          ElaScrollPageArea *property_set_area = new ElaScrollPageArea(this);
          property_set_area->setFixedHeight(200);
          QVBoxLayout *property_set_area_layout =
              new QVBoxLayout(property_set_area);
          ElaText *property_text = new ElaText(property_name, this);
          property_text->setTextPixelSize(14);

          ElaPlainTextEdit *property_text_edit = new ElaPlainTextEdit(this);
          property_set_area_layout->addWidget(property_text);
          property_set_area_layout->addWidget(property_text_edit);
          connect(property_text_edit, &ElaPlainTextEdit::textChanged,
                  [this, property_text_edit, bindfunc]() {
                    bindfunc(property_text_edit);
                  });
          connect(this, &ModelConfigContainer::valuechanged,
                  [=](QString name, QString text) {
                    if (property_name == name)
                      property_text_edit->setPlainText(text);
                  });
          return property_set_area;
        };

    auto *sovits_weights = create_line_edit_property(
        "sovits_weights", [this](QString text) { setsovits_weights(text); });
    auto *gpt_weights = create_line_edit_property(
        "gpt_weights", [this](QString text) { setgpt_weights(text); });
    auto *ref_audio_path = create_line_edit_property(
        "ref_audio_path", [this](QString text) { setref_audio_path(text); });

    auto *prompt_lang = create_line_edit_property(
        "prompt_lang", [this](QString text) { setprompt_lang(text); });
    auto *prompt_text = create_text_edit_property(
        "prompt_text", [this](ElaPlainTextEdit *text_edit) {
          setprompt_text(text_edit->toPlainText());
        });
    auto *model_description = create_text_edit_property(
        "model_description", [this](ElaPlainTextEdit *text_edit) {
          setmodel_description(text_edit->toPlainText());
        });
    scoll_content_laytout->addWidget(sovits_weights);
    scoll_content_laytout->addSpacing(10);
    scoll_content_laytout->addWidget(gpt_weights);
    scoll_content_laytout->addSpacing(10);
    scoll_content_laytout->addWidget(ref_audio_path);
    scoll_content_laytout->addSpacing(10);
    scoll_content_laytout->addWidget(prompt_lang);
    scoll_content_laytout->addSpacing(10);
    scoll_content_laytout->addWidget(prompt_text);
    scoll_content_laytout->addSpacing(10);
    scoll_content_laytout->addWidget(model_description);
    scoll_content_laytout->addSpacing(10);
  }
}
ModelConfigContainer::~ModelConfigContainer() {}
} // namespace NAssist