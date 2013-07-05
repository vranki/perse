#ifndef UDEV_H
#define UDEV_H

#include <QObject>

/**
 * Handles listing devices from udev api
 */
class UDev : public QObject
{
    Q_OBJECT
public:
    explicit UDev(QObject *parent = 0);
signals:
    void deviceFound(QString name, QString file, QString manu, QString product, QString vid, QString pid);
public slots:
    void listDevices();
};

#endif // UDEV_H
