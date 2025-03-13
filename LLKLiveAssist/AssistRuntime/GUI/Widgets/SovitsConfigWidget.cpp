#include "SovitsConfigWidget.h"

#include <ElaText.h>
#include <QVBoxLayout>
#include <qwidget.h>

namespace NAssist {

SovitsConfigWidget::SovitsConfigWidget(QWidget *parent) : QWidget(parent) {

  QVBoxLayout *sovits_config_layout = new QVBoxLayout();
  this->setLayout(sovits_config_layout);
  this->hide();

  sovits_config_layout->setContentsMargins(30, 0, 0, 0);
  sovits_config_layout->addStretch();

  ElaText *sovits_config_title = new ElaText("GPT-Sovits config", this);
  sovits_config_title->setTextPointSize(8);

  sovits_config_layout->setSpacing(10);
  sovits_config_layout->addWidget(sovits_config_title);

  QWidget *model_card_flow_area = new QWidget(this);
  model_card_flow_layout = new ElaFlowLayout(model_card_flow_area);

  sovits_config_layout->addWidget(model_card_flow_area);
}
} // namespace NAssist