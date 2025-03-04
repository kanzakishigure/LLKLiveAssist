#include "GsovitsTableView.h"
#include "Data/GSoVITSModel.h"
#include "ElaTableView.h"
#include "GSoVITSAssist.h"
#include "GUI/Page/ModelPage.h"
#include "ModuleManager.h"
#include <Core/logger.h>
#include <ElaScrollPage.h>
#include <QMouseEvent>
#include <QString>
#include <algorithm>
#include <cstddef>
#include <qobjectdefs.h>
#include <vector>

namespace NAssist {

GsovitsTableView::GsovitsTableView(QWidget *parent) : ElaTableView(parent) {

  model_menu = new ElaMenu(this);
  model_menu->addSeparator();
  auto *deleta_action =
      model_menu->addElaIconAction(ElaIconType::Ban, "删除模型");
  connect(deleta_action, &QAction::triggered, this, [this,parent]() {
    QItemSelectionModel *selection_model = this->selectionModel();

    if (selection_model) {
      QModelIndexList selected_rows = selection_model->selectedRows();
      std::vector<GSoVITSModel> delete_models;
      auto model_lib = ModuleManager::getInstance()
                             .getModule<GSoVITSAssist>()
                             ->getModelLib();
      for (const QModelIndex &index : selected_rows) {
        int row = index.row();
        // 假设模型有两列，获取数据
        QString name_data =
            this->model()
                ->data(this->model()->index(row, 1), Qt::DisplayRole)
                .toString();

        GUI_INFO("选中数据：{}", name_data.toStdString());

        
        size_t delete_index = index.row();
        if (delete_index >= 0 && delete_index < model_lib->size()) {
          if (model_lib->at(delete_index).model_name ==
              name_data.toStdString()) {
            delete_models.push_back(model_lib->at(delete_index));
          }
        }
      }
      for(const auto& delete_model:delete_models)
      {
        auto it = std::find(model_lib->begin(),model_lib->end(),delete_model);
        GUI_INFO("删除选中数据：{}", it->model_name);
        model_lib->erase(it);
        
      }
      auto* model_page = static_cast<ModelPage*>(parent);
      Q_EMIT model_page->gSovitsmodelChanged();
    }
  });

  auto *edit_action =
      model_menu->addElaIconAction(ElaIconType::BarsStaggered, "编辑模型");
  connect(edit_action, &QAction::triggered, this, [this]() {
    QItemSelectionModel *selection_model = this->selectionModel();
    if (selection_model) {
      QModelIndexList selected_rows = selection_model->selectedRows();
      if (!selected_rows.empty()) {
        auto selectindex = selected_rows.front();
        Q_EMIT modelEditorShow(selectindex.row());
      }
    }
  });
}
GsovitsTableView::~GsovitsTableView() {}
void GsovitsTableView::mouseReleaseEvent(QMouseEvent *event) {
  switch (event->button()) {
  case Qt::RightButton: {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    model_menu->popup(event->globalPosition().toPoint());
#else
    model_menu->popup(event->globalPos());
#endif
    break;
  }
  default: {
    break;
  }
  }
  ElaTableView::mouseReleaseEvent(event);
}
} // namespace NAssist