/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *ImageLabel;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QPushButton *FileButton;
    QHBoxLayout *horizontalLayout;
    QLabel *TEXT;
    QLineEdit *DataString;
    QPushButton *Generator;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        ImageLabel = new QLabel(centralwidget);
        ImageLabel->setObjectName("ImageLabel");
        ImageLabel->setGeometry(QRect(170, 160, 441, 361));
        widget = new QWidget(centralwidget);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(10, 50, 771, 69));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        FileButton = new QPushButton(widget);
        FileButton->setObjectName("FileButton");

        verticalLayout->addWidget(FileButton);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        TEXT = new QLabel(widget);
        TEXT->setObjectName("TEXT");

        horizontalLayout->addWidget(TEXT);

        DataString = new QLineEdit(widget);
        DataString->setObjectName("DataString");

        horizontalLayout->addWidget(DataString);

        Generator = new QPushButton(widget);
        Generator->setObjectName("Generator");

        horizontalLayout->addWidget(Generator);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        ImageLabel->setText(QString());
        FileButton->setText(QCoreApplication::translate("MainWindow", "\320\236\320\272\320\275\320\276 \320\264\320\273\321\217 \321\200\320\260\320\261\320\276\321\202\321\213 \321\201 \320\264\320\260\320\275\320\275\321\213\320\274\320\270 \320\270\320\267 \321\204\320\260\320\271\320\273\320\260", nullptr));
        TEXT->setText(QCoreApplication::translate("MainWindow", "\320\222\320\262\320\265\320\264\320\270\321\202\320\265 \320\264\320\260\320\275\320\275\321\213\320\265", nullptr));
        Generator->setText(QCoreApplication::translate("MainWindow", "Enter", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
