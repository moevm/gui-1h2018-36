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

    void on_rbtByteByByte_toggled(bool checked);

    void on_rbtFilename_toggled(bool checked);

    void on_rbtHash_toggled(bool checked);

    void on_rbtFolders_toggled(bool checked);

    void on_rbtUnique_toggled(bool checked);

    void on_btnSearch_clicked();

    void on_dateCheckBox_toggled(bool checked);

    void on_checkBox_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QStringListModel *model;
    QStringList fileNames;
    QString duplicateType = QString("byteByByte");
};

#endif // MAINWINDOW_H
