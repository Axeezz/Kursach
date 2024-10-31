#include "filewindow.h"
#include "ui_filewindow.h"
#include "functionalfile.h"
#include <QFile>
#include <filesystem>
#include <string>

FileWindow::FileWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FileWindow)
    , LabelFile(new QTextEdit(this)) // инициализируем QTextEdit
{
    ui->setupUi(this);
    LabelFile->setReadOnly(true); // делаем текстовое поле только для чтения
    ui->verticalLayout->addWidget(LabelFile); // добавляем текстовое поле в layout
}

FileWindow::~FileWindow()
{
    delete ui;
}

std::string FileWindow::CreateUniqueFolder()
{
    std::string baseName = "QR-Codes";
    std::string folderName = baseName;
    int count = 1;

    while (std::filesystem::exists(folderName)) {
        folderName = baseName + "_" + std::to_string(count);
        count++;
    }

    std::string currentPath = std::filesystem::current_path().string();
    std::string fullFolderPath = currentPath + "/" + folderName;
    std::filesystem::create_directory(fullFolderPath);

    AppendMessage(QString::fromStdString("Папка " + folderName + " успешно создана. \n" + fullFolderPath + "\n"));

    return fullFolderPath;
}

void FileWindow::SaveGeneratedImage(const QImage &image, const QString &folderPath)
{
    QString baseName = "image";
    QString fileExtension = ".png";
    QString fileName = baseName + fileExtension;
    QString filePath = folderPath + "/" + fileName;
    int count = 1;

    // Проверяем, существует ли файл с таким именем
    while (QFile::exists(filePath))
    {
        // Если существует, добавляем номер к имени файла
        fileName = baseName + "_" + QString::number(count) + fileExtension;
        filePath = folderPath + "/" + fileName;
        count++;
    }

    // Сохраняем изображение
    image.save(filePath);
    AppendMessage("Изображение сохранено по адресу: " + filePath + "\n");
}

void FileWindow::AppendMessage(const QString &message)
{
    LabelFile->append(message); // используем append для QTextEdit
}

void FileWindow::on_pushButton_clicked()
{
    QString Path = ui->Path->text();

    QString folderPath = QString::fromStdString(CreateUniqueFolder());

    QImage Image = createImageFromMatrix(matrix);

    SaveGeneratedImage(Image, folderPath);
}






