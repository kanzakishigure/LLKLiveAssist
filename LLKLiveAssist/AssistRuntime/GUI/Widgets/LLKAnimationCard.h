#pragma once

#include "LLKUrlCard.h"

#include <QFrame>
namespace NAssist {

class LLKAnimationCard : public QFrame {

  Q_OBJECT

  Q_PROPERTY_CREATE_Q_H(int, BorderRadius)
  Q_PROPERTY_CREATE_Q_H(qreal, MainOpacity)
  Q_PROPERTY_CREATE_Q_H(qreal, NoiseOpacity)
  Q_PROPERTY_CREATE_Q_H(int, BrushAlpha)
  Q_PROPERTY_CREATE_Q_H(QString, Title);
  Q_PROPERTY_CREATE_Q_H(QString, SubTitle);
  Q_PROPERTY_CREATE_Q_H(int, TitlePixelSize);
  Q_PROPERTY_CREATE_Q_H(int, SubTitlePixelSize);
  Q_PROPERTY_CREATE_Q_H(int, TitleSpacing);
  Q_PROPERTY_CREATE_Q_H(int, SubTitleSpacing);
  Q_PROPERTY_CREATE_Q_H(QPixmap, CardPixmap);
  Q_PROPERTY_CREATE_Q_H(QSize, CardPixmapSize);
  Q_PROPERTY_CREATE_Q_H(int, CardPixmapBorderRadius)
  Q_PROPERTY_CREATE_Q_H(ElaCardPixType::PixMode, CardPixMode);
  Q_PROPERTY_CREATE_Q_H(QString, Url);

public:
  explicit LLKAnimationCard(QWidget *parent = nullptr);
  ~LLKAnimationCard();

  void hideCard();
  void showCard();
  

private:
  LLKUrlCard *m_urlcard{nullptr};
  int init_width;
  int init_height;
};
} // namespace NAssist