#pragma once

#include "AudioAssist.h"
#include "BasePage.h"
#include "Data/GSoVITSModel.h"
#include "ElaFlowLayout.h"
#include <cstddef>
#include <vector>

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
  Q_SIGNAL void llkModelNavigation();

public:
  Q_INVOKABLE void
  flushModelCard(const std::vector<GSoVITSModel> &sovits_models);
  void onGSoVITSModelChanged();

protected:
  virtual void mouseReleaseEvent(QMouseEvent *event);

private:
  ElaMenu *_homeMenu{nullptr};
  ElaPivot *m_pivot{nullptr};
  ElaComboBox *m_comboBox{nullptr};
  ElaFlowLayout *model_card_flow_layout{nullptr};

  std::vector<GSoVITSModel> m_sovits_models;
  AudioConfig m_audio_config_data;
};
} // namespace NAssist
