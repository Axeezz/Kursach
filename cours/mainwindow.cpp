#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "filewindow.h"
#include "functionalfile.h"
#include <QApplication>
#include <QImage>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Generator_clicked()
{
    NewString string;

    string.stringToBinaryDump(ui -> DataString -> text(), this);

    QImage image = createImageFromMatrix(matrix);

    ui->ImageLabel->setPixmap(QPixmap::fromImage(image));
    ui->ImageLabel->show();
}


void MainWindow::on_FileButton_clicked()
{
    FileWindow window;
    window.setModal(true);
    window.exec();
}

