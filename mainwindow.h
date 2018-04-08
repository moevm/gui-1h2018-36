#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

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
    void on_btnAddPath_clicked();

    void on_btnRemovePath_clicked();

    void on_btnClearPath_clicked();

    void on_btnPastePath_clicked();

private:
    Ui::MainWindow *ui;
    QStringListModel *model;
    QStringList fileNames;
};

#endif // MAINWINDOW_H
