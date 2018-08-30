Perse
=====

Permission Settings manager GUI for Linux UDev.

Got a USB gadget which won't work unless you chmod it's device file to be writable, such
as Arduino or USB-serial adapter?

Hate writing udev rules for those gadgets?

Perse is a easy GUI tool to make devices writable in Linux. Just select the devices
you want to be world writable and Perse creates persistent udev rules for them.

Ubuntu packages available at https://launchpad.net/~ville-ranki/+archive/misc

To build and install debian package (checkout debian branch!):

 * git clone https://github.com/vranki/perse.git -b debian
 * cd perse
 * debuild -us -uc -b
 * sudo dpkg -i ../perse*.deb

You can now launch Perse from the application launcher.

To compile manually:

 * Install libudev-dev and Qt dev tools
 * qmake
 * make

GNU GPLv3, (c) Ville Ranki <ville.ranki@iki.fi>
