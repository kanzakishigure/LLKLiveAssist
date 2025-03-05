#include "SignalEmitter.h"
#include <qobject.h>

namespace NAssist {

SignalEmitter::SignalEmitter(QObject *parent) : QObject(parent) {}
} // namespace NAssist