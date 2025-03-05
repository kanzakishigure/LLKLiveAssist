#pragma once

#include "ElaAcrylicUrlCard.h"
#include <qwidget.h>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
namespace NAssist {

class LLKUrlCard : public ElaAcrylicUrlCard {
  Q_OBJECT
  
public:
  explicit LLKUrlCard(QWidget *parent = nullptr);
  ~LLKUrlCard();
  Q_INVOKABLE  void showCard();
  Q_INVOKABLE  void hideCard();
  void hideCard(QParallelAnimationGroup* group);
  void showCard(QParallelAnimationGroup* group);

private:

qreal HoverOpacity;
QGraphicsOpacityEffect* opacityEffect{nullptr};
int floatGeometryOffset{25};


};
} // namespace NAssist