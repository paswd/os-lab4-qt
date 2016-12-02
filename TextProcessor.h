#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H

#include <QString>
#include <cstdio>
#include <cstdlib>
#include <QFileInfo>
#include <unistd.h>
#include <QVector>

bool IsSeparator(char sym);
unsigned long long ConvertToBytes(unsigned long long num, size_t unit);
unsigned long long FileSize(QString filename);
bool IsFileExists(QString filename);
QString SetSpaces(QString str);
QString GetParameter(QString str, size_t num);

QString GetRange(QString filename);
QString SearchInFilePrefix(QString filename, QString substr);
QString SearchInFilePostfix(QString filename, QString substr);
QString SearchInFileBasic(QString filename, QString substr);
QString EditWrite(QString filename, unsigned long long row, unsigned long long col, QString text);
QString EditDelete(QString filename, unsigned long long row, unsigned long long col, unsigned long long cnt);

#endif // TEXTPROCESSOR_H
