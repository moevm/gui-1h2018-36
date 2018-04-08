#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QStringListModel"
#include "QClipboard"

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
