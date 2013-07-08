#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QMessageBox>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(ruleFile.canWriteFile()) {
        connect(&udev, SIGNAL(deviceFound(QString,QString,QString,QString)), this, SLOT(deviceFound(QString,QString,QString,QString)));
        connect(&ruleFile, SIGNAL(deviceHasRule(QString)), this, SLOT(deviceHasRule(QString)));
        updateTable();
        ui->deviceTable->resizeColumnsToContents();
        updateTimer.setSingleShot(false);
        updateTimer.setInterval(2000);
        connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateTable()));
        updateTimer.start();
    } else {
        QMessageBox *mb = new QMessageBox(this);
        mb->setModal(true);
        mb->setText("Unable to open rule file. Make sure you run this application as root.");
        mb->exec();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::canRun()
{
    return ruleFile.canWriteFile();
}

void MainWindow::deviceFound(QString name, QString file, QString manu, QString usbId)
{
    qDebug() << Q_FUNC_INFO << name << file << usbId;
    if(usbIds.contains(usbId)) return;
    usbIds.append(usbId);

    bool exists = false;
    bool writable = false;
    bool readable = false;


    QFileInfo fileInfo(file);
    if(fileInfo.exists()) {
        QFile::Permissions filePermissions = fileInfo.permissions();

        writable = filePermissions & QFile::WriteOther;
        readable = filePermissions & QFile::ReadOther;
        exists = true;
    } else {
        qDebug() << Q_FUNC_INFO << "File doesn't exist" << file;
        readable = true;
        writable = true;
    }

    int row = ui->deviceTable->rowCount();
    ui->deviceTable->setRowCount(row+1);

    QTableWidgetItem *item = new QTableWidgetItem();
    item->setCheckState(Qt::Unchecked);
    item->setTextAlignment(Qt::AlignHCenter);
    ui->deviceTable->setItem(row, 0, item);
    QString statusString = "";
    if(exists) {
        if(readable) statusString += "R";
        if(writable) statusString += "W";
    } else {
        statusString = "Not present";
    }
    item = new QTableWidgetItem(statusString);
    ui->deviceTable->setItem(row, 1, item);
    item = new QTableWidgetItem(name);
    ui->deviceTable->setItem(row, 2, item);
    item = new QTableWidgetItem(manu);
    ui->deviceTable->setItem(row, 3, item);
    item = new QTableWidgetItem(usbId);
    ui->deviceTable->setItem(row, 4, item);
}

void MainWindow::deviceHasRule(QString usbId)
{
    qDebug() << Q_FUNC_INFO << usbId;
    bool deviceInList = false;
    for(int row=0;row<ui->deviceTable->rowCount();row++) {
        if(ui->deviceTable->item(row, 4)->text() == usbId) {
            ui->deviceTable->item(row, 0)->setCheckState(Qt::Checked);
            deviceInList = true;
        }
    }
    if(!deviceInList) {
        deviceFound("", "", "", usbId);
        deviceHasRule(usbId);
    }
}

void MainWindow::saveChanges() {
    QStringList allowedIds;
    for(int row=0;row<ui->deviceTable->rowCount();row++) {
        if(ui->deviceTable->item(row, 0)->checkState() == Qt::Checked)
            allowedIds.append(ui->deviceTable->item(row, 4)->text());
    }
    ruleFile.writeFile(allowedIds);

    // Reload udev rules with "udevadm control --reload-rules"

    QStringList args;
    args << "control" << "--reload-rules";
    QProcess udevadm;
    udevadm.start("udevadm", args);
    udevadm.waitForFinished();
    if(udevadm.exitCode() != 0) {
        QMessageBox *mb = new QMessageBox(this);
        mb->setModal(true);
        mb->setText("Running udevadm failed. Make sure you run this application as root.");
        mb->exec();
    } else {
        // Run "udevadm trigger" to apply changes
        args.clear();
        args << "trigger";
        udevadm.start("udevadm", args);
        udevadm.waitForFinished();
    }
    updateTable();
}

void MainWindow::updateTable()
{
    disconnect(ui->deviceTable, SIGNAL(cellChanged(int,int)), this, SLOT(saveChanges()));
    while(ui->deviceTable->rowCount()) {
        ui->deviceTable->itemAt(0,0)->setCheckState(Qt::Unchecked);
        ui->deviceTable->removeRow(0);
    }
    usbIds.clear();
    udev.listDevices();
    ruleFile.readFile();
    connect(ui->deviceTable, SIGNAL(cellChanged(int,int)), this, SLOT(saveChanges()));
}
