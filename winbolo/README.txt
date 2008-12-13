This directory contains the WinBolo/LinBolo source, server source code and Log viewer source code.

The docs directory contains the documentation from the official distribution.



Windows Requirements
Windows 95,98,NT4 (SP5+) ,Windows 2000, Windows ME, Windows XP, Vista, Windows 2003
DirectX 5+
Windows MM libraries (included in windows)
Winsock 2 (included from Windows 98 onwards, downloadable for Windows 95)


Windows Build Instructions
No project fles provided.
Client requires everything. (exclude linux directory and logviewer directory) Build as an applicaiton.
Server requires everything except gui and logviwer directory. Build as console application
Logviwer requires src/logviwer and src/zlib directories.



Linux Server Requirements
Linux x86
Linux 2.2+
SDL 1.2**

To build type: make in src/server


Linux Client Requirements
Linux x86
Linux 2.2+
X Server (XFree 3.3.6+) capable of 16 bit colour
8Mb RAM
Glibc 2.0 (2.1+ recommended)
GTK 1.2*
SDL 1.1 and 1.2**
SDL_Mixer 1.1 ***
SDL_TTF 1.2 ***
*GTK can be downloaded from:  http://www.gtk.org/
**SDL can be downloaded from: http://www.libsdl.org/
*** SDL Libraries can be downloaded from: http://www.libsdl.org/libraries.html

To build type:
type make in src/gui/linux

Note: The linux client currently does not build. It was last updated for version 1.13.

zlib provided under the zlib license. See www.zlib.org or src/zlib/zlib.h for more details.

