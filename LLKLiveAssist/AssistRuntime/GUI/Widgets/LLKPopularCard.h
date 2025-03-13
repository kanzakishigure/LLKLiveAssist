#pragma once
#include "ElaSlider.h"
#include "stdafx.h"
#include <QPixmap>
#include <QWidget>


namespace NAssist {

class LLKPopularCardPrivate;
class LLKPopularCard : public QWidget {
  Q_OBJECT
  Q_Q_CREATE(LLKPopularCard)
  Q_PROPERTY_CREATE_Q_H(bool, Selected)
  Q_PROPERTY_CREATE_Q_H(int, BorderRadius)
  Q_PROPERTY_CREATE_Q_H(QPixmap, CardPixmap)
  Q_PROPERTY_CREATE_Q_H(QString, Title)
  Q_PROPERTY_CREATE_Q_H(QString, SubTitle)
  Q_PROPERTY_CREATE_Q_H(QString, InteractiveTips)
  Q_PROPERTY_CREATE_Q_H(QString, DetailedText)
  Q_PROPERTY_CREATE_Q_H(QString, CardButtontext)
  Q_PROPERTY_CREATE_Q_H(int, CardSliderValue)
  
  Q_PROPERTY_CREATE_Q_H(QWidget *, CardFloatArea)
  Q_PROPERTY_CREATE_Q_H(QPixmap, CardFloatPixmap)
  
public:
  explicit LLKPopularCard(QWidget *parent = nullptr);
  ~LLKPopularCard();
Q_SIGNALS:
  Q_SIGNAL void popularCardClicked();
  Q_SIGNAL void popularCardButtonClicked();
  Q_SIGNAL void popularCardTestButtonClicked();
  Q_SIGNAL void popularCardVolumeSliderValueChanged(int value);
protected:
  virtual bool event(QEvent *event) override;
  virtual void paintEvent(QPaintEvent *event) override;
};

} // namespace NAssist