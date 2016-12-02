#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H

#include <QString>
#include <cstdio>
#include <cstdlib>
#include <QFileInfo>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <QVector>

class FileMapping {
public:
    int fd;
    size_t FileSize;
    char *DataPtr;
};

bool IsSeparator(char sym);
unsigned long long ConvertToBytes(unsigned long long num, size_t unit);
unsigned long long FileSize(QString filename);
bool IsFileExists(QString filename);
QString SetSpaces(QString str);
QString GetParameter(QString str, size_t num);

QString GetRange(QString filename, size_t max_mapping);
QString SearchInFilePrefix(QString filename, QString substr, size_t max_mapping);
QString SearchInFilePostfix(QString filename, QString substr, size_t max_mapping);
QString SearchInFileBasic(QString filename, QString substr, size_t max_mapping);
QString EditWrite(QString filename, unsigned long long row, unsigned long long col,
                  QString text, size_t max_mapping);
QString EditDelete(QString filename, unsigned long long row, unsigned long long col,
                   unsigned long long cnt, size_t max_mapping);

#endif // TEXTPROCESSOR_H
