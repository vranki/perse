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
        connect(&udev, SIGNAL(deviceFound(QString,QString,QString,QString,QString,QString)), this, SLOT(deviceFound(QString,QString,QString,QString,QString,QString)));
        connect(&ruleFile, SIGNAL(deviceLoaded(QString,QString,QString,QString,QString,QString)), this, SLOT(deviceFound(QString,QString,QString,QString,QString,QString)));
        udev.listDevices();
        ui->deviceTable->resizeColumnsToContents();
        ruleFile.readFile();
        connect(ui->deviceTable, SIGNAL(cellChanged(int,int)), this, SLOT(saveChanges()));
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

void MainWindow::deviceFound(QString name, QString file, QString manu, QString product, QString vid, QString pid)
{
    qDebug() << Q_FUNC_INFO << name << file << product << vid << pid;
    QString usbId = vid + ":" + pid;
    if(usbIds.contains(usbId)) return;
    usbIds.append(usbId);

    bool writable = false;


    QFileInfo fileInfo(file);
    if(fileInfo.exists()) {
        QFile::Permissions filePermissions = fileInfo.permissions();

        writable = filePermissions & QFile::WriteOther;

        qDebug() << Q_FUNC_INFO << "file perms" << file << writable;
    } else {
        qDebug() << Q_FUNC_INFO << "File doesn't exist" << file;
        writable = true;
    }


    int row = ui->deviceTable->rowCount();
    ui->deviceTable->setRowCount(row+1);

    QTableWidgetItem *item = new QTableWidgetItem();
    if(writable) {
        qDebug() << Q_FUNC_INFO << "Setting as CHECKED";
        item->setCheckState(Qt::Checked);
    } else {
        item->setCheckState(Qt::Unchecked);
    }

    ui->deviceTable->setItem(row, 0, item);
    item = new QTableWidgetItem(name);
    ui->deviceTable->setItem(row, 1, item);
    item = new QTableWidgetItem(file);
    ui->deviceTable->setItem(row, 2, item);
    item = new QTableWidgetItem(manu);
    ui->deviceTable->setItem(row, 3, item);
    item = new QTableWidgetItem(product);
    ui->deviceTable->setItem(row, 4, item);
    item = new QTableWidgetItem(usbId);
    ui->deviceTable->setItem(row, 5, item);
}

void MainWindow::saveChanges() {
    qDebug() << Q_FUNC_INFO;
    QStringList allowedIds;
    for(int row=0;row<ui->deviceTable->rowCount();row++) {
        if(ui->deviceTable->item(row, 0)->checkState() == Qt::Checked)
            allowedIds.append(ui->deviceTable->item(row, 5)->text());
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
}
