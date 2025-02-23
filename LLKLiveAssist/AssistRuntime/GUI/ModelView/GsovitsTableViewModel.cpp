#include "GsovitsTableViewModel.h"
#include "Data/GSoVITSModel.h"
#include "Def.h"
#include "ElaIcon.h"
#include "ElaIconButton.h"
#include <QIcon>
#include <qnamespace.h>
#include <vector>
#include <Core/logger.h>
namespace NAssist {
GsovitsTableViewModel::GsovitsTableViewModel(QObject *parent) : QAbstractTableModel{parent} {
  _header << "预览" << "Name" << "Author" << "分类" << "描述";
  
}

GsovitsTableViewModel::~GsovitsTableViewModel() {}

int GsovitsTableViewModel::rowCount(const QModelIndex &parent) const { return _dataList.count(); }

int GsovitsTableViewModel::columnCount(const QModelIndex &parent) const {
  return _header.count();
}

QVariant GsovitsTableViewModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole && index.column() != 0) {
    return _dataList[index.row()][index.column() - 1];
  } else if (role == Qt::DecorationRole && index.column() == 0) {
    return _iconList[index.row()];
    
  } else if (role == Qt::DecorationPropertyRole) {
    return Qt::AlignCenter;
  } 
  return QVariant();
}

QVariant GsovitsTableViewModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return _header[section];
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

void GsovitsTableViewModel::appendData(const std::vector<GSoVITSModel> &models) {

  beginResetModel();
  _dataList.clear();
  _iconList.clear();
  for (const auto &model : models) {
    QStringList data;
    data << model.model_name.c_str();
    data << model.model_author.c_str();
    data << model.model_category.c_str();
    data << model.model_description.c_str();
    _dataList.append(data);
    _iconList.append(QIcon(
        QPixmap(model.model_img.c_str())
        .copy()
        .scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  }
  GUI_INFO("_iconList size : {}",_iconList.count());
  endResetModel();
}
} // namespace NAssist