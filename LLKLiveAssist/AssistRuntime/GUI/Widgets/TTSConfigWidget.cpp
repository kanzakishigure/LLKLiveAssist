#include "TTSConfigWidget.h"

#include "ElaSlider.h"
#include "GSoVITSAssist.h"
#include "GUI/Theme/LLKStyle.h"
#include <ElaLineEdit.h>
#include <ElaPushButton.h>
#include <ElaScrollPageArea.h>
#include <ElaText.h>
#include <ModuleManager.h>
#include <QDesktopServices.h>
#include <QUrl.h>
#include <QVBoxLayout>
#include <cstdlib>
#include <format>
#include <qchar.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qregexp.h>
#include <qvalidator.h>
#include <string>

namespace NAssist {

TTSConfigWidget::TTSConfigWidget(QWidget *parent) : QWidget(parent) {

  m_tts_config_layout = new QVBoxLayout();

  setLayout(m_tts_config_layout);
  hide();

  m_tts_config_layout->setContentsMargins(30, 0, 0, 0);
  m_tts_config_layout->addStretch();

  ElaText *audio_config_title = new ElaText("TTS Config", this);
  audio_config_title->setTextPointSize(8);

  m_tts_config_layout->setSpacing(10);
  m_tts_config_layout->addWidget(audio_config_title);

  m_infer_data =
      ModuleManager::getInstance().getModule<GSoVITSAssist>()->getInferData();

  auto *top_k_slider = new ElaSlider(this);
  top_k_slider->setRange(0, 100);
  auto *top_k_reg_validator =
      new QRegExpValidator(QRegExp("^(100|[1-9]\\d?)$"));
  auto top_k = LLKStyle::CreateLineEditWithSlider(
      this, "top_k", top_k_reg_validator, top_k_slider, [this](QString text) {
        m_infer_data.top_k = text.toInt();
        ModuleManager::getInstance().getModule<GSoVITSAssist>()->setInferData(
            m_infer_data);
      });

  auto *top_p_slider = new ElaSlider(this);
  top_p_slider->setRange(0, 100);
  auto *top_p_validator =
      new QRegExpValidator(QRegExp(R"(^(0(\.\d{2})?|1(\.00)?)$)"));
  auto top_p = LLKStyle::CreateLineEditWithDoubleSlider(
      this, "top_p", top_p_validator, top_p_slider, [this](QString text) {
        m_infer_data.top_p = text.toDouble();
        ModuleManager::getInstance().getModule<GSoVITSAssist>()->setInferData(
            m_infer_data);
      });

  auto *temperature_validator =
      new QRegExpValidator(QRegExp(R"(^(0(\.\d{2})?|1(\.00)?)$)"));
  auto *temperature_slider = new ElaSlider(this);
  temperature_slider->setRange(0, 100);
  auto temperature = LLKStyle::CreateLineEditWithDoubleSlider(
      this, "temperature", temperature_validator, temperature_slider,
      [this](QString text) {
        m_infer_data.temperature = text.toDouble();
        ModuleManager::getInstance().getModule<GSoVITSAssist>()->setInferData(
            m_infer_data);
      });

  auto *speed_factor_validator = new QDoubleValidator(0, 1.65, 2, this);
  auto *speed_factor_slider = new ElaSlider(this);
  speed_factor_slider->setRange(0, 165);
  auto speed_factor = LLKStyle::CreateLineEditWithDoubleSlider(
      this, "speed_factor", speed_factor_validator, speed_factor_slider,
      [this](QString text) {
        m_infer_data.speed_factor = text.toDouble();
        ModuleManager::getInstance().getModule<GSoVITSAssist>()->setInferData(
            m_infer_data);
      });

  QStringList text_split_method_list = {
      "cut0:不切",           "cut1:凑四句一切",    "cut2:凑50字一切",
      "cut3:按中文句号。切", "cut4:按英文句号.切", "cut5:按标点符号切"};
  auto text_split_method = LLKStyle::CreateComboBox(
      this, "text split method", text_split_method_list, [this](int index) {
        m_infer_data.text_split_method = fmt::format("cut{}", index);
        ModuleManager::getInstance().getModule<GSoVITSAssist>()->setInferData(
            m_infer_data);
      });

  auto index = std::stoi(m_infer_data.text_split_method.substr(3, 1));
  std::get<1>(text_split_method)->setCurrentIndex(index);
  speed_factor_slider->setValue(m_infer_data.speed_factor * 100);
  top_k_slider->setValue(m_infer_data.top_k);
  top_p_slider->setValue(m_infer_data.top_p * 100);
  temperature_slider->setValue(m_infer_data.temperature * 100);
  m_tts_config_layout->addWidget(std::get<0>(text_split_method));
  m_tts_config_layout->addWidget(std::get<0>(speed_factor));
  m_tts_config_layout->addWidget(std::get<0>(top_k));
  m_tts_config_layout->addWidget(std::get<0>(top_p));
  m_tts_config_layout->addWidget(std::get<0>(temperature));
}
} // namespace NAssist