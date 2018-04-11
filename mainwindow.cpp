#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QStringListModel"
#include "QClipboard"
#include "QMessageBox"
#include "QCryptographicHash"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QStringListModel();
    modelProtected = new QStringListModel();

    ui->grpByteByByte->show();
    ui->grpFilename->hide();
    ui->grpHash->hide();
    ui->tabProgress->hide();
    ui->dateSince->setEnabled(false);
    ui->dateUntil->setEnabled(false);
    ui->dspbMinSize->setEnabled(false);
    ui->dspbMaxSize->setEnabled(false);
    ui->cmbSize->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAddPath_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);

    QString fileName;
    QStringList selectedFileNames;
    if (dialog.exec())
        selectedFileNames = dialog.selectedFiles();

    for(int i = 0; i < selectedFileNames.length(); i++){
        fileName = selectedFileNames.at(i);
        if(!fileNames.contains(fileName))
            fileNames << fileName;
    }
    model->setStringList(fileNames);
    ui->listView->setModel(model);
}

void MainWindow::on_btnRemovePath_clicked()
{
    QModelIndex index = ui->listView->currentIndex();
    QString itemToDelete = index.data(Qt::DisplayRole).toString();
    fileNames.removeOne(itemToDelete);
    model->setStringList(fileNames);
    ui->listView->setModel(model);
}

void MainWindow::on_btnClearPath_clicked()
{
    fileNames.clear();
    model->setStringList(fileNames);
    ui->listView->setModel(model);
}

void MainWindow::on_btnPastePath_clicked()
{
    QClipboard* c = QApplication::clipboard();
    QString fileName = c->text();
    if(fileName != "") {
        fileName.replace("\\", "/");
        if (QFile(fileName).exists()) {
            if(!fileNames.contains(fileName))
                fileNames << fileName;
        }
    }
    model->setStringList(fileNames);
    ui->listView->setModel(model);
}

void MainWindow::on_rbtByteByByte_toggled(bool checked)
{
    if(checked) {
        ui->grpByteByByte->show();
        this->duplicateType = QString("byteByByte");
    } else {
        ui->grpByteByByte->hide();
        ui->chbBBDatetime->setChecked(false);
        ui->chbBBExtension->setChecked(false);
        ui->chbBBFilename->setChecked(false);
    }
}

void MainWindow::on_rbtFilename_toggled(bool checked)
{
    if(checked) {
        ui->grpFilename->show();
        this->duplicateType = QString("filename");
    } else {
        ui->grpFilename->hide();
        ui->chbFNDatetime->setChecked(false);
        ui->chbFNExtension->setChecked(false);
        ui->chbFNFilename->setChecked(false);
        ui->chbFNFileSize->setChecked(false);
    }
}

void MainWindow::on_rbtHash_toggled(bool checked)
{
    if(checked) {
        ui->grpHash->show();
        this->duplicateType = QString("hash");
    } else {
        ui->grpHash->hide();
        ui->chbHashExtension->setChecked(false);
        ui->chbHashFilename->setChecked(false);
        ui->chbHashFileSize->setChecked(false);
        ui->chbHashUseCache->setChecked(false);
    }
}

void MainWindow::on_rbtFolders_toggled(bool checked)
{
    if(checked) {
        this->duplicateType = QString("folders");
    }
}

void MainWindow::on_rbtUnique_toggled(bool checked)
{
    if(checked) {
        this->duplicateType = QString("unique");
    }
}

void MainWindow::on_btnSearch_clicked()
{
    QMessageBox::information(NULL, "Тип дубликата", this->duplicateType);
    QStringList* list = this->getListOfFiles();
}

void MainWindow::on_btnAddPathProtected_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);

    QString fileName;
    QStringList selectedFileNames;
    if (dialog.exec())
        selectedFileNames = dialog.selectedFiles();

    for(int i = 0; i < selectedFileNames.length(); i++){
        fileName = selectedFileNames.at(i);
        if(!fileNamesProtected.contains(fileName))
            fileNamesProtected << fileName;
    }
    modelProtected->setStringList(fileNamesProtected);
    ui->listViewProtected->setModel(modelProtected);
}

void MainWindow::on_btnRemovePathProtected_clicked()
{
    QModelIndex index = ui->listViewProtected->currentIndex();
    QString itemToDelete = index.data(Qt::DisplayRole).toString();
    fileNamesProtected.removeOne(itemToDelete);
    modelProtected->setStringList(fileNamesProtected);
    ui->listViewProtected->setModel(modelProtected);
}

void MainWindow::on_btnClearPathProtected_clicked()
{
    fileNamesProtected.clear();
    modelProtected->setStringList(fileNamesProtected);
    ui->listViewProtected->setModel(modelProtected);
}

void MainWindow::on_btnPastePathProtected_clicked()
{
    QClipboard* c = QApplication::clipboard();
    QString fileName = c->text();
    if(fileName != "") {
        fileName.replace("\\", "/");
        if (QFile(fileName).exists()) {
            if(!fileNamesProtected.contains(fileName))
                fileNamesProtected << fileName;
        }
    }
    modelProtected->setStringList(fileNamesProtected);
    ui->listViewProtected->setModel(modelProtected);
}

void MainWindow::on_chbFilterDate_toggled(bool checked)
{
    ui->dateSince->setEnabled(checked);
    ui->dateUntil->setEnabled(checked);
}

void MainWindow::on_chbFilterSize_toggled(bool checked)
{
    ui->dspbMinSize->setEnabled(checked);
    ui->dspbMaxSize->setEnabled(checked);
    ui->cmbSize->setEnabled(checked);
}

void MainWindow::getListOfDirectoriesToSearch()
{
}

QStringList* MainWindow::getListOfFiles()
{
    QStringList* result = new QStringList();
    result->append(fileNames);

    for (int i = 0; i < result->size(); i++) {
        QString fileName = result->at(i);
        QFileInfo info(fileName);
        if (info.isDir()) {
            for (QFileInfo childFileInfo : QDir(fileName).entryInfoList()) {
                QString childFileName = childFileInfo.fileName();
                if (childFileName != "." && childFileName != "..") {
                    result->append(childFileInfo.filePath());
                }
            }
        }
    }

    for(QString fileName : *result) {
        QFileInfo info(fileName);
        if (info.isDir()) {
            result->removeOne(fileName);
        }
    }
    //Next two lines for debug
//    model->setStringList(*result);
//    ui->listView->setModel(model);

    return result;
}

QByteArray MainWindow::fileChecksum(QString fileName, QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}
