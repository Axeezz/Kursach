#ifndef FUNCTIONALFILE_H
#define FUNCTIONALFILE_H
#include <QImage>
#include <QCoreApplication>
#include <QString>
#include <QByteArray>
#include <QDebug>

const int MATRIX_SIZE = 61;


class FunctionalFile
{
public:
    FunctionalFile();

private:

};

QImage createImageFromMatrix(const int matrix[MATRIX_SIZE][MATRIX_SIZE]);
extern const int matrix[MATRIX_SIZE][MATRIX_SIZE];

class NewString
{
    QByteArray binaryStream;
    char CodMet[5] = "0100";
    void AddServInf();

    public:

    void stringToBinaryDump(const QString &input, QWidget *parent);

};

#endif // FUNCTIONALFILE_H
