#include "LLKPopularCardPrivate.h"

#include <QTimer>

#include "GUI/Widgets/LLKPopularCard.h"
#include "GUI/Widgets/LLKPopularCardFloater.h"
namespace NAssist {
LLKPopularCardPrivate::LLKPopularCardPrivate(QObject *parent)
    : QObject{parent} {}

LLKPopularCardPrivate::~LLKPopularCardPrivate() {}

void LLKPopularCardPrivate::_showFloater() {
  Q_Q(LLKPopularCard);
  q->update();
  _isFloating = true;
  _pHoverYOffset = 0;
  _floatTimer->stop();
  _floater->showFloater();
}

} // namespace NAssist