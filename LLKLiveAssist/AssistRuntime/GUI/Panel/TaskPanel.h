#pragma once 

#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QGridLayout>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qobject.h>
#include <QGroupBox>
#include <qwidget.h>

#include "GUI/Widget/LLKWidget.h"


namespace NAssist {

    class TaskPanel : public QGroupBox
    {
        Q_OBJECT  
        QLayout* m_layout;
        QGroupBox* m_taskGroupBox; 
        PushButton* m_taskControlButton;
    public:
        explicit TaskPanel(QWidget* Parent);
        
    private Q_SLOTS:
        void on_TaskStart(); //start all task
        void on_TaskStop(); //start all task

        void on_BiliConfig_select(); //set biliconfig_data, emit signal
        void on_SovitsConfig_select(); //set SovitsConfig data, emit signal
        void on_AudioConfig_select(); //set AudioConfig data, emit signal

    };
}