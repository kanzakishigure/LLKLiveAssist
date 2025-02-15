
#include "GUI/AssistRuntimeWindow.h"
#include "Runtime/AssistRuntime.h"

#include <QApplication>
#include <QScreen>

#include "ElaApplication.h"
#include "GUI/AssistRuntimeWindow.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif
namespace NAssist {

int Main(int argc, char **argv) {
  // init the AssistCore
  AssistRuntime instance;

  // init the gui
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#else
  // 根据实际屏幕缩放比例更改
  qputenv("QT_SCALE_FACTOR", "1.5");
#endif
#endif

  QApplication app(argc, argv);
  eApp->init();
  AssistRuntimeWindow w;
  w.show();
  auto exit_code = app.exec();

  return exit_code;
}
} // namespace NAssist

int main(int argc, char **argv) { return NAssist::Main(argc, argv); }
