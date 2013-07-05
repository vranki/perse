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

void UDev::listDevices()
{

    /* Create the udev object */
    struct udev *udev = udev_new();
    if (!udev) {
        printf("Can't create udev\n");
        return;
    }

    /* Create a list of the devices in the 'hidraw' subsystem. */
    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "usb");
    udev_enumerate_scan_devices(enumerate);
    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    /* For each item enumerated, print out its information.
           udev_list_entry_foreach is a macro which expands to
           a loop. The loop will be executed for each member in
           devices, setting dev_list_entry to a list entry
           which contains the device's path in /sys. */
    struct udev_list_entry *dev_list_entry;
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;

        /* Get the filename of the /sys entry for the device
               and create a udev_device object (dev) representing it */
        path = udev_list_entry_get_name(dev_list_entry);
        struct udev_device *dev;
        dev = udev_device_new_from_syspath(udev, path);


        /* The device pointed to by dev contains information about
               the hidraw device. In order to get information about the
               USB device, get the parent device with the
               subsystem/devtype pair of "usb"/"usb_device". This will
               be several levels up the tree, but the function will find
               it.*/
        struct udev_device *parentDev;
        parentDev = udev_device_get_parent_with_subsystem_devtype(
                    dev,
                    "usb",
                    "usb_device");
        if (parentDev) {
            /* usb_device_get_devnode() returns the path to the device node
                   itself in /dev. */
            printf("Device Node Path: %s\n", udev_device_get_devnode(parentDev));
            /* From here, we can call get_sysattr_value() for each file
               in the device's /sys entry. The strings passed into these
               functions (idProduct, idVendor, serial, etc.) correspond
               directly to the files in the directory which represents
               the USB device. Note that USB strings are Unicode, UCS2
               encoded, but the strings returned from
               udev_device_get_sysattr_value() are UTF-8 encoded. */
            /*
            printf("  VID/PID: %s %s\n",
                   udev_device_get_sysattr_value(parentDev,"idVendor"),
                   udev_device_get_sysattr_value(parentDev, "idProduct"));
            printf("  %s\n  %s\n",
                   udev_device_get_sysattr_value(parentDev,"manufacturer"),
                   udev_device_get_sysattr_value(parentDev,"product"));
            printf("  serial: %s\n",
                   udev_device_get_sysattr_value(parentDev, "serial"));
            printf("  subsys: %s\n",
                    udev_device_get_subsystem(parentDev));
            printf("  type: %s\n",
                    udev_device_get_devtype(parentDev));
*/
            QString name, file, manu, product, vid, pid;
            name = QString::fromUtf8(udev_device_get_sysattr_value(parentDev,"product"));
            file = QString::fromUtf8(udev_device_get_devnode(parentDev));
            vid = udev_device_get_sysattr_value(parentDev, "idVendor");
            pid = udev_device_get_sysattr_value(parentDev, "idProduct");
            manu = udev_device_get_sysattr_value(parentDev, "manufacturer");
            product = udev_device_get_sysattr_value(parentDev, "product");
            emit deviceFound(name, file, manu, product, vid, pid);
            udev_device_unref(parentDev);
        }
    }
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}
