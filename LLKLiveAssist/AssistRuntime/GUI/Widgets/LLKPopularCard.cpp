#include "LLKPopularCard.h"
#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>

#include "ElaPushButton.h"

#include "ElaTheme.h"
#include "GUI/Widgets//private/LLKPopularCardPrivate.h"
#include "LLKPopularCardFloater.h"
#include <qcolor.h>
#include <qglobal.h>

namespace NAssist {
Q_PROPERTY_CREATE_Q_CPP(LLKPopularCard, int, BorderRadius)
Q_PROPERTY_CREATE_Q_CPP(LLKPopularCard, QPixmap, CardPixmap)
Q_PROPERTY_CREATE_Q_CPP(LLKPopularCard, QString, Title)
Q_PROPERTY_CREATE_Q_CPP(LLKPopularCard, QString, SubTitle)
Q_PROPERTY_CREATE_Q_CPP(LLKPopularCard, QString, InteractiveTips)
Q_PROPERTY_CREATE_Q_CPP(LLKPopularCard, QString, DetailedText)
Q_PROPERTY_CREATE_Q_CPP(LLKPopularCard, QPixmap, CardFloatPixmap)

LLKPopularCard::LLKPopularCard(QWidget *parent)
    : QWidget{parent}, d_ptr(new LLKPopularCardPrivate()) {
  Q_D(LLKPopularCard);
  setFixedSize(320, 120);
  d->q_ptr = this;
  d->_pBorderRadius = 8;
  d->_pHoverYOffset = 0;
  d->_pHoverOpacity = 0;
  d->_pSelected = false;
  setObjectName("LLKPopularCard");
  setStyleSheet("#LLKPopularCard{background-color:transparent}");
  setMouseTracking(true);

  d->_pCardFloatArea = parentWidget();
  d->_floater = new LLKPopularCardFloater(this, d, d->_pCardFloatArea);
  d->_floatTimer = new QTimer(this);
  connect(d->_floatTimer, &QTimer::timeout, d,
          &LLKPopularCardPrivate::_showFloater);

  d->_themeMode = eTheme->getThemeMode();
  connect(
      eTheme, &ElaTheme::themeModeChanged, this,
      [=](ElaThemeType::ThemeMode themeMode) { d->_themeMode = themeMode; });
}
LLKPopularCard::~LLKPopularCard() {}

void LLKPopularCard::setCardButtontext(QString cardButtonText) {
  Q_D(LLKPopularCard);
  if (cardButtonText.isEmpty()) {
    return;
  }
  d->_pCardButtontext = cardButtonText;
  d->_floater->_overButton->setText(d->_pCardButtontext);
  Q_EMIT pCardButtontextChanged();
}

QString LLKPopularCard::getCardButtontext() const {
  Q_D(const LLKPopularCard);
  return d->_pCardButtontext;
}
int LLKPopularCard::getCardSliderValue() const {
  Q_D(const LLKPopularCard);
  return d->_floater->_volumeSlider->value();
}
void LLKPopularCard::setCardSliderValue(int value) {
  Q_D(LLKPopularCard);
  d->_floater->_volumeSlider->setValue(value);
  update();
  d->_floater->update();
  Q_EMIT pCardSliderValueChanged();
}
void LLKPopularCard::setSelected(bool selected) {
  Q_D(LLKPopularCard);
  d->_pSelected = selected;
  d->_floater->_overButton->setDisabled(selected);
  update();
  d->_floater->update();
  Q_EMIT pSelectedChanged();
}

bool LLKPopularCard::getSelected() const {
  Q_D(const LLKPopularCard);
  return d->_pSelected;
}
void LLKPopularCard::setCardFloatArea(QWidget *floatArea) {
  Q_D(LLKPopularCard);
  if (!floatArea || floatArea == this) {
    return;
  }
  d->_pCardFloatArea = floatArea;
  d->_floater->setParent(floatArea);
  Q_EMIT pCardFloatAreaChanged();
}

QWidget *LLKPopularCard::getCardFloatArea() const {
  Q_D(const LLKPopularCard);
  return d->_pCardFloatArea;
}

bool LLKPopularCard::event(QEvent *event) {
  Q_D(LLKPopularCard);
  switch (event->type()) {
  case QEvent::Enter: {
    d->_floatTimer->start(450);
    QPropertyAnimation *hoverAnimation =
        new QPropertyAnimation(d, "pHoverYOffset");
    connect(hoverAnimation, &QPropertyAnimation::valueChanged, this,
            [=]() { update(); });
    hoverAnimation->setDuration(130);
    hoverAnimation->setStartValue(d->_pHoverYOffset);
    hoverAnimation->setEndValue(6);
    hoverAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QPropertyAnimation *opacityAnimation =
        new QPropertyAnimation(d, "pHoverOpacity");
    opacityAnimation->setDuration(130);
    opacityAnimation->setStartValue(d->_pHoverOpacity);
    opacityAnimation->setEndValue(1);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    break;
  }
  case QEvent::Leave: {
    d->_floatTimer->stop();
    QPropertyAnimation *hoverAnimation =
        new QPropertyAnimation(d, "pHoverYOffset");
    connect(hoverAnimation, &QPropertyAnimation::valueChanged, this,
            [=]() { update(); });
    hoverAnimation->setDuration(130);
    hoverAnimation->setStartValue(d->_pHoverYOffset);
    hoverAnimation->setEndValue(0);
    hoverAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QPropertyAnimation *opacityAnimation =
        new QPropertyAnimation(d, "pHoverOpacity");
    opacityAnimation->setDuration(130);
    opacityAnimation->setStartValue(d->_pHoverOpacity);
    opacityAnimation->setEndValue(0);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    break;
  }
  case QEvent::MouseButtonRelease: {
    Q_EMIT popularCardClicked();
    break;
  }
  default: {
    break;
  }
  }
  return QWidget::event(event);
}

void LLKPopularCard::paintEvent(QPaintEvent *event) {
  Q_D(LLKPopularCard);
  if (d->_isFloating) {
    return;
  }
  QPainter painter(this);
  painter.save();
  painter.setRenderHints(QPainter::SmoothPixmapTransform |
                         QPainter::Antialiasing | QPainter::TextAntialiasing);
  if (underMouse()) {
    // 阴影绘制
    painter.setOpacity(d->_pHoverOpacity);
    QRect shadowRect = rect();
    shadowRect.adjust(0, 0, 0, -d->_pHoverYOffset);
    eTheme->drawEffectShadow(&painter, shadowRect, d->_shadowBorderWidth,
                             d->_pBorderRadius);
  }
  QRectF foregroundRect(d->_shadowBorderWidth,
                        d->_shadowBorderWidth - d->_pHoverYOffset + 1,
                        width() - 2 * d->_shadowBorderWidth,
                        height() - 2 * d->_shadowBorderWidth);
  // 背景绘制
  painter.setOpacity(1);
  painter.setPen(underMouse() ? ElaThemeColor(d->_themeMode, PopupBorderHover)
                              : ElaThemeColor(d->_themeMode, BasicBorder));
  if (getSelected()) {

    auto back_corlor = d->_themeMode == ElaThemeType::ThemeMode::Dark
                           ? QColor(196, 215, 214, 160)
                           : QColor(223, 236, 213, 160);
    painter.setBrush(back_corlor);

    painter.drawRoundedRect(foregroundRect, d->_pBorderRadius,
                            d->_pBorderRadius);

  } else {

    painter.setBrush(ElaThemeColor(d->_themeMode, BasicBaseAlpha));

    painter.drawRoundedRect(foregroundRect, d->_pBorderRadius,
                            d->_pBorderRadius);
  }

  // 图片绘制
  painter.save();
  QRectF pixRect(foregroundRect.x() + foregroundRect.height() * 0.15,
                 foregroundRect.y() + foregroundRect.height() * 0.15,
                 foregroundRect.height() * 0.7, foregroundRect.height() * 0.7);
  QPainterPath pixPath;
  pixPath.addRoundedRect(pixRect, 4, 4);
  painter.setClipPath(pixPath);
  painter.drawPixmap(pixRect, d->_pCardPixmap, d->_pCardPixmap.rect());
  painter.restore();

  // 文字绘制
  // 计算按钮最终矩形
  int buttonTargetWidth =
      d->_floater->_overButton->fontMetrics().horizontalAdvance(
          d->_floater->_overButton->text()) +
      40;
  if (buttonTargetWidth > width() * 0.25) {
    buttonTargetWidth = width() * 0.25;
  }
  // 处理ElaPushButton的阴影偏移
  d->_buttonTargetRect =
      QRect(QPoint(width() + 2 * d->_floater->_floatGeometryOffset -
                       d->_shadowBorderWidth + 3 -
                       foregroundRect.height() * 0.15 - buttonTargetWidth,
                   foregroundRect.height() * 0.15 - 3),
            QSize(buttonTargetWidth, 36));

  // Title
  painter.setPen(ElaThemeColor(d->_themeMode, BasicText));
  QFont font = painter.font();
  font.setWeight(QFont::Bold);
  font.setPixelSize(15);
  painter.setFont(font);
  int titleHeight = painter.fontMetrics().height();
  QRectF titleRect(pixRect.right() + d->_textHSpacing, pixRect.y(),
                   d->_floater->_floatGeometryOffset * 2 +
                       foregroundRect.width() - pixRect.width() -
                       d->_textHSpacing * 2 - foregroundRect.height() * 0.15 -
                       buttonTargetWidth,
                   titleHeight);
  QString titleText = painter.fontMetrics().elidedText(
      d->_pTitle, Qt::ElideRight, titleRect.width());
  painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine,
                   titleText);

  // SubTitle
  font.setWeight(QFont::DemiBold);
  font.setPixelSize(13);
  painter.setFont(font);
  int subTitleHeight = painter.fontMetrics().height();
  QRectF subTitleRect(
      pixRect.right() + d->_textHSpacing, titleRect.bottom() + d->_textVSpacing,
      d->_floater->_floatGeometryOffset * 2 + foregroundRect.width() -
          pixRect.width() - d->_textHSpacing * 2 -
          foregroundRect.height() * 0.15 - buttonTargetWidth,
      subTitleHeight);
  QString subTitleText = painter.fontMetrics().elidedText(
      d->_pSubTitle, Qt::ElideRight, subTitleRect.width());
  painter.drawText(subTitleRect,
                   Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine,
                   subTitleText);

  // InteractiveTips
  int tipWidth = painter.fontMetrics().horizontalAdvance(d->_pInteractiveTips);
  int tipHeight = painter.fontMetrics().height();
  if (!d->_pInteractiveTips.isEmpty()) {
    font.setWeight(QFont::DemiBold);
    font.setPixelSize(12);
    painter.setFont(font);
    // 覆盖背景绘制
    QRectF tipRect(foregroundRect.right() - d->_textHSpacing - tipWidth,
                   foregroundRect.bottom() - d->_textHSpacing - tipHeight,
                   foregroundRect.width() / 2 - d->_textHSpacing, tipHeight);
    painter.setPen(Qt::NoPen);
    painter.setBrush(ElaThemeColor(d->_themeMode, BasicBaseDeep));
    QRectF baseRect = tipRect;
    baseRect.setRight(tipRect.x() + tipWidth);
    baseRect.adjust(-7, -3, 4, 3);
    d->_interactiveTipsBaseRect = baseRect;
    painter.drawRoundedRect(baseRect, 6, 6);
    // 文字绘制
    painter.setPen(ElaThemeColor(d->_themeMode, BasicText));
    painter.drawText(tipRect,
                     Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
                     d->_pInteractiveTips);
  } else {
    QRectF tipRect(foregroundRect.right() - d->_textHSpacing - 50,
                   foregroundRect.bottom() - d->_textHSpacing - tipHeight,
                   foregroundRect.width() / 2 - d->_textHSpacing, tipHeight);
    tipRect.setRight(tipRect.x() + tipWidth);
    tipRect.adjust(-7, -3, 4, 3);
    d->_interactiveTipsBaseRect = tipRect;
  }
  painter.restore();
}

} // namespace NAssist