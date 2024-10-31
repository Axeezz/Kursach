/********************************************************************************
** Form generated from reading UI file 'filewindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILEWINDOW_H
#define UI_FILEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileWindow
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *Path;
    QPushButton *pushButton;
    QLabel *LabelFile;

    void setupUi(QDialog *FileWindow)
    {
        if (FileWindow->objectName().isEmpty())
            FileWindow->setObjectName("FileWindow");
        FileWindow->setEnabled(true);
        FileWindow->resize(572, 306);
        layoutWidget = new QWidget(FileWindow);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 10, 551, 281));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        label = new QLabel(layoutWidget);
        label->setObjectName("label");

        verticalLayout->addWidget(label);

        Path = new QLineEdit(layoutWidget);
        Path->setObjectName("Path");

        verticalLayout->addWidget(Path);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName("pushButton");

        verticalLayout->addWidget(pushButton);


        verticalLayout_2->addLayout(verticalLayout);

        LabelFile = new QLabel(layoutWidget);
        LabelFile->setObjectName("LabelFile");

        verticalLayout_2->addWidget(LabelFile);


        retranslateUi(FileWindow);

        QMetaObject::connectSlotsByName(FileWindow);
    } // setupUi

    void retranslateUi(QDialog *FileWindow)
    {
        FileWindow->setWindowTitle(QCoreApplication::translate("FileWindow", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("FileWindow", "\320\243\320\272\320\260\320\266\320\270\321\202\320\265 \320\277\321\203\321\202\321\214 \320\272 \321\204\320\260\320\271\320\273\321\203", nullptr));
        pushButton->setText(QCoreApplication::translate("FileWindow", "Enter", nullptr));
        LabelFile->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class FileWindow: public Ui_FileWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILEWINDOW_H
