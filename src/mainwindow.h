#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QList>
#include <QStringList>
#include "udev.h"
#include "rulefile.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool canRun();
private slots:
    void deviceFound(QString name, QString file, QString manu, QString product, QString vid, QString pid);
    void saveChanges();
private:
    Ui::MainWindow *ui;
    UDev udev;
    QList<QTableWidgetItem*> tableItems;
    QStringList usbIds;
    RuleFile ruleFile;
};

#endif // MAINWINDOW_H
