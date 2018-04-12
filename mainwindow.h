#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QFile>
#include <QCryptographicHash>

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
    void on_btnSearch_clicked();
    void on_btnAddPathProtected_clicked();
    void on_btnRemovePathProtected_clicked();
    void on_btnClearPathProtected_clicked();
    void on_btnPastePathProtected_clicked();

    void on_rbtByteByByte_toggled(bool checked);
    void on_rbtFilename_toggled(bool checked);
    void on_rbtHash_toggled(bool checked);
    void on_rbtFolders_toggled(bool checked);
    void on_rbtUnique_toggled(bool checked);
    void on_chbFilterDate_toggled(bool checked);
    void on_chbFilterSize_toggled(bool checked);

    void getListOfDirectoriesToSearch();
    QStringList* getListOfFiles();
    QByteArray fileChecksum(QString fileName, QCryptographicHash::Algorithm hashAlgorithm);

private:
    Ui::MainWindow *ui;
    QStringListModel *model;
    QStringListModel *modelProtected;
    QStringListModel *foundFilesModel;
    QStringListModel *foundDuplicatesModel;
    QStringList fileNames;
    QStringList fileNamesProtected;
    QStringList foundFilesList;
    QStringList foundDuplicatesList;
    QList<QFile> fileList;
    QString duplicateType = QString("byteByByte");
};

#endif // MAINWINDOW_H
