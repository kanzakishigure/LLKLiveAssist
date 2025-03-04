#pragma once

#include "ElaMenu.h"
#include "ElaTableView.h"
#include "GsovitsTableViewModel.h"
#include <cstddef>
namespace NAssist {

class GsovitsTableView : public ElaTableView {
  Q_OBJECT
public:
  explicit GsovitsTableView(QWidget *parent = nullptr);
  ~GsovitsTableView();

  Q_SIGNALS:
    Q_SIGNAL void modelEditorShow(size_t selectIndex);
protected:
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  ElaMenu *model_menu;
};
} // namespace NAssist