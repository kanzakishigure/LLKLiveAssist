#pragma once
#include "BasePage.h"
#include <QWidget>
#include <qobjectdefs.h>

class ElaRadioButton;
class ElaToggleSwitch;
class ElaComboBox;
namespace NAssist {

class SettingPage : public BasePage {

  Q_OBJECT
public:
  Q_INVOKABLE explicit SettingPage(QWidget *parent = nullptr);
  ~SettingPage();

private:
  ElaComboBox *_themeComboBox{nullptr};
  ElaToggleSwitch *_micaSwitchButton{nullptr};
  ElaToggleSwitch *_logSwitchButton{nullptr};
  ElaRadioButton *_minimumButton{nullptr};
  ElaRadioButton *_compactButton{nullptr};
  ElaRadioButton *_maximumButton{nullptr};
  ElaRadioButton *_autoButton{nullptr};
};
} // namespace NAssist