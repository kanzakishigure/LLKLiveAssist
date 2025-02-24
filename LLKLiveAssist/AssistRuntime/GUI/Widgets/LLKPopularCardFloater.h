#pragma once

#include <QWidget>
#include <qboxlayout.h>

#include "Def.h"
#include "ElaScrollArea.h"
#include "ElaSlider.h"
class ElaPushButton;
class QGraphicsOpacityEffect;
namespace NAssist {

class LLKPopularCard;
class LLKPopularCardPrivate;

class LLKPopularCardFloater : public QWidget {
  Q_OBJECT
  Q_PROPERTY_CREATE(qreal, HoverYOffset);
  Q_PROPERTY_CREATE(qreal, HoverOpacity);

public:
  explicit LLKPopularCardFloater(LLKPopularCard *card,
                                 LLKPopularCardPrivate *cardPrivate,
                                 QWidget *parent = nullptr);
  ~LLKPopularCardFloater();
  void showFloater();
  void hideFloater();

protected:
  virtual bool event(QEvent *event) override;
  virtual void paintEvent(QPaintEvent *event) override;

private:
  friend class LLKPopularCard;
  ElaThemeType::ThemeMode _themeMode;
  ElaPushButton *_overButton{nullptr};
  QGraphicsOpacityEffect *_opacityEffect{nullptr};
  bool _isHideAnimationFinished{true};
  LLKPopularCard *_card{nullptr};
  LLKPopularCardPrivate *_cardPrivate{nullptr};
  QRect _calculateTargetGeometry(QRect cardGeometry);
  int _floatGeometryOffset{25};
  QVBoxLayout *floater_layout;
  ElaScrollArea *floater_scroll_area;
};
} // namespace NAssist
