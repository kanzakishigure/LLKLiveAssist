#pragma once
#include "ElaComboBox.h"
#include "ElaScrollBar.h"
#include "ElaSlider.h"
#include <Def.h>
#include <ElaLineEdit.h>
#include <ElaPlainTextEdit.h>
#include <ElaScrollPageArea.h>
#include <ElaText.h>
#include <QHBoxLayout>
#include <Qcombobox>
#include <cstdint>
#include <qchar.h>
#include <qglobal.h>
#include <qslider.h>
#include <qwidget.h>
#include <tuple>
#include <variant>

namespace NAssist {

namespace LLKStyle {
constexpr uint32_t ScrollPageAreaFixedHeight = 50;
constexpr uint32_t ScrollPageAreaBorderRadius = 3;
constexpr uint32_t LineEditFixedHeight = 23;
constexpr uint32_t LineEditBorderRadius = 6;
constexpr uint32_t ComboBoxFixedHeight = 28;
constexpr uint32_t PushButtonFixedHeight = 28;

constexpr uint32_t FloaterAreaFixedHeight = 40;
template <typename TValidator>
inline QWidget *CreateLineEdit(QWidget *parent, QString property_name,
                               TValidator *validator,
                               std::function<void(QString)> bindfunc) {
  ElaScrollPageArea *property_set_area = new ElaScrollPageArea(parent);
  QHBoxLayout *property_set_area_layout = new QHBoxLayout(property_set_area);
  property_set_area->setFixedHeight(LLKStyle::ScrollPageAreaFixedHeight);
  ElaText *property_text = new ElaText(property_name, parent);
  property_text->setTextPixelSize(14);

  ElaLineEdit *property_edit = new ElaLineEdit(parent);
  property_edit->setFixedWidth(200);
  property_edit->setFixedHeight(LLKStyle::LineEditFixedHeight);
  property_edit->setBorderRadius(LLKStyle::LineEditBorderRadius);
  property_edit->setValidator(validator);
  property_set_area_layout->addWidget(property_text);
  property_set_area_layout->addSpacing(10);
  property_set_area_layout->addStretch();
  property_set_area_layout->addWidget(property_edit);
  parent->connect(property_edit, &ElaLineEdit::focusOut,
                  [bindfunc](QString text) { bindfunc(text); });

  return property_set_area;
}
inline std::tuple<QWidget *,ElaLineEdit*>
CreateLineEditWithSlider(QWidget *parent, QString property_name,
                         QValidator *validator, QSlider *slider,
                         std::function<void(QString)> bindfunc) {
  ElaScrollPageArea *property_set_area = new ElaScrollPageArea(parent);
  QHBoxLayout *property_set_area_layout = new QHBoxLayout(property_set_area);
  property_set_area->setFixedHeight(LLKStyle::ScrollPageAreaFixedHeight);
  ElaText *property_text = new ElaText(property_name, parent);
  property_text->setTextPixelSize(14);

  ElaLineEdit *property_edit = new ElaLineEdit(parent);
  property_edit->setFixedWidth(80);
  property_edit->setFixedHeight(LLKStyle::LineEditFixedHeight);
  property_edit->setBorderRadius(LLKStyle::LineEditBorderRadius);
  property_edit->setValidator(validator);

  slider->setFixedWidth(260);
  property_set_area_layout->addWidget(property_text);
  property_set_area_layout->addStretch();
  property_set_area_layout->addWidget(slider);
  property_set_area_layout->addSpacing(10);
  property_set_area_layout->addWidget(property_edit);

  parent->connect(property_edit, &ElaLineEdit::focusOut,
                  [bindfunc, slider](QString text) {
                    slider->setValue(text.toInt());
                    bindfunc(text);
                  });
  parent->connect(slider, &ElaSlider::valueChanged,
                  [slider, property_edit,bindfunc](int value) {
                    property_edit->setText(QString::number(value));
                    bindfunc(QString::number(value));
                  });
  return {property_set_area,property_edit};
}
inline std::tuple<QWidget *,ElaLineEdit*>
CreateLineEditWithDoubleSlider(QWidget *parent, QString property_name,
                               QValidator *validator, QSlider *slider,
                               std::function<void(QString)> bindfunc) {
  ElaScrollPageArea *property_set_area = new ElaScrollPageArea(parent);
  QHBoxLayout *property_set_area_layout = new QHBoxLayout(property_set_area);
  property_set_area->setFixedHeight(LLKStyle::ScrollPageAreaFixedHeight);
  ElaText *property_text = new ElaText(property_name, parent);
  property_text->setTextPixelSize(14);

  ElaLineEdit *property_edit = new ElaLineEdit(parent);
  property_edit->setFixedWidth(80);
  property_edit->setFixedHeight(LLKStyle::LineEditFixedHeight);
  property_edit->setBorderRadius(LLKStyle::LineEditBorderRadius);
  property_edit->setValidator(validator);

  slider->setFixedWidth(260);
  property_set_area_layout->addWidget(property_text);
  property_set_area_layout->addStretch();
  property_set_area_layout->addWidget(slider);
  property_set_area_layout->addSpacing(10);
  property_set_area_layout->addWidget(property_edit);

  parent->connect(property_edit, &ElaLineEdit::focusOut,
                  [slider, bindfunc](QString text) {
                    auto value = text.toDouble() * 100;
                    slider->setValue(value);
                    bindfunc(text);
                  });
  parent->connect(slider, &ElaSlider::valueChanged,
                  [slider, property_edit,bindfunc](int value) {
                    double percentage = value / 100.0f;
                    property_edit->setText(QString::number(percentage, 'f', 2));
                    bindfunc(QString::number(percentage, 'f', 2));
                  });
  return {property_set_area,property_edit};
}
inline std::tuple<QWidget *,ElaPlainTextEdit*>
CreatePlainTextEdit(QWidget *parent, QString property_name,
                    std::function<void(ElaPlainTextEdit *)> bindfunc) {
  ElaScrollPageArea *property_set_area = new ElaScrollPageArea(parent);
  QHBoxLayout *property_set_area_layout = new QHBoxLayout(property_set_area);
  property_set_area->setFixedHeight(LLKStyle::ScrollPageAreaFixedHeight);
  ElaText *property_text = new ElaText(property_name, parent);
  property_text->setTextPixelSize(14);

  ElaPlainTextEdit *property_edit = new ElaPlainTextEdit(parent);
  property_edit->setFixedWidth(200);
  property_edit->setFixedHeight(LLKStyle::LineEditFixedHeight);

  property_set_area_layout->addWidget(property_text);
  property_set_area_layout->addWidget(property_edit);
  parent->connect(property_edit, &ElaPlainTextEdit::textChanged,
                  [bindfunc, property_edit]() { bindfunc(property_edit); });

  return {property_set_area,property_edit};
}
inline std::tuple<QWidget *,ElaComboBox*>
CreateComboBox(QWidget *parent, QString property_name,
                               QStringList comboList,
                               std::function<void(int)> bindfunc) {
  ElaScrollPageArea *property_set_area = new ElaScrollPageArea(parent);
  QHBoxLayout *property_set_area_layout = new QHBoxLayout(property_set_area);
  property_set_area->setFixedHeight(LLKStyle::ScrollPageAreaFixedHeight);
  ElaText *property_text = new ElaText(property_name, parent);
  property_text->setTextPixelSize(14);

  ElaComboBox *property_combobox = new ElaComboBox(parent);
  property_combobox->addItems(comboList);
  property_combobox->setFixedWidth(200);
  property_combobox->setFixedHeight(LLKStyle::ComboBoxFixedHeight);

  property_set_area_layout->addWidget(property_text);
  property_set_area_layout->addStretch();
  property_set_area_layout->addWidget(property_combobox);

  parent->connect(property_combobox,
                  QOverload<int>::of(&ElaComboBox::currentIndexChanged),
                  [bindfunc](int index) { bindfunc(index); });

  return {property_set_area,property_combobox};
}
}; // namespace LLKStyle

} // namespace NAssist