#include "ConsoleDilogPanel.h"
#include <qboxlayout.h>
#include <QPushButton>
namespace NAssist {

    ConsoleDilogPanel::ConsoleDilogPanel(QWidget* parent)
    :QGroupBox(parent)
    {
        m_Layout = new QVBoxLayout();
       
        
        
        setLayout(m_Layout);
        QMetaObject::connectSlotsByName(this);
    }
    

}