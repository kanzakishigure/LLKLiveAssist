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
  ElaMenu *model_menu;
  GsovitsTableView *m_tableView;
  GsovitsTableViewModel *m_gsovits_table_view_model;
  ElaContentDialog *m_model_config_dilog;
  ModelConfigContainer *m_model_config_container;
};
} // namespace NAssist
