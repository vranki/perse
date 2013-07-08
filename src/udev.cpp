#include "udev.h"
#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <QDebug>

UDev::UDev(QObject *parent) :
    QObject(parent)
{
}

// Almost copypaste from http://www.signal11.us/oss/udev/
void UDev::listDevices()
{
    struct udev *udev = udev_new();
    if (!udev) {
        qDebug() << Q_FUNC_INFO << "Can't create udev!";
        return;
    }

    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "usb");
    udev_enumerate_scan_devices(enumerate);
    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *dev_list_entry;
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;
        path = udev_list_entry_get_name(dev_list_entry);
        struct udev_device *dev;
        dev = udev_device_new_from_syspath(udev, path);

        struct udev_device *parentDev;
        parentDev = udev_device_get_parent_with_subsystem_devtype(
                    dev,
                    "usb",
                    "usb_device");
        if (parentDev) {
            QString name, file, manu, product, vid, pid;
            name = QString::fromUtf8(udev_device_get_sysattr_value(parentDev,"product"));
            file = QString::fromUtf8(udev_device_get_devnode(parentDev));
            vid = udev_device_get_sysattr_value(parentDev, "idVendor");
            pid = udev_device_get_sysattr_value(parentDev, "idProduct");
            manu = udev_device_get_sysattr_value(parentDev, "manufacturer");
            QString usbId = vid + ":" + pid;
            emit deviceFound(name, file, manu, usbId);
            udev_device_unref(parentDev);
        }
    }
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}
