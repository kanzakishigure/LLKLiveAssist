#pragma once 


#include <QMainWindow>
#include <qwidget.h>
#include "Panel/TaskPanel.h"
#include "Panel/ConsoleDilogPanel.h"
#include "Panel/TaskConfigPanel.h"
namespace NAssist {

    class AssistRuntimeWindow : public QMainWindow
    {
        TaskPanel* m_taskPanel;
        TaskConfigPanel* m_taskConfigPanel;
        ConsoleDilogPanel* m_consoleDilogPanel;

        QWidget* m_CentralWidget;
    public:
        explicit AssistRuntimeWindow(QWidget *parent = nullptr);

   

    };
}