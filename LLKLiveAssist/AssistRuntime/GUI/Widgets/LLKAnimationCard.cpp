#include "LLKAnimationCard.h"
#include "LLKUrlCard.h"
#include <QTimer>
#include <qboxlayout.h>
#include <qlayoutitem.h>
#include <qpropertyanimation.h>
#include <qsize.h>
#include <qvariant.h>
#include <qwidget.h>

namespace NAssist {
#define LLK_PROPERTY_CREATE_LLK_CPP(CLASS, TYPE, M)                            \
  void CLASS::set##M(TYPE M) { m_urlcard->set##M(M); }                         \
  TYPE CLASS::get##M() const { return m_urlcard->get##M(); }

LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, int, BorderRadius)
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, qreal, MainOpacity)
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, qreal, NoiseOpacity)
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, int, BrushAlpha)
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, QString, Title);
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, QString, SubTitle);
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, int, TitlePixelSize);
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, int, SubTitlePixelSize);
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, int, TitleSpacing);
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, int, SubTitleSpacing);
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, QPixmap, CardPixmap);
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, QSize, CardPixmapSize);
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, int, CardPixmapBorderRadius)
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, ElaCardPixType::PixMode,
                            CardPixMode);
LLK_PROPERTY_CREATE_LLK_CPP(LLKAnimationCard, QString, Url);
LLKAnimationCard::LLKAnimationCard(QWidget *parent) : QFrame(parent) {
  m_urlcard = new LLKUrlCard(this);

  init_width = m_urlcard->geometry().width();
  init_height = m_urlcard->geometry().height();

  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->addWidget(m_urlcard);
  setMaximumWidth(0);
  setMinimumWidth(0); // 允许后续调整
}
LLKAnimationCard::~LLKAnimationCard() {}
void LLKAnimationCard::showCard() {
  setVisible(true);

  QTimer::singleShot(0, [this]() {
    QPropertyAnimation *scale_animation =
        new QPropertyAnimation(this, "minimumWidth");
    scale_animation->setDuration(800);
    scale_animation->setStartValue(0);
    scale_animation->setEndValue(init_width);
    scale_animation->setEasingCurve(QEasingCurve::InOutQuad);
    // 动画结束时恢复高度限制，允许自由调整
    connect(scale_animation, &QPropertyAnimation::finished, [this]() {
      setMaximumWidth(QWIDGETSIZE_MAX);
      setMinimumWidth(0); // 允许后续调整
    });
    connect(scale_animation, &QPropertyAnimation::valueChanged, this,
            [=]() { update(); });
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    m_urlcard->showCard(group);
    group->addAnimation(scale_animation);
    group->start(QAbstractAnimation::DeleteWhenStopped);
  });
}

void LLKAnimationCard::hideCard() {

  QTimer::singleShot(0, [this]() {
    QPropertyAnimation *scale_animation =
        new QPropertyAnimation(this, "minimumWidth");
    scale_animation->setDuration(800);
    scale_animation->setStartValue(init_width);
    scale_animation->setEndValue(0);
    scale_animation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(scale_animation, &QPropertyAnimation::finished, [this]() {
      setMaximumWidth(0);
      setMinimumWidth(0); // 允许后续调整
      setVisible(false);
    });
    connect(scale_animation, &QPropertyAnimation::valueChanged, this,
            [=]() { update(); });
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    m_urlcard->hideCard(group);
    group->addAnimation(scale_animation);
    group->start(QAbstractAnimation::DeleteWhenStopped);
  });
}

} // namespace NAssist