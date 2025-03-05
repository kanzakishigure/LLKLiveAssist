#include "LLKUrlCard.h"
#include "ElaAcrylicUrlCard.h"
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QTimer>
#include <qpropertyanimation.h>
#include <qtimer.h>
#include <qvariant.h>

namespace NAssist {
LLKUrlCard::LLKUrlCard(QWidget *parent) : ElaAcrylicUrlCard(parent) {

  opacityEffect = new QGraphicsOpacityEffect(this);
  opacityEffect->setOpacity(1);
  
  setGraphicsEffect(opacityEffect);
}
LLKUrlCard::~LLKUrlCard() {}

void LLKUrlCard::showCard() {

  setVisible(true);

  HoverOpacity = 1;
  opacityEffect->setOpacity(1);

  QTimer::singleShot(0,this, [this]() {
    QPropertyAnimation *geometry_animation =
        new QPropertyAnimation(this, "geometry");
    connect(geometry_animation, &QPropertyAnimation::valueChanged, this,
            [=]() { update(); });

    geometry_animation->setEasingCurve(QEasingCurve::OutQuad);
    geometry_animation->setDuration(800);

    QRect card_geometry = QRect(this->mapToParent(QPoint(0, 0)), this->size());
    QRect end_geometry = QRect(card_geometry);
    QRect start_geometry =
        QRect(end_geometry.x() - floatGeometryOffset,
              end_geometry.y() - floatGeometryOffset, 0, this->height());

    geometry_animation->setStartValue(start_geometry);
    geometry_animation->setEndValue(end_geometry);

    QPropertyAnimation *opacity_animation =
        new QPropertyAnimation(opacityEffect, "opacity");
    opacity_animation->setDuration(800);   // 动画时长 0.8 秒
    opacity_animation->setStartValue(0.0); // 完全透明
    opacity_animation->setEndValue(1.0);   // 完全不透明
    opacity_animation->setEasingCurve(QEasingCurve::InOutQuad);

    QParallelAnimationGroup *parallel_group = new QParallelAnimationGroup;
    parallel_group->addAnimation(geometry_animation);

    parallel_group->addAnimation(opacity_animation);
    parallel_group->start(QAbstractAnimation::DeleteWhenStopped);
  });
}
void LLKUrlCard::hideCard() {

  QTimer::singleShot(0, this,[this]() {
    QPropertyAnimation *geometry_animation =
        new QPropertyAnimation(this, "geometry");
    connect(geometry_animation, &QPropertyAnimation::valueChanged, this,
            [=]() { update(); });

    connect(geometry_animation, &QPropertyAnimation::finished, this, [=]() {
      setVisible(false);
      update();
    });
    geometry_animation->setEasingCurve(QEasingCurve::OutQuad);
    geometry_animation->setDuration(800);
    QRect card_geometry = QRect(this->mapToParent(QPoint(0, 0)), this->size());
    QRect start_geometry = QRect(card_geometry);
    QRect end_geometry =
        QRect(start_geometry.x() - floatGeometryOffset,
              start_geometry.y() - floatGeometryOffset, 0, this->height());
    geometry_animation->setStartValue(start_geometry);
    geometry_animation->setEndValue(end_geometry);

    QPropertyAnimation *opacity_animation =
        new QPropertyAnimation(opacityEffect, "opacity");
    opacity_animation->setDuration(800);   // 动画时长 0.8 秒
    opacity_animation->setStartValue(1.0); // 完全不透明
    opacity_animation->setEndValue(0.0);   // 完全透明
    opacity_animation->setEasingCurve(QEasingCurve::InOutQuad);

    QParallelAnimationGroup *parallel_group = new QParallelAnimationGroup;
    parallel_group->addAnimation(geometry_animation);
    parallel_group->addAnimation(opacity_animation);
    parallel_group->start(QAbstractAnimation::DeleteWhenStopped);
  });
}
void LLKUrlCard::showCard(QParallelAnimationGroup *group) {

  setVisible(true);

  HoverOpacity = 1;
  opacityEffect->setOpacity(1);
  QPropertyAnimation *geometry_animation =
      new QPropertyAnimation(this, "geometry");
  connect(geometry_animation, &QPropertyAnimation::valueChanged, this,
          [=]() { update(); });

  geometry_animation->setEasingCurve(QEasingCurve::OutQuad);
  geometry_animation->setDuration(800);

  QRect card_geometry = QRect(this->mapToParent(QPoint(0, 0)), this->size());
  QRect end_geometry = QRect(card_geometry);
  QRect start_geometry =
      QRect(end_geometry.x() - floatGeometryOffset,
            end_geometry.y() - floatGeometryOffset, 0, this->height());

  geometry_animation->setStartValue(start_geometry);
  geometry_animation->setEndValue(end_geometry);

  QPropertyAnimation *opacity_animation =
      new QPropertyAnimation(opacityEffect, "opacity");
  opacity_animation->setDuration(800);   // 动画时长 0.8 秒
  opacity_animation->setStartValue(0.0); // 完全透明
  opacity_animation->setEndValue(1.0);   // 完全不透明
  opacity_animation->setEasingCurve(QEasingCurve::InOutQuad);

  group->addAnimation(geometry_animation);

  group->addAnimation(opacity_animation);
}
void LLKUrlCard::hideCard(QParallelAnimationGroup *group) {

  QPropertyAnimation *geometry_animation =
      new QPropertyAnimation(this, "geometry");
  connect(geometry_animation, &QPropertyAnimation::valueChanged, this,
          [=]() { update(); });

  connect(geometry_animation, &QPropertyAnimation::finished, this, [=]() {
    setVisible(false);
    update();
  });
  geometry_animation->setEasingCurve(QEasingCurve::OutQuad);
  geometry_animation->setDuration(800);
  QRect card_geometry = QRect(this->mapToParent(QPoint(0, 0)), this->size());
  QRect start_geometry = QRect(card_geometry);
  QRect end_geometry =
      QRect(start_geometry.x() - floatGeometryOffset,
            start_geometry.y() - floatGeometryOffset, 0, this->height());
  geometry_animation->setStartValue(start_geometry);
  geometry_animation->setEndValue(end_geometry);

  QPropertyAnimation *opacity_animation =
      new QPropertyAnimation(opacityEffect, "opacity");
  opacity_animation->setDuration(800);   // 动画时长 0.8 秒
  opacity_animation->setStartValue(1.0); // 完全不透明
  opacity_animation->setEndValue(0.0);   // 完全透明
  opacity_animation->setEasingCurve(QEasingCurve::InOutQuad);

  group->addAnimation(geometry_animation);
  group->addAnimation(opacity_animation);
}
} // namespace NAssist