#include "TaskConfigPanel.h"


#include <qboxlayout.h>
#include <QPushButton>
namespace NAssist {

    TaskConfigPanel::TaskConfigPanel(QWidget* parent)
    :QGroupBox(parent)
    {
        m_Layout = new QVBoxLayout();
       
        
        setLayout(m_Layout);
        
        QMetaObject::connectSlotsByName(this);
    }
    

}