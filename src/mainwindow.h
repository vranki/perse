#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QList>
#include <QStringList>
#include <QTimer>
#include "udev.h"
#include "rulefile.h"

namespace Ui {
class MainWindow;
}

/**
 * God class and UI main window
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool canRun();
private slots:
    void deviceFound(QString name, QString file, QString manu, QString usbId);
    void deviceHasRule(QString usbId);
    void saveChanges();
    void updateTable();
private:

    Ui::MainWindow *ui;
    UDev udev;
    QList<QTableWidgetItem*> tableItems;
    QStringList usbIds;
    RuleFile ruleFile;
    QTimer updateTimer;
};

#endif // MAINWINDOW_H
