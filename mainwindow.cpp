#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QCursor>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->showMessage(tr("Developed in 2016 by PasWD (Pavel Sushko), dept. 806, MAI (8O-207b)"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_InteractiveSelectFile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open file"),
                "",
                "Text files (*.txt);;All files (*.*)"
                );
    if (filename != "") {
        ui->InteractiveFileName->setText(filename);
    }
}

void MainWindow::on_ConsoleStart_clicked()
{
    //Action console
    this->setCursor(Qt::WaitCursor);
    unsigned long long max_mapping = ui->SettingsMemoryLimit->text().toULongLong();
    max_mapping = ConvertToBytes(max_mapping, ui->SettingsMemoryLimitType->currentIndex());
    QString command = ui->ConsoleIn->text();
    command = command.simplified();
    QString action = command.section(' ', 0, 0);
    if (action == "range") {
        QString filename = command.section(' ', 1, 1);
        if (!IsFileExists(filename)) {
            ui->ConsoleOut->setText("Cannot open file\n");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        unsigned long long min = command.section(' ', 2, 2).toULongLong();
        unsigned long long max = command.section(' ', 3, 3).toULongLong();
        bool is_limit = true;
        if (min == 0 && max == 0) {
            is_limit = false;
        }
        unsigned long long size = FileSize(filename);
        if ((size > max || size < min) && is_limit) {
            ui->ConsoleOut->setText("File size is out of range\n");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        ui->ConsoleOut->setText(GetRange(filename, max_mapping));
    } else if (action == "search") {
        //QString filename = command.section(' ', 1, 1);
        QString filename = GetParameter(command, 1);
        if (!IsFileExists(filename)) {
            ui->ConsoleOut->setText("Cannot open file\n");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        unsigned long long min = command.section(' ', 4, 4).toULongLong();
        unsigned long long max = command.section(' ', 5, 5).toULongLong();
        bool is_limit = true;
        if (min == 0 && max == 0) {
            is_limit = false;
        }
        unsigned long long size = FileSize(filename);
        if ((size > max || size < min) && is_limit) {
            ui->ConsoleOut->setText("File size is out of range\n");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        QString type = command.section(' ', 2, 2);
        /*QString substr = command.section(' ', 3, 3);
        substr = SetSpaces(substr);*/
        QString substr = GetParameter(command, 3);

        if (type == "basic") {
            ui->ConsoleOut->setText(SearchInFileBasic(filename, substr, max_mapping));
        } else if (type == "prefix") {
            ui->ConsoleOut->setText(SearchInFilePrefix(filename, substr, max_mapping));
        } else if (type == "postfix") {
            ui->ConsoleOut->setText(SearchInFilePostfix(filename, substr, max_mapping));
        } else {
            ui->ConsoleOut->setText("Parameter is incorrect\n");
        }

    } else if (action == "edit") {
        QString filename = command.section(' ', 1, 1);
        if (!IsFileExists(filename)) {
            ui->ConsoleOut->setText("Cannot open file\n");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        unsigned long long min = command.section(' ', 5, 5).toULongLong();
        unsigned long long max = command.section(' ', 6, 6).toULongLong();
        bool is_limit = true;
        if (min == 0 && max == 0) {
            is_limit = false;
        }
        unsigned long long size = FileSize(filename);
        if ((size > max || size < min) && is_limit) {
            ui->ConsoleOut->setText("File size is out of range\n");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        unsigned long long row = command.section(' ', 2, 2).toULongLong();
        unsigned long long col = command.section(' ', 3, 3).toULongLong();
        //QString text = command.section(' ', 4, 4);
        QString text = GetParameter(command, 4);
        ui->ConsoleOut->setText(EditWrite(filename, row, col, text, max_mapping));

    } else if (action == "delete") {
        QString filename = command.section(' ', 1, 1);
        if (!IsFileExists(filename)) {
            ui->ConsoleOut->setText("Cannot open file\n");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        unsigned long long min = command.section(' ', 5, 5).toULongLong();
        unsigned long long max = command.section(' ', 6, 6).toULongLong();
        bool is_limit = true;
        if (min == 0 && max == 0) {
            is_limit = false;
        }
        unsigned long long size = FileSize(filename);
        if ((size > max || size < min) && is_limit) {
            ui->ConsoleOut->setText("File size is out of range\n");
            return;
            this->setCursor(Qt::ArrowCursor);
        }

        unsigned long long row = command.section(' ', 2, 2).toULongLong();
        unsigned long long col = command.section(' ', 3, 3).toULongLong();
        unsigned long long length = command.section(' ', 4, 4).toULongLong();
        ui->ConsoleOut->setText(EditDelete(filename, row, col, length, max_mapping));

    } else {
        ui->ConsoleOut->setText("Unknown command\n");
    }
    this->setCursor(Qt::ArrowCursor);

    /*
    this->setCursor(Qt::WaitCursor);
    QString filename = ui->InteractiveFileName->text();
    if (!IsCurrentFileInRange(filename, ui)) {
        ui->InteractiveResult->setText("File size is out of range\n");
        return;
    }
    ui->InteractiveResult->setText(GetRange(filename));
    this->setCursor(Qt::ArrowCursor); */
}

void MainWindow::on_InteractiveResultClear_clicked()
{
    QString tmp = "";
    ui->InteractiveResult->setText(tmp);
}

void MainWindow::on_InteractiveSetFileLimits_clicked()
{
    bool is_enabled = false;
    if (ui->InteractiveSetFileLimits->isChecked()) {
        is_enabled = true;
    }
    ui->InteractiveFileSizeMax->setEnabled(is_enabled);
    ui->InteractiveFileSizeMaxType->setEnabled(is_enabled);
    ui->InteractiveFileSizeMin->setEnabled(is_enabled);
    ui->InteractiveFileSizeMinType->setEnabled(is_enabled);
}

void MainWindow::on_InteractiveGetRange_clicked()
{
    this->setCursor(Qt::WaitCursor);
    unsigned long long max_mapping = ui->SettingsMemoryLimit->text().toULongLong();
    max_mapping = ConvertToBytes(max_mapping, ui->SettingsMemoryLimitType->currentIndex());
    QString filename = ui->InteractiveFileName->text();
    if (!IsCurrentFileInRange(filename, ui)) {
        ui->InteractiveResult->setText("File size is out of range\n");
        return;
    }
    ui->InteractiveResult->setText(GetRange(filename, max_mapping));
    this->setCursor(Qt::ArrowCursor);
}

void MainWindow::on_InteractiveSearch_clicked()
{
    this->setCursor(Qt::WaitCursor);
    unsigned long long max_mapping = ui->SettingsMemoryLimit->text().toULongLong();
    max_mapping = ConvertToBytes(max_mapping, ui->SettingsMemoryLimitType->currentIndex());
    QString filename = ui->InteractiveFileName->text();
    if (!IsCurrentFileInRange(filename, ui)) {
        ui->InteractiveResult->setText("File size is out of range\n");
        return;
    }
    QString substr = ui->InteractiveSearchSubstr->text();
    QString res = "";
    if (ui->InteractiveSearchParamPrefix->isChecked()) {
        res = SearchInFilePrefix(filename, substr, max_mapping);
    } else if (ui->InteractiveSearchParamPostfix->isChecked()) {
        res = SearchInFilePostfix(filename, substr, max_mapping);
    } else {
        res = SearchInFileBasic(filename, substr, max_mapping);
    }
    ui->InteractiveResult->setText(res);
    this->setCursor(Qt::ArrowCursor);
}

void MainWindow::on_InteractiveEdit_clicked()
{
    this->setCursor(Qt::WaitCursor);
    unsigned long long max_mapping = ui->SettingsMemoryLimit->text().toULongLong();
    max_mapping = ConvertToBytes(max_mapping, ui->SettingsMemoryLimitType->currentIndex());
    QString filename = ui->InteractiveFileName->text();
    if (!IsCurrentFileInRange(filename, ui)) {
        ui->InteractiveResult->setText("File size is out of range\n");
        return;
    }
    unsigned long long col = (size_t) ui->InteractiveEditCol->text().toULongLong();
    unsigned long long row = (size_t) ui->InteractiveEditRow->text().toULongLong();
    if (ui->InteractiveEditWrite->isChecked()) {
        QString text = ui->InteractiveEditText->toPlainText();
        ui->InteractiveResult->setText(EditWrite(filename, row, col, text, max_mapping));
    } else {
        unsigned long long cnt = ui->InteractiveEditSymCnt->text().toULongLong();
        ui->InteractiveResult->setText(EditDelete(filename, row, col, cnt, max_mapping));
    }
    this->setCursor(Qt::ArrowCursor);
}

void MainWindow::on_InteractiveEditWrite_clicked()
{
    ui->InteractiveEditSymCnt->setEnabled(false);
    ui->InteractiveEditText->setEnabled(true);
}

void MainWindow::on_InteractiveEditDelete_clicked()
{
    ui->InteractiveEditSymCnt->setEnabled(true);
    ui->InteractiveEditText->setEnabled(false);
}

bool IsCurrentFileInRange(QString filename, Ui::MainWindow *ui) {
    if (!ui->InteractiveSetFileLimits->isChecked()) {
        return true;
    }
    unsigned long long min = ui->InteractiveFileSizeMin->text().toULongLong();
    unsigned long long max = ui->InteractiveFileSizeMax->text().toULongLong();
    min = ConvertToBytes(min, ui->InteractiveFileSizeMinType->currentIndex());
    max = ConvertToBytes(max, ui->InteractiveFileSizeMaxType->currentIndex());
    unsigned long long fsize = FileSize(filename);
    if (fsize >= min && fsize <= max) {
        return true;
    }
    return false;
}
