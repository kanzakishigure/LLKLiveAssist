#pragma once

#include "ElaWindow.h"

#include <QMainWindow>

class ElaContentDialog;

namespace NAssist {
class HomePage;
class AssistRuntimeWindow : public ElaWindow {

  Q_OBJECT

public:
  explicit AssistRuntimeWindow(QWidget *parent = nullptr);
  void initWindow();
  void initEdgeLayout();
  void initContent();

private:
  // ela widget
  ElaContentDialog *_closeDialog{nullptr};
  HomePage *m_homePage{nullptr};

  QWidget *m_CentralWidget;
};
} // namespace NAssist