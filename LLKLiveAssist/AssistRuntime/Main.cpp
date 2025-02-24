
#include "GUI/AssistRuntimeWindow.h"
#include "ModuleManager.h"
#include "Runtime/AssistRuntime.h"

#include <QApplication>
#include <QScreen>
#include <filesystem>

#include "ElaApplication.h"
#include "GUI/AssistRuntimeWindow.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif
namespace NAssist {

int Main(int argc, char **argv) {

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
#ifdef LLK_WORK_SPACE
  std::filesystem::current_path(LLK_WORK_SPACE);
#endif
  LLKLogger::instance()->init();
  // init the AssistCore
  AssistRuntime instance;

  QApplication app(argc, argv);
  eApp->init();
  AssistRuntimeWindow w;
  w.show();
  auto exit_code = app.exec();
  ModuleManager::getInstance().shutdown();
  LLKLogger::instance()->shutdown();
  return exit_code;
}
} // namespace NAssist

int main(int argc, char **argv) { return NAssist::Main(argc, argv); }
