#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QStringListModel"
#include "QClipboard"
#include "QMessageBox"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QStringListModel();

    ui->grpByteByByte->show();
    ui->grpFilename->hide();
    ui->grpHash->hide();
    ui->tabProgress->hide();
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
    } else{
        ui->grpByteByByte->hide();
    }
}

void MainWindow::on_rbtFilename_toggled(bool checked)
{
    if(checked) {
        ui->grpFilename->show();
        this->duplicateType = QString("filename");
    } else{
        ui->grpFilename->hide();
    }
}

void MainWindow::on_rbtHash_toggled(bool checked)
{
    if(checked) {
        ui->grpHash->show();
        this->duplicateType = QString("hash");
    } else{
        ui->grpHash->hide();
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
}
