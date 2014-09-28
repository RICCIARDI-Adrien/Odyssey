Odyssey
=======

A lightweight version of the Mercury/Polyphemus robot software.

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

A Raspberry Pi Model A handles the robot's camera (a RaspiCam) and connects to a PC via WiFi.
The code has been compiled on the Raspberry Pi using GCC.

*Manual_Control* program allows to test all the robot hardware and to set the motor various speeds.
*Network_Control* is the server to which the PC Remote application will connect. It controls the motors and monitors the robot parameters.
