#pragma once
#include <QPixmap>
#include <QWidget>
#include "ElaSlider.h"
#include "stdafx.h"

namespace NAssist {

class LLKPopularCardPrivate;
class LLKPopularCard : public QWidget
{
    Q_OBJECT
    Q_Q_CREATE(LLKPopularCard)
    Q_PROPERTY_CREATE_Q_H(int, BorderRadius)
    Q_PROPERTY_CREATE_Q_H(QPixmap, CardPixmap)
    Q_PROPERTY_CREATE_Q_H(QString, Title)
    Q_PROPERTY_CREATE_Q_H(QString, SubTitle)
    Q_PROPERTY_CREATE_Q_H(QString, InteractiveTips)
    Q_PROPERTY_CREATE_Q_H(QString, DetailedText)
    Q_PROPERTY_CREATE_Q_H(QString, CardButtontext)
    Q_PROPERTY_CREATE_Q_H(QWidget*, CardFloatArea)
    Q_PROPERTY_CREATE_Q_H(QPixmap, CardFloatPixmap)
    Q_PROPERTY_CREATE_Q_H(ElaSlider*, CardTemperatureSlider)
public:
    explicit LLKPopularCard(QWidget* parent = nullptr);
    ~LLKPopularCard();
Q_SIGNALS:
    Q_SIGNAL void popularCardClicked();
    Q_SIGNAL void popularCardButtonClicked();

protected:
    virtual bool event(QEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
};

} // namespace NAssist