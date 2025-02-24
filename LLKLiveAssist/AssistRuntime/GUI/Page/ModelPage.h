#pragma once

#include "AudioAssist.h"
#include "BasePage.h"
#include "Data/GSoVITSModel.h"
#include "ElaContentDialog.h"
#include "ElaTableView.h"
#include <vector>


class ElaTableView;

namespace NAssist {
class GsovitsTableViewModel;
class ModelPage : public BasePage {
  Q_OBJECT
public:
  Q_INVOKABLE explicit ModelPage(QWidget *parent = nullptr);
  ~ModelPage();

protected:
  virtual void createCustomWidget(QString desText) override;
signals:
  void gSovitsmodelChanged();

private:
  ElaTableView *m_tableView;
  GsovitsTableViewModel *m_gsovits_table_view_model;
  ElaContentDialog *m_model_config_dilog;
};
} // namespace NAssist
