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

    QStringList* getListOfFiles();
    QStringList* filterListOfFiles(QStringList* list);
    QStringList* excludeFromListOfFiles(QStringList* list);
    QStringList* getListOfFilesInDir(QString dirPath);
    QByteArray fileChecksum(QString fileName, QCryptographicHash::Algorithm hashAlgorithm);

private:
    Ui::MainWindow *ui;
    QStringListModel *modelAdded;
    QStringListModel *modelProtectedFiles;
    QStringListModel *foundDuplicatesModel;
    QStringList addedFileNames;
    QStringList addedFileNamesProtected;
    QStringList foundFilesList;
    QStringList foundDuplicatesList;
    QList<QFile> fileList;
    QString duplicateType = QString("byteByByte");

    QMap<QString, QString> filterTypes;
};

#endif // MAINWINDOW_H
