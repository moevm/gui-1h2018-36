#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QStringListModel"
#include "QClipboard"
#include "QMessageBox"
#include "QCryptographicHash"
#include "QStandardItem"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    searchDirectoriesModel = new QStringListModel();
    searchProtectedDirectoriesModel = new QStringListModel();
    foundDuplicatesModel = new QStringListModel();

    ui->grpByteByByte->show();
    ui->grpFilename->hide();
    ui->grpHash->hide();

    ui->dateSince->setCalendarPopup(true);
    ui->dateUntil->setCalendarPopup(true);

    filterTypes.insert("Все файлы", "(\\S|\\s)*\\.(\\S|\\s)*$");
    filterTypes.insert("Картинки", "(\\S|\\s)*\\.(bmp|gif|jpg|pcx|png|psd|ico|tif|pct)$");
    filterTypes.insert("Все картинки", "(\\S|\\s)*\\.(bmp|gif|jpg|jpeg|pcx|png|psd|ico|tif|pct|jfif|art|rle|crw|dcx|dib|tga|tiff|gem|pic|pix|vda|vst|wpg)$");
    filterTypes.insert("Аудиофайлы", "(\\S|\\s)*\\.(mpa|mp2|mp3|m4a|wav|wma|mid|midi|aif|aiff|au|ra|rmi|snd|ogg)$");
    filterTypes.insert("Видеофайлы", "(\\S|\\s)*\\.(avi|mov|movie|mpg|m1v|mpeg|qt|ram|wm|wmv)$");
    filterTypes.insert("Файлы Flash", "(\\S|\\s)*\\.(flv|f4v|f4p|f4a|f4b)$");
    filterTypes.insert("Документы", "(\\S|\\s)*\\.(doc|txt|rtf|html|htm|pdf|ppt|pps|xls|xml|dot|csv|tab|asc)$");
    filterTypes.insert("Файлы MS Excel", "(\\S|\\s)*\\.(xls|xlsx|xlsm|xlsb|xltx|xltm|xlam|xlw)$");
    filterTypes.insert("Файлы MS Word", "(\\S|\\s)*\\.(doc|docx|docm|dotx|dotm|docb)$");
    filterTypes.insert("Архивы", "(\\S|\\s)*\\.(zip|cab|arc|arj|rar|jar|ace|lhz|lha|pak|bh|gz|gzip|tar|tgz|sea|sit|zoo)$");
    filterTypes.insert("Программные файлы", "(\\S|\\s)*\\.(exe|dll|com|sys|ocx|vbx|bat|msi)$");
    filterTypes.insert("Исходные тексты программ", "(\\S|\\s)*\\.(asm|c|cpp|css|h|bas|vba|dpr|dfm|inc|pas|js|java)$");
    filterTypes.insert("Текстовые файлы", "(\\S|\\s)*\\.(txt|bak)$");

    sizeNames.insert("Kb", 1024);
    sizeNames.insert("Mb", 1048576);
    sizeNames.insert("Gb", 1073741824);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAddPath_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);

    QStringList selectedFolders;
    if (dialog.exec())
        selectedFolders = dialog.selectedFiles();

    for(int i = 0; i < selectedFolders.length(); i++){
        QString folderName = selectedFolders.at(i);
        if(!searchDirectoriesList.contains(folderName))
            searchDirectoriesList << folderName;
    }
    searchDirectoriesModel->setStringList(searchDirectoriesList);
    ui->lstAddedDirectories->setModel(searchDirectoriesModel);
}

void MainWindow::on_btnRemovePath_clicked()
{
    QModelIndex index = ui->lstAddedDirectories->currentIndex();
    QString itemToDelete = index.data(Qt::DisplayRole).toString();
    searchDirectoriesList.removeOne(itemToDelete);
    searchDirectoriesModel->setStringList(searchDirectoriesList);
    ui->lstAddedDirectories->setModel(searchDirectoriesModel);
}

void MainWindow::on_btnClearPath_clicked()
{
    searchDirectoriesList.clear();
    searchDirectoriesModel->setStringList(searchDirectoriesList);
    ui->lstAddedDirectories->setModel(searchDirectoriesModel);
}

void MainWindow::on_btnPastePath_clicked()
{
    QClipboard* c = QApplication::clipboard();
    QString fileName = c->text();
    if(fileName != "") {
        fileName.replace("\\", "/");
        if (QFile(fileName).exists()) {
            if(!searchDirectoriesList.contains(fileName))
                searchDirectoriesList << fileName;
        }
    }
    searchDirectoriesModel->setStringList(searchDirectoriesList);
    ui->lstAddedDirectories->setModel(searchDirectoriesModel);
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

void MainWindow::on_rbtUnique_toggled(bool checked)
{
    if(checked) {
        this->duplicateType = QString("unique");
    }
}

void MainWindow::on_btnSearch_clicked()
{
    foundFilesList.clear();
    foundDuplicatesList.clear();

    if (this->searchDirectoriesModel->rowCount() > 0) {
        QStringList* unfilteredFileList = this->getListOfFiles();
        QStringList* filteredFileList = filterListOfFiles(unfilteredFileList);
        excludeFromListOfFiles(filteredFileList);

        bool compareByExtension, compareByHash, compareByFilename, compareByFileSize, compareCompletely;

        compareCompletely = (this->duplicateType == "byteByByte");

        compareByHash = (this->duplicateType == "hash");
        compareByFilename = (this->duplicateType == "filename") ||
                (ui->chbHashFilename->isChecked()) ||
                (ui->chbBBFilename->isChecked()) ;

        compareByFileSize = ui->chbFNFileSize->isChecked()  || ui->chbHashFileSize->isChecked();
        compareByExtension = ui->chbBBExtension->isChecked() || ui->chbHashExtension->isChecked();

        //        QString mes = "compareByteByByte: " + QString::number(compareCompletely) +
        //                ", compareByHash: " + QString::number(compareByHash) +
        //                ", compareByFilename: " +QString::number(compareByFilename)+
        //                ", compareByFileSize: " + QString::number(compareByFileSize) +
        //                ", compareByExtension: " + QString::number(compareByExtension);
        //        qDebug() << mes;

        this->foundFilesList = *filteredFileList;

        int len = foundFilesList.count();
        for(int i = 0; i < len; i++) {
            for(int j = i + 1; j < len; j++) {
                bool addToList = true;
                QString fileName1 = foundFilesList.at(i);
                QString fileName2 = foundFilesList.at(j);
                if (fileName1 != fileName2) {
                    if (addToList && compareCompletely)
                        addToList *= compareByteByByte(fileName1, fileName2);

                    if (addToList && compareByHash)
                        addToList *= compareFileHash(fileName1, fileName2);

                    if (addToList && compareByFilename)
                        addToList *= compareFileName(fileName1, fileName2);

                    if (addToList && compareByExtension)
                        addToList *= compareFileExtension(fileName1, fileName2);

                    if (addToList && compareByFileSize)
                        addToList *= compareFileSize(fileName1, fileName2);

                    if (!foundDuplicatesList.contains(fileName1) && addToList)
                        foundDuplicatesList.append(fileName1);

                    if (!foundDuplicatesList.contains(fileName2) && addToList)
                        foundDuplicatesList.append(fileName2);
                }
            }
        }

        foundDuplicatesModel->setStringList(foundDuplicatesList);
        ui->lstvFoundDuplicates->setModel(foundDuplicatesModel);

        QString messageEnd = "";

        switch (this->foundDuplicatesList.size() % 100) {
        case 1:
            messageEnd = " дубликат";
            break;
        case 2: case 3: case 4:
            messageEnd = " дубликата";
            break;
        default:
            messageEnd = " дубликатов";
            break;
        }

        if ((this->foundDuplicatesList.size() % 100) <= 20 && (this->foundDuplicatesList.size() % 100) >= 10) {
            messageEnd = " дубликатов";
        }

        QString message = "";
        if (this->foundDuplicatesList.size()) {
            message = QString("Найдено ") + QString::number(this->foundDuplicatesList.size()) + messageEnd;
        } else {
            message = QString("Дубликатов не найдено");
        }
        QMessageBox::information(NULL, "Количество дубликатов", message);
    } else {
        QMessageBox::information(NULL, "Ошибка", "Пожалуйста, добавьте папки для поиска");
        on_btnAddPath_clicked();
    }
}

bool MainWindow::compareFileHash(QString fullFileName1, QString fullFileName2) {
    QByteArray hashOfFile1 = fileChecksum(fullFileName1, QCryptographicHash::Sha1);
    QByteArray hashOfFile2 = fileChecksum(fullFileName2, QCryptographicHash::Sha1);
    return hashOfFile1 == hashOfFile2;
}

bool MainWindow::compareByteByByte(QString fullFileName1, QString fullFileName2) {
    QByteArray hashOfFile1 = fileChecksum(fullFileName1, QCryptographicHash::Sha1);
    QByteArray hashOfFile2 = fileChecksum(fullFileName2, QCryptographicHash::Sha1);
    QByteArray data1, data2;
    if (hashOfFile1 == hashOfFile2) {
        QFile file1(fullFileName1), file2(fullFileName2);
        if (file1.open(QFile::ReadOnly | QFile::Truncate)) {
            data1 = file1.readAll();
            file1.close();
            qDebug() << QString::number(data1.size());
        }

        if (file2.open(QFile::ReadOnly | QFile::Truncate)) {
            data2 = file2.readAll();
            file2.close();
            qDebug() << QString::number(data2.size());
        }
    } else {
        return false;
    }
    return data1 == data2;
}

bool MainWindow::compareFileName(QString fullFileName1, QString fullFileName2) {
    if (fullFileName1 == fullFileName2)
        return false;
    return getOnlyFileName(fullFileName1) == getOnlyFileName(fullFileName2);
}

bool MainWindow::compareFileExtension(QString fullFileName1, QString fullFileName2) {
    return getFileExtension(fullFileName1) == getFileExtension(fullFileName2);
}

bool MainWindow::compareFileSize(QString fullFileName1, QString fullFileName2) {
    QFileInfo info1(fullFileName1), info2(fullFileName2);
    return info1.size() == info1.size();
}

QString MainWindow::getFileName(QString fullFileName) {
    int pathLength = fullFileName.lastIndexOf("/");
    return fullFileName.right(fullFileName.length() - (pathLength + 1));
}

QString MainWindow::getOnlyFileName(QString fullFileName) {
    return getFileName(fullFileName).split("\.")[0];
}

QString MainWindow::getFileExtension(QString fullFileName) {
    int extensionLength = fullFileName.length() - (fullFileName.lastIndexOf(".") + 1);
    if (fullFileName.lastIndexOf(".") != -1)
        return fullFileName.right(extensionLength);
    else
        return NULL;
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
        if(!searchProtectedDirectories.contains(fileName))
            searchProtectedDirectories << fileName;
    }
    searchProtectedDirectoriesModel->setStringList(searchProtectedDirectories);
    ui->listViewProtected->setModel(searchProtectedDirectoriesModel);
}

void MainWindow::on_btnRemovePathProtected_clicked()
{
    QModelIndex index = ui->listViewProtected->currentIndex();
    QString itemToDelete = index.data(Qt::DisplayRole).toString();
    searchProtectedDirectories.removeOne(itemToDelete);
    searchProtectedDirectoriesModel->setStringList(searchProtectedDirectories);
    ui->listViewProtected->setModel(searchProtectedDirectoriesModel);
}

void MainWindow::on_btnClearPathProtected_clicked()
{
    searchProtectedDirectories.clear();
    searchProtectedDirectoriesModel->setStringList(searchProtectedDirectories);
    ui->listViewProtected->setModel(searchProtectedDirectoriesModel);
}

void MainWindow::on_btnPastePathProtected_clicked()
{
    QClipboard* c = QApplication::clipboard();
    QString fileName = c->text();
    if(fileName != "") {
        fileName.replace("\\", "/");
        if (QFile(fileName).exists()) {
            if(!searchProtectedDirectories.contains(fileName))
                searchProtectedDirectories << fileName;
        }
    }
    searchProtectedDirectoriesModel->setStringList(searchProtectedDirectories);
    ui->listViewProtected->setModel(searchProtectedDirectoriesModel);
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

QStringList* MainWindow::getListOfFiles()
{
    QStringList* dirList = new QStringList(searchDirectoriesList);
    QStringList* result = new QStringList();

    for (int i = 0; i < dirList->size(); i++) {
        QString path = dirList->at(i);
        for (QFileInfo childFileInfo : QDir(path).entryInfoList()) {
            if (childFileInfo.isDir()) {
                QString childFileName = childFileInfo.fileName();
                if (childFileName != "." && childFileName != "..")
                    dirList->append(childFileInfo.filePath());
            } else {
                result->append(childFileInfo.canonicalFilePath());
            }
        }
    }
    return result;
}

QStringList* MainWindow::getListOfFilesInDir(QString dirPath)
{
    QStringList* listOfFilePaths = new QStringList();

    QFileInfo info(dirPath);
    if (info.isDir()) {
        for (QFileInfo childNodeInfo : QDir(dirPath).entryInfoList()) {
            if (childNodeInfo.isDir()) {
                QStringList* listOfChildFilePaths = getListOfFilesInDir(childNodeInfo.filePath());
                listOfFilePaths->append(*listOfChildFilePaths);
            } else {
                QString childNodeName = childNodeInfo.fileName();
                if (childNodeName != "." && childNodeName != "..") {
                    listOfFilePaths->append(childNodeInfo.filePath());
                }
            }
        }
    }

    return listOfFilePaths;
}

QStringList* MainWindow::filterListOfFiles(QStringList* list)
{
    QStringList* result = new QStringList();
    QString option = ui->cmbFilterMask->currentText();

    int maskNameLength = option.indexOf("|");
    QString maskName = option.left(maskNameLength - 1);
    QString regExpMask = filterTypes.find(maskName).value();
    QRegularExpression regExp(regExpMask);

    for (int i = 0; i < list->size(); i++) {
        bool addToList = true;
        QString filePath = list->at(i);
        QFileInfo info(filePath);

        QRegularExpressionMatch match = regExp.match(filePath);
        addToList *= match.hasMatch();

        if (ui->chbFilterDate->isChecked()) {
            QDateTime lastModificationTime = info.lastModified();
            bool fitInTime = (ui->dateSince->dateTime() <= lastModificationTime) &&
                    (lastModificationTime <= ui->dateUntil->dateTime());

            addToList *= fitInTime;
        }

        if (ui->chbFilterSize->isChecked()) {
            qint64 fileSize = info.size();
            qint64 scale = sizeNames.find(ui->cmbSize->currentText()).value();
            qint64 minFileSize = ui->dspbMinSize->value() * scale;
            qint64 maxFileSize = ui->dspbMaxSize->value() * scale;

            bool fitInSize = (minFileSize <= fileSize) && (fileSize <= maxFileSize);
            addToList *= fitInSize;
        }

        if (!ui->chbFilterHidden->isChecked()) {
            addToList *= !info.isHidden();
        }

        if (addToList)
            result->append(filePath);
    }
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

void MainWindow::excludeFromListOfFiles(QStringList* list)
{
    for (int i = list->size() - 1; i >= 0; i--) {
        QString path = list->at(i);
        for (QString protectedPath : searchProtectedDirectories) {
            if (path.startsWith(protectedPath))
                list->removeAll(path);
        }
    }
}

void MainWindow::on_clbSearch_clicked()
{
    on_btnSearch_clicked();
}
