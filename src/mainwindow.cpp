#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QMessageBox>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , currentListRow(0)
{
    ui->setupUi(this);
    ui->deviceTable->verticalHeader()->setVisible(false);
    ui->deviceTable->horizontalHeader()->setStretchLastSection(true);
    if(!ruleFile.canWriteFile()) {
        QMessageBox mb(this);
        mb.setModal(true);
        mb.setWindowTitle("Perse warning");
        mb.setText("Unable to open rule file");
        mb.setInformativeText("Run this application as root if you want to change permissions.");
        mb.exec();
    }
    connect(&udev, &UDev::deviceFound, this, &MainWindow::deviceFound);
    connect(&ruleFile, &RuleFile::deviceHasRule, this, &MainWindow::deviceHasRule);
    updateTable();
    ui->deviceTable->resizeColumnsToContents();
    updateTimer.setSingleShot(false);
    updateTimer.setInterval(2000);
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateTable()));
    updateTimer.start();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::deviceFound(QString name,
                             QString file,
                             QString manu,
                             QString usbId) {
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

    if(ui->deviceTable->rowCount() < currentListRow + 1)
        ui->deviceTable->insertRow(currentListRow);

    QTableWidgetItem *item = new QTableWidgetItem();
    item->setCheckState(Qt::Unchecked);
    item->setTextAlignment(Qt::AlignHCenter);
    ui->deviceTable->setItem(currentListRow, 0, item);
    QString statusString = "";
    if(exists) {
        if(readable) statusString += "R";
        if(writable) statusString += "W";
    } else {
        statusString = "Not present";
    }
    item = new QTableWidgetItem(statusString);
    ui->deviceTable->setItem(currentListRow, 1, item);
    item = new QTableWidgetItem(name);
    ui->deviceTable->setItem(currentListRow, 2, item);
    item = new QTableWidgetItem(manu);
    ui->deviceTable->setItem(currentListRow, 3, item);
    item = new QTableWidgetItem(usbId);
    ui->deviceTable->setItem(currentListRow, 4, item);

    currentListRow++;
}

void MainWindow::deviceHasRule(QString usbId) {
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
    if(udevadm.exitCode()) {
        QMessageBox mb(this);
        mb.setModal(true);
        mb.setText("Running udevadm command failed");
        mb.setInformativeText("Make sure you run this application as root.");
        mb.exec();
    } else {
        // Run "udevadm trigger" to apply changes
        args.clear();
        args << "trigger";
        udevadm.start("udevadm", args);
        udevadm.waitForFinished();
    }
    updateTable();
}

void MainWindow::updateTable() {
    disconnect(ui->deviceTable, &QTableWidget::cellChanged, this, &MainWindow::saveChanges);
    currentListRow = 0;
    usbIds.clear();
    udev.listDevices();
    while(ui->deviceTable->rowCount() > currentListRow)
        ui->deviceTable->removeRow(currentListRow);
    ruleFile.readFile();
    connect(ui->deviceTable, &QTableWidget::cellChanged, this, &MainWindow::saveChanges);
}
