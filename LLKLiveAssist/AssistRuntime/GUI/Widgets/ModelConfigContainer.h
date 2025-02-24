#pragma once

#include "Data/GSoVITSModel.h"
#include "Def.h"
#include "ElaPlainTextEdit.h"
#include <QWidget>
#include <filesystem>
#include <qchar.h>
#include <qpixmap.h>
#include <qwidget.h>


class ElaImageCard;
namespace NAssist {

class ModelConfigContainer : public QWidget {
  Q_OBJECT
  Q_PROPERTY_CREATE(QString, model_name)
  Q_PROPERTY_CREATE(QString, model_img_path)
  Q_PROPERTY_CREATE(QString, model_author)
  Q_PROPERTY_CREATE(QString, model_category)

  Q_PROPERTY_CREATE(QString, sovits_weights)
  Q_PROPERTY_CREATE(QString, gpt_weights)
  Q_PROPERTY_CREATE(QString, ref_audio_path)
  Q_PROPERTY_CREATE(QString, prompt_text)
  Q_PROPERTY_CREATE(QString, prompt_lang)
  Q_PROPERTY_CREATE(QString, model_description)
public:
  explicit ModelConfigContainer(QWidget *parent = nullptr);
  ~ModelConfigContainer();
signals:
  void valuechanged(QString property_name, QString text);

protected:
private:
  void checkPath(std::filesystem::path path);

private:
  ElaThemeType::ThemeMode _themeMode;
  QString _themeCard;
  ElaImageCard *m_header_area;
};
} // namespace NAssist
