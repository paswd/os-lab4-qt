#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TextProcessor.h"
#include <QString>
#include <QFileDialog>
#include <QCursor>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_InteractiveSelectFile_clicked();

    void on_ConsoleStart_clicked();

    void on_InteractiveGetRange_clicked();

    void on_InteractiveResultClear_clicked();

    void on_InteractiveSetFileLimits_clicked();

    void on_InteractiveSearch_clicked();

    void on_InteractiveEdit_clicked();

    void on_InteractiveEditWrite_clicked();

    void on_InteractiveEditDelete_clicked();

private:
    Ui::MainWindow *ui;
};

bool IsCurrentFileInRange(QString filename, Ui::MainWindow *ui);

#endif // MAINWINDOW_H
