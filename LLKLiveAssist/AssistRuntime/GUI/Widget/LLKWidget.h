#pragma once
#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <qpushbutton.h>

namespace NAssist {

    class Label : public QLabel
    {
    public:
        explicit Label(QWidget *parent = nullptr)
            : QLabel(parent)
        {
            setAlignment(Qt::AlignCenter);
            setStyleSheet(R"(
                          QLabel {
                          margin: 20px;
                          font-size: 40px;
                          }
                          )");
        }
    };

    class PushButton : public QPushButton
    {
    public:
        explicit PushButton(QWidget *parent = nullptr)
            : QPushButton(parent)
        {
          setStyleSheet(R"(QPushButton {
                        border: 1px solid #8f8f91;
                        border-radius: 3px;
                        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(55, 55, 55), stop: 1 rgb(139, 139, 139));
                        min-width: 10em;}

                        QPushButton:pressed {
                            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(255, 255, 255), stop: 1 rgb(25, 76, 228));
                        }
                        QPushButton:flat {
                            border: 2px solid rgb(255, 0, 76);
                            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(255, 255, 255), stop: 1 rgb(25, 76, 228));
                        }
                        QPushButton:default {
                            border-color: navy; /* make the default button prominent */
                        })");
        }
    };


    
}