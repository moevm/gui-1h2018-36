#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QStringListModel"
#include "QClipboard"
#include "QMessageBox"
#include "QCryptographicHash"
#include "QStandardItem"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    modelAdded = new QStringListModel();
    modelProtectedFiles = new QStringListModel();
    foundDuplicatesModel = new QStringListModel();

    ui->grpByteByByte->hide();
    ui->grpFilename->hide();
    ui->grpHash->show();

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAddPath_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);

    QStringList selectedFileNames;
    if (dialog.exec())
        selectedFileNames = dialog.selectedFiles();

    for(int i = 0; i < selectedFileNames.length(); i++){
        QString fileName = selectedFileNames.at(i);
        if(!addedFileNames.contains(fileName))
            addedFileNames << fileName;
    }
    modelAdded->setStringList(addedFileNames);
    ui->listView->setModel(modelAdded);
}

void MainWindow::on_btnRemovePath_clicked()
{
    QModelIndex index = ui->listView->currentIndex();
    QString itemToDelete = index.data(Qt::DisplayRole).toString();
    addedFileNames.removeOne(itemToDelete);
    modelAdded->setStringList(addedFileNames);
    ui->listView->setModel(modelAdded);
}

void MainWindow::on_btnClearPath_clicked()
{
    addedFileNames.clear();
    modelAdded->setStringList(addedFileNames);
    ui->listView->setModel(modelAdded);
}

void MainWindow::on_btnPastePath_clicked()
{
    QClipboard* c = QApplication::clipboard();
    QString fileName = c->text();
    if(fileName != "") {
        fileName.replace("\\", "/");
        if (QFile(fileName).exists()) {
            if(!addedFileNames.contains(fileName))
                addedFileNames << fileName;
        }
    }
    modelAdded->setStringList(addedFileNames);
    ui->listView->setModel(modelAdded);
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
    foundFilesList.clear();
    foundDuplicatesList.clear();

    //    this->foundFilesList = *this->getListOfFiles();
    QStringList* unfilteredFileList = this->getListOfFiles();
    QStringList* filteredFileList = filterListOfFiles(unfilteredFileList);

    this->foundFilesList = *filteredFileList;

    if (this->modelAdded->rowCount() > 0) {

        for (QString filename1 : foundFilesList) {
            for (QString filename2 : foundFilesList) {
                if (filename1 != filename2){
                    if (this->duplicateType == "hash") {
                        QByteArray hashOfFile1 = fileChecksum(filename1, QCryptographicHash::Sha1);
                        QByteArray hashOfFile2 = fileChecksum(filename2, QCryptographicHash::Sha1);

                        if (hashOfFile1 == hashOfFile2) {
                            if (!foundDuplicatesList.contains(filename1)) {
                                foundDuplicatesList.append(filename1);
                            }
                        }
                    } else if (this->duplicateType == "filename") {
                        int fileNameLength1 = filename1.lastIndexOf("/");
                        int fileNameLength2 = filename2.lastIndexOf("/");
                        QString fileName1 = filename1.right(filename1.length() - (fileNameLength1 + 1));
                        QString fileName2 = filename2.right(filename2.length() - (fileNameLength2 + 1));

                        if (fileName1 == fileName2) {
                            if (!foundDuplicatesList.contains(filename1)) {
                                foundDuplicatesList.append(filename1);
                            }
                        }
                    }
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
        if(!addedFileNamesProtected.contains(fileName))
            addedFileNamesProtected << fileName;
    }
    modelProtectedFiles->setStringList(addedFileNamesProtected);
    ui->listViewProtected->setModel(modelProtectedFiles);
}

void MainWindow::on_btnRemovePathProtected_clicked()
{
    QModelIndex index = ui->listViewProtected->currentIndex();
    QString itemToDelete = index.data(Qt::DisplayRole).toString();
    addedFileNamesProtected.removeOne(itemToDelete);
    modelProtectedFiles->setStringList(addedFileNamesProtected);
    ui->listViewProtected->setModel(modelProtectedFiles);
}

void MainWindow::on_btnClearPathProtected_clicked()
{
    addedFileNamesProtected.clear();
    modelProtectedFiles->setStringList(addedFileNamesProtected);
    ui->listViewProtected->setModel(modelProtectedFiles);
}

void MainWindow::on_btnPastePathProtected_clicked()
{
    QClipboard* c = QApplication::clipboard();
    QString fileName = c->text();
    if(fileName != "") {
        fileName.replace("\\", "/");
        if (QFile(fileName).exists()) {
            if(!addedFileNamesProtected.contains(fileName))
                addedFileNamesProtected << fileName;
        }
    }
    modelProtectedFiles->setStringList(addedFileNamesProtected);
    ui->listViewProtected->setModel(modelProtectedFiles);
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
    QStringList* result = new QStringList();
    result->append(addedFileNames);

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
        QString filePath = list->at(i);

        QRegularExpressionMatch match = regExp.match(filePath);

        if (match.hasMatch()) {
            result->append(filePath);
        }
    }
    return result;
}

//QStringList* MainWindow::excludeFromListOfFiles(QStringList* list)
//{
//    QStandardItemModel* model = new QStandardItemModel();
//    QStandardItem* item = new QStandardItem();
//    item->setCheckable(true);
//}

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

void MainWindow::on_clbSearch_clicked()
{
    on_btnSearch_clicked();
}
