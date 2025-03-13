#pragma once

#include <AudioAssist.h>
#include <ElaComboBox.h>
#include <ElaFlowLayout.h>
#include <QWidget>
#include <Data/GSoVITSModel.h>

namespace NAssist {

class TTSConfigWidget : public QWidget {
  Q_OBJECT
public:
  explicit TTSConfigWidget(QWidget *parent = nullptr);
  ~TTSConfigWidget() = default;

  

private:
  ElaComboBox *m_ComboBox{nullptr};
  QVBoxLayout* m_tts_config_layout{nullptr};
  InferData m_infer_data;

};
} // namespace NAssist