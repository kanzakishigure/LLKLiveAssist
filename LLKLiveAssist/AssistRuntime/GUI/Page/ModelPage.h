#pragma once

#include "AudioAssist.h"
#include "BasePage.h"
#include "Data/GSoVITSModel.h"
#include "ElaContentDialog.h"
#include "ElaMenu.h"
#include "ElaTableView.h"
#include "GUI/Widgets/ModelConfigContainer.h"
#include <vector>

namespace NAssist {
class GsovitsTableViewModel;
class GsovitsTableView;
class ModelPage : public BasePage {
  Q_OBJECT
public:
  Q_INVOKABLE explicit ModelPage(QWidget *parent = nullptr);
  ~ModelPage();
signals:
  void gSovitsmodelChanged();

protected:
  virtual void createCustomWidget(QString desText) override;

private:
  ElaMenu *model_menu{nullptr};
  GsovitsTableView *m_tableView{nullptr};
  GsovitsTableViewModel *m_gsovits_table_view_model{nullptr};
  ElaContentDialog *m_model_config_dilog{nullptr};
  ModelConfigContainer *m_model_config_container{nullptr};
};
} // namespace NAssist
