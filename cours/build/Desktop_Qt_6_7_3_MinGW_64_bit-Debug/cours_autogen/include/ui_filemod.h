/********************************************************************************
** Form generated from reading UI file 'filemod.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILEMOD_H
#define UI_FILEMOD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_FileMod
{
public:

    void setupUi(QDialog *FileMod)
    {
        if (FileMod->objectName().isEmpty())
            FileMod->setObjectName("FileMod");
        FileMod->resize(400, 300);

        retranslateUi(FileMod);

        QMetaObject::connectSlotsByName(FileMod);
    } // setupUi

    void retranslateUi(QDialog *FileMod)
    {
        FileMod->setWindowTitle(QCoreApplication::translate("FileMod", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FileMod: public Ui_FileMod {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILEMOD_H
