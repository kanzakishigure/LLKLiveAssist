#include "TaskPanel.h"
#include "GUI/Widget/LLKWidget.h"

#include <qboxlayout.h>
#include <QPushButton>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qwidget.h>

namespace NAssist {

    TaskPanel::TaskPanel(QWidget* parent)
    :QGroupBox(parent)
    {
        
        this->setStyleSheet(R"(
            QGroupBox {
            border: 0;
            min-width: 15em;
            }
            )");
        //add taskbox
        {
            m_taskGroupBox = new QGroupBox(this);
            QLayout* task_layout =new QVBoxLayout();

            task_layout->setAlignment(Qt::Alignment::enum_type::AlignTop);
            task_layout->setSpacing(25);
            task_layout->setMargin(40);
            m_taskGroupBox->setLayout(task_layout);
            m_taskGroupBox->setStyleSheet(R"(
                QGroupBox {
                border: 0.5px solid rgba(134, 142, 150, 0.8)
                
                }
                )");
            auto* bili_config =  new PushButton(this);
            auto* gsovit_config =  new PushButton(this);
            auto* audio_config =  new PushButton(this);
            task_layout->addWidget(bili_config);
            task_layout->addWidget(gsovit_config);
            task_layout->addWidget(audio_config);


        }
        //start Button
        {
            m_taskControlButton = new PushButton(this);
        }
        m_layout = new QVBoxLayout();
        m_layout->setAlignment(Qt::Alignment::enum_type::AlignHCenter);
        m_layout->setSpacing(25);
       
        m_layout->addWidget(m_taskGroupBox);
        m_layout->addWidget(m_taskControlButton);
        

        
       
       
        setLayout(m_layout);
       
        QMetaObject::connectSlotsByName(this);
    }
    
   

    
    void TaskPanel::on_TaskStart()
    {

    }
    void TaskPanel::on_TaskStop()
    {

    } 
    
    void TaskPanel::on_BiliConfig_select()
    {

    } 
    void TaskPanel::on_SovitsConfig_select()
    {

    }
    void TaskPanel::on_AudioConfig_select()
    {

    } 
}