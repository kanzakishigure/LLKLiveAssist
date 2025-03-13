#pragma once

#include <ElaFlowLayout.h>
#include <QWidget>

namespace NAssist {

class SovitsConfigWidget : public QWidget {
  Q_OBJECT
public:
  explicit SovitsConfigWidget(QWidget *parent = nullptr);
  ~SovitsConfigWidget() = default;
  ElaFlowLayout *getModelCardFlowLayout() { return model_card_flow_layout; }

private:
  ElaFlowLayout *model_card_flow_layout{nullptr};
};
} // namespace NAssist