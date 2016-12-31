Odyssey
=======

A lightweight version of the Mercury/Polyphemus robot software.  
This project uses git submodules, use the following command to clone it :
```
git clone --recursive https://github.com/RICCIARDI-Adrien/Odyssey
```

See https://github.com/bibi09/odyssey for the first and more complete version.

Microcontroller
---------------

This directory contains the code for the Microchip PIC16F876 microcontroller used to control the motors and to monitor the battery voltage.
It has been compiled with the SourceBoost BoostC compiler version 7.22 (http://www.sourceboost.com).
The *Hardware* directory contains the microcontroller board schematics (edited with gEDA gschem).

PC
--

All the PC stuff is contained in this directory.

*Remote* is a Java 7 application allowing to control the robot via a WiFi connection. It was made in Java to run on a lot of platforms without modification. The project is in Eclipse 4.4 format.

Raspberry_Pi
------------

A Raspberry Pi Model A handles the robot's camera (a RaspiCam) and creates a WiFi access point for a PC to connect to it.

The whole Raspberry Pi firmware is located in this directory. It is based on the OpenWRT Linux distribution.

To compile it, simply type *make* in the *Raspberry_Pi* directory. The resulting SD-card image will be located in the *OpenWRT/bin/brcm2708/openwrt-brcm2708-bcm2708-rpi-ext4-sdcard.img* file.

You can burn it on a SD-card the same way than any other image is burned.
