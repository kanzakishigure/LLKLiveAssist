#pragma once 

#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QGridLayout>
#include <qlayout.h>
#include <qobject.h>
#include <QGroupBox>

namespace NAssist {

    class TaskConfigPanel : public QGroupBox
    {
        Q_OBJECT  
        QLayout* m_Layout;
    public:
        explicit TaskConfigPanel(QWidget* parent);
        
    
    private Q_SLOTS:


    };
}