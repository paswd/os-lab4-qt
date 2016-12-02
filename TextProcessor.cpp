#include "TextProcessor.h"
#include <QString>
#include <cstdio>
#include <cstdlib>
#include <QFileInfo>
#include <unistd.h>
#include <QVector>
#include <QDebug>

bool IsSeparator(char sym) {
    if (sym == ' ' || sym == '\n' || sym == '\t' || sym == '\0') {
        return true;
    }
    return false;
}

unsigned long long ConvertToBytes(unsigned long long num, size_t unit) {
    /*if (unit == "КБ") {
        num *= 1024;
    } else if (unit == "МБ") {
        num *= 1024 * 1024;
    } else if (unit == "ГБ") {
        num *= 1024 * 1024 * 1024;
    }*/
    for (size_t i = 0; i < unit; i++) {
        num *= 1024;
    }
    return num;
}

unsigned long long FileSize(QString filename) {
    QFileInfo info(filename.toLocal8Bit());
    return (unsigned long long) info.size();
}

bool IsFileExists(QString filename) {
    FILE *file = fopen(filename.toLocal8Bit(), "r");
    if (file != NULL) {
        fclose(file);
        return true;
    }
    return false;
}

QString SetSpaces(QString str) {
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '_') {
            str[i] = ' ';
        }
    }
    return str;
}

QString GetParameter(QString str, size_t num) {
    str.simplified();
    size_t cnt = 0;
    int i;
    bool special = false;
    for (i = 0; cnt < num && i < str.size(); i++) {
        //qDebug() << "Test1";
        if (special) {
            special = false;
            continue;
        }
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t') {
            cnt++;
        }
        if (str[i] == '\\') {
           special = true;
        }
    }
    //size_t start = i;
    special = false;
    QString res = "";
    for (; cnt == num && i < str.size(); i++) {
        //qDebug() << "Test2";
        if (special) {
            res += str[i];
            special = false;
            continue;
        }

        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t') {
            cnt++;
        }
        if (str[i] == '\\') {
           special = true;
           continue;
        }
        res += str[i];
    }
    return res;
}

QString GetRange(QString filename) {
    const size_t alphabet_len = 256;
    size_t arr[alphabet_len];
    for (size_t i = 0; i < alphabet_len; i++) {
        arr[i] = 0;
    }
    FILE *file;
    QString result = "";
    if ((file = fopen(filename.toLocal8Bit(), "r")) == NULL) {
        result = "Cannot open file\n";
        return result;
    }

    char sym;
    while ((sym = fgetc(file)) != EOF) {
        sym %= alphabet_len;
        arr[(size_t) sym]++;
    }
    fclose(file);
    for (size_t i = 0; i < alphabet_len; i++) {
        if (IsSeparator(arr[i])) {
            continue;
        }
        if (arr[i] > 0) {
            result += (char) i;
            result += " (";
            result += QString::number(i);
            result += "): ";
            result += QString::number(arr[i]);
            result += '\n';
        }
    }
    return result;
}

QString SearchInFilePrefix(QString filename, QString substr) {
    FILE *file;
    QString result = "";
    if ((file = fopen(filename.toLocal8Bit(), "r")) == NULL) {
        result = "Cannot open file\n";
        return result;
    }

    char sym;
    bool ignore = false;
    bool found = false;
    unsigned long long pos = 0;

    unsigned long long col = 1;
    unsigned long long row = 1;

    unsigned long long current_col = 0;
    unsigned long long current_row = 0;
    while ((sym = fgetc(file)) != EOF) {
        if (ignore) {
            if (sym == '\n') {
                col = 1;
                row++;
            } else {
                col++;
            }
            if (IsSeparator(sym)) {
                ignore = false;
            }
            continue;
        }

        if (substr[(int) pos].toLatin1() == sym) {
            if (!pos) {
                current_col = col;
                current_row = row;
            }
            if (pos >= (size_t) substr.size() - 1) {
                found = true;
                break;
            }
            pos++;
        } else {
            pos = 0;
            if (!IsSeparator(sym)) {
                ignore = true;
            }
        }
        if (sym == '\n') {
            col = 1;
            row++;
        } else {
            col++;
        }
    }
    fclose(file);
    if (found) {
        result = "Search (prefix): First found position: (";
        result += QString::number(current_row) + ", " + QString::number(current_col) + ")\n";
    } else {
        result = "Search (prefix): Not found\n";
    }
    return result;
}

QString SearchInFilePostfix(QString filename, QString substr) {
    FILE *file;
    QString result = "";
    if ((file = fopen(filename.toLocal8Bit(), "r")) == NULL) {
        result = "Cannot open file\n";
        return result;
    }

    char sym;
    bool found = false;
    unsigned long long pos = 0;

    unsigned long long col = 1;
    unsigned long long row = 1;

    unsigned long long current_col = 0;
    unsigned long long current_row = 0;

    QVector <char> arr_tmp;
    arr_tmp.resize(substr.size());
    qDebug() << "Size: " << substr << " - " << substr.size();
    size_t nw_pos = arr_tmp.size() - 1;
    while ((sym = fgetc(file)) != EOF) {
        if (IsSeparator(sym)) {
            found = true;
            for (size_t i = 0; i < substr.size(); i++) {
                qDebug() << arr_tmp[i] << ":" << substr[(int) i];
                if (arr_tmp[i] != substr[(int) i]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                current_col = col;
                current_row = row;
                break;
            }
        }
        for (size_t i = nw_pos + 1; i < (size_t) arr_tmp.size(); i++) {
            arr_tmp[i - 1] = arr_tmp[i];
        }
        arr_tmp[arr_tmp.size() - 1] = sym;

        if (sym == '\n') {
            col = 1;
            row++;
        } else {
            col++;
        }
        if (nw_pos > 0) {
            nw_pos--;
        }
    }
    fclose(file);
    if (found) {
        result = "Search (postfix): First found position: (";
        result += QString::number(current_row) + ", " + QString::number(current_col - substr.size()) + ")\n";
    } else {
        result = "Search (postfix): Not found\n";
    }
    return result;
}

QString SearchInFileBasic(QString filename, QString substr) {
    FILE *file;
    QString result = "";
    if ((file = fopen(filename.toLocal8Bit(), "r")) == NULL) {
        result = "Cannot open file\n";
        return result;
    }

    char sym;
    bool found = false;
    unsigned long long pos = 0;

    //unsigned long long col = 1;
    //unsigned long long row = 1;

    //unsigned long long current_col = 0;
    //unsigned long long current_row = 0;

    QVector <char> arr_tmp;
    QVector <unsigned long long> cols;
    QVector <unsigned long long> rows;
    arr_tmp.resize(substr.size());
    cols.resize(substr.size());
    rows.resize(substr.size());
    size_t nw_pos = arr_tmp.size() - 1;
    cols[cols.size() - 1] = 1;
    rows[rows.size() - 1] = 1;
    while ((sym = fgetc(file)) != EOF) {
        for (size_t i = nw_pos + 1; i < (size_t) arr_tmp.size(); i++) {
            arr_tmp[i - 1] = arr_tmp[i];
        }
        arr_tmp[arr_tmp.size() - 1] = sym;
        for (size_t i = nw_pos + pos; i < (size_t) arr_tmp.size(); i++) {
            if (substr[(int) pos].toLatin1() == arr_tmp[i]) {
                if (pos >= (size_t) substr.size() - 1) {
                    found = true;
                    break;
                }
                pos++;
            } else {
                pos = 0;
                break;
            }
        }
        if (found) {
            break;
        }

        if (nw_pos > 0) {
            nw_pos--;
        }
        for (size_t i = nw_pos + 1; i < (size_t) arr_tmp.size(); i++) {
            cols[i - 1] = cols[i];
            rows[i - 1] = rows[i];
        }
        if (sym == '\n') {
            cols[cols.size() - 1] = 1;
            rows[rows.size() - 1]++;
        } else {
            cols[cols.size() - 1]++;
        }
    }
    fclose(file);
    if (found) {
        result = "Search (basic): First found position: (";
        result += QString::number(rows[0]) + ", " + QString::number(cols[0]) + ")\n";
    } else {
        result = "Search (basic): Not found\n";
    }
    return result;
}

QString EditWrite(QString filename, unsigned long long row, unsigned long long col, QString text) {
    FILE *file;
    QString result = "";
    if ((file = fopen(filename.toLocal8Bit(), "r")) == NULL) {
        result = "Cannot open file\n";
        return result;
    }
    QFileInfo file_info(filename.toLocal8Bit());
    QString filename_tmp = file_info.absolutePath() + ".tmp_" + file_info.baseName();
    FILE *file_tmp = fopen(filename_tmp.toLocal8Bit(), "r");
    if (file_tmp != NULL) {
        fclose(file_tmp);
        unlink(filename_tmp.toLocal8Bit());
    }
    file_tmp = fopen(filename_tmp.toLocal8Bit(), "w");

    char sym;
    unsigned long long current_row = 1;
    unsigned long long current_col = 1;
    while (current_row < row || (current_row == row && current_col < col)) {
        if ((sym = fgetc(file)) == EOF) {
            break;
        }
        if (sym == '\n') {
            current_col = 1;
            current_row++;
            continue;
        }
        current_col++;
        fwrite(&sym, sizeof(char), 1, file_tmp);
    }
    for (size_t i = 0; i < (size_t) text.size(); i++) {
        char tmp = text[(int) i].toLatin1();
        fwrite(&tmp, sizeof(char), 1, file_tmp);
    }
    if (sym != EOF) {
        while ((sym = fgetc(file)) != EOF) {
            fwrite(&sym, sizeof(char), 1, file_tmp);
        }
    }
    fclose(file);
    fclose(file_tmp);
    unlink(filename.toLocal8Bit());
    rename(filename_tmp.toLocal8Bit(), filename.toLocal8Bit());
    return "Edit (Write): Success\n";
}

QString EditDelete(QString filename, unsigned long long row, unsigned long long col, unsigned long long cnt) {
    FILE *file;
    QString result = "";
    if ((file = fopen(filename.toLocal8Bit(), "r")) == NULL) {
        result = "Cannot open file\n";
        return result;
    }
    QFileInfo file_info(filename.toLocal8Bit());
    QString filename_tmp = file_info.absolutePath() + ".tmp_" + file_info.baseName();
    FILE *file_tmp = fopen(filename_tmp.toLocal8Bit(), "r");
    if (file_tmp != NULL) {
        fclose(file_tmp);
        unlink(filename_tmp.toLocal8Bit());
    }
    file_tmp = fopen(filename_tmp.toLocal8Bit(), "w");

    char sym;
    unsigned long long current_row = 1;
    unsigned long long current_col = 1;
    while (current_row < row || (current_row == row && current_col < col)) {
        if ((sym = fgetc(file)) == EOF) {
            break;
        }
        if (sym == '\n') {
            current_col = 1;
            current_row++;
            continue;
        }
        current_col++;
        fwrite(&sym, sizeof(char), 1, file_tmp);
    }
    unsigned long long i = 0;
    while ((sym = fgetc(file)) != EOF) {
        if (i < cnt) {
            i++;
            continue;
        }
        fwrite(&sym, sizeof(char), 1, file_tmp);
    }

    fclose(file);
    fclose(file_tmp);
    unlink(filename.toLocal8Bit());
    rename(filename_tmp.toLocal8Bit(), filename.toLocal8Bit());
    return "Edit (Delete): Success\n";
}

