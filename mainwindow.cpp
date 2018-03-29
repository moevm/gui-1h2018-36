#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->grpByteByByte->show();
    ui->grpFilename->hide();
    ui->grpHash->hide();
    ui->tabProgress->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}
