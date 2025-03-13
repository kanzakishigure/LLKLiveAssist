#pragma once

#include "BasePage.h"
#include "Data/GSoVITSModel.h"

#include "GUI/Widgets//TTSConfigWidget.h"
#include "GUI/Widgets/BiliConfigWidget.h"
#include "GUI/Widgets/SovitsConfigWidget.h"
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
  flushModelCard(std::shared_ptr<std::vector<GSoVITSModel>> sovits_models);
  void onGSoVITSModelChanged();

protected:
  virtual void mouseReleaseEvent(QMouseEvent *event);

private:
  ElaMenu *m_homeMenu{nullptr};
  ElaPivot *m_pivot{nullptr};

  SovitsConfigWidget *m_SovitsConfigWidget{nullptr};
  BiliConfigWidget *m_BiliConfigWidget{nullptr};
  TTSConfigWidget *m_TTSConfigWidget{nullptr};
};
} // namespace NAssist
