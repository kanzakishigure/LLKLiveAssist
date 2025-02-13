#pragma once

#include "BasePage.h"
#include "Data/GSoVITSModel.h"
#include "AudioAssist.h"
class ElaMenu;
class ElaToggleSwitch;
class ElaPivot;
class ElaComboBox;
namespace NAssist {

class HomePage : public BasePage {
  Q_OBJECT
public:
  Q_INVOKABLE explicit HomePage(QWidget *parent = nullptr);
  ~HomePage();
Q_SIGNALS:
  Q_SIGNAL void elaScreenNavigation();
  Q_SIGNAL void elaBaseComponentNavigation();
  Q_SIGNAL void elaSceneNavigation();
  Q_SIGNAL void elaCardNavigation();
  Q_SIGNAL void elaIconNavigation();

protected:
  virtual void mouseReleaseEvent(QMouseEvent *event);

private:
  ElaMenu *_homeMenu{nullptr};
  ElaPivot* m_pivot{ nullptr };
  ElaComboBox* m_comboBox{ nullptr };
  GSoVITSModel sovits_config_data;
  
  AudioConfig audio_config_data;
};
} // namespace NAssist
