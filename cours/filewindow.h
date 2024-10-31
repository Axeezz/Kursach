/*
#ifndef FILEWINDOW_H
#define FILEWINDOW_H

#include <QDialog>

namespace Ui {
class FileWindow;
}

class FileWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FileWindow(QWidget *parent = nullptr);
    ~FileWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::FileWindow *ui;
    std::string CreateUniqueFolder();
    void SaveImageToFile(QString filePath, QImage image);
};

#endif // FILEWINDOW_H
*/

#ifndef FILEWINDOW_H
#define FILEWINDOW_H

#include <QDialog>
#include <QTextEdit> // добавляем include

namespace Ui {
class FileWindow;
}

class FileWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FileWindow(QWidget *parent = nullptr);
    ~FileWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::FileWindow *ui;
    std::string CreateUniqueFolder();
    void SaveGeneratedImage(const QImage &image, const QString &folderPath);
    void AppendMessage(const QString &message);
    QTextEdit *LabelFile; // заменяем QLabel на QTextEdit
};

#endif // FILEWINDOW_H
