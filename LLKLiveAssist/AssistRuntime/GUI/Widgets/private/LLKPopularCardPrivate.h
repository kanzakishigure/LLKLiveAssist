#pragma once
#include "Def.h"
#include "ElaSlider.h"

#include <QObject>
#include <QPixmap>

namespace NAssist {

class LLKPopularCard;
class LLKPopularCardFloater;
class LLKPopularCardPrivate : public QObject {
  Q_OBJECT
  Q_D_CREATE(LLKPopularCard)
  Q_PROPERTY_CREATE_D(bool, Selected)
  Q_PROPERTY_CREATE_D(int, BorderRadius)
  Q_PROPERTY_CREATE_D(QPixmap, CardPixmap)
  Q_PROPERTY_CREATE_D(QString, Title)
  Q_PROPERTY_CREATE_D(QString, SubTitle)
  Q_PROPERTY_CREATE_D(QString, InteractiveTips)
  Q_PROPERTY_CREATE_D(QString, DetailedText)
  Q_PROPERTY_CREATE_D(QString, CardButtontext)
  Q_PROPERTY_CREATE_D(QWidget *, CardFloatArea)
  Q_PROPERTY_CREATE_D(QPixmap, CardFloatPixmap)
  
  
  Q_PROPERTY_CREATE(qreal, HoverYOffset);
  Q_PROPERTY_CREATE(qreal, HoverOpacity);

public:
  explicit LLKPopularCardPrivate(QObject *parent = nullptr);
  ~LLKPopularCardPrivate();

private:
  friend class LLKPopularCardFloater;
  ElaThemeType::ThemeMode _themeMode;
  QTimer *_floatTimer{nullptr};
  LLKPopularCardFloater *_floater{nullptr};
  bool _isFloating{false};
  int _shadowBorderWidth{6};
  int _textHSpacing{20};
  int _textVSpacing{5};
  QRectF _interactiveTipsBaseRect;
  QRect _buttonTargetRect;

  void _showFloater();
};
} // namespace NAssist
