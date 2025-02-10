#include "AssistRuntimeWindow.h"
#include "Panel/ConsoleDilogPanel.h"
#include "Panel/TaskConfigPanel.h"
#include "Panel/TaskPanel.h"
#include "Widget/LLKWidget.h"

#include <QVboxLayout>

namespace NAssist {


    AssistRuntimeWindow::AssistRuntimeWindow(QWidget *parent)
    :QMainWindow(parent)
    {
        setWindowTitle("LLKAssit");


        
        setMinimumSize(960,720);
        m_CentralWidget = new QWidget;
        m_CentralWidget->setStyleSheet(R"(
            QWidget{
            background-color:#1C1C1C
            })");
        setCentralWidget(m_CentralWidget);
        QLayout *layout = new QHBoxLayout;
        centralWidget()->setLayout(layout);
        
        
        m_taskPanel = new TaskPanel(m_CentralWidget);
        layout->addWidget(m_taskPanel);
        m_taskConfigPanel = new TaskConfigPanel(m_CentralWidget);
        layout->addWidget(m_taskConfigPanel);
        m_consoleDilogPanel = new ConsoleDilogPanel(m_CentralWidget);
        layout->addWidget(m_consoleDilogPanel);
        
        //add widget for main windows
        
    }
}