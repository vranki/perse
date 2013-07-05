Perse
=====

Permission Settings manager GUI for Linux UDev.

Got a USB gadget which won't work unless you chmod it's device file to be writable?

Hate writing udev rules for gadgets?

Perse is a simple GUI tool to make devices writable in Linux. Just check the devices
you want to be world writable and Perse creates the udev rules.



Still WIP but usable.

run "sudo touch /etc/udev/rules.d/50-perse.rules" before first run. This will
be created by deb/rpm packaging later.

Run as root. 

GNU GPLv3, (c) Ville Ranki <ville.ranki@iki.fi>

