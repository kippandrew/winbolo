This is an prototype/alpha version of JBolo. It is not remotely production ready.
Or read this as: This is experimental and does not work correctly.


Requirements
============
* Java 1.5
* Currently paths are hardcoded to c:\jbolo The following files need to be placed in that directory
	* map.map
	* tiles.bmp
	* background.bmp
 	* jbolo.ini
	* subdirectory "sounds" with wav files.

Running
=======
java JBolo
will start a single player test game loading "map.map"

java JBolo -test
will attempt to join a 1.14 game (hardcoded location) and listen for a while for new packets.

java JBolo -skin
will start the skin load test code.

java jbolo.com.jbolo.gui.dialog.gamefinder.GameFinderDialog
will start the tracker game finder window test code. Can connect to the tracker and download games list.

There are a few other main functions for testing other dialogs.


Notes and History
=================

Keys are hardcoded to:
	* Q - acelerate
	* A - decelerate
	* O - turn left
	* P - turn right
	* space - shoot
	* shift - lay mine
	* Arrow keys - Move screen


Most of the code was written in 2003-2004. Its original target platform was java 1.1 and was to support
running inside the Microsoft Java VM so users would not need to download a seperate JRE to play.
As such it currently uses AWT for graphics. More recently some java 1.5 features (generics) were used.

Dialogs were created using NetBeans 3.5.1 Form files included.

It is almost a file by file direct port of the C code, designed to be fully compatible with WinBolo.
The GameClient interface is the equivilent of backend.h. There are a few classes heirachies to reduce
duplicate code e.g. Pillbox extends GameItem, swamp.c and rubble.c are replaced by ChangableTerrain.java
and floodfill.c and minesexp.c are replaced by TimedTerrain.java.

