#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    QString filepath = QApplication::applicationDirPath() + "/styles.css";
    qDebug() << filepath;

    QFile file(filepath);
    if(!file.open(QFile::ReadOnly |QFile::Text)) {
//        QMessageBox::information(NULL, "Ошибка", "Файл со стилями не найден");
    } else {
        QString styleSheet = QString(file.readAll());
        w.setStyleSheet(styleSheet);
    }

    w.show();
    return a.exec();
}
