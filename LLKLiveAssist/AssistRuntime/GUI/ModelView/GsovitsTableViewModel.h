#pragma once

#include "Data/GSoVITSModel.h"
#include <QAbstractTableModel>
#include <vector>

namespace NAssist {
class GsovitsTableViewModel : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit GsovitsTableViewModel(QObject *parent = nullptr);
  ~GsovitsTableViewModel();
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  void appendData(std::shared_ptr<std::vector<GSoVITSModel>> models);
private:
  QStringList _header;
  QList<QStringList> _dataList;
  QList<QIcon> _iconList;
};
} // namespace NAssist
