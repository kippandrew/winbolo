#
# $Id$
#
# Copyright (c) 1998-2008 John Morrison.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.


CC = gcc
CFLAGS = -g `sdl-config --cflags` `gtk-config --cflags` 
#-s
#CFLAGS = -s -O3 -ffast-math -fomit-frame-pointer -mpentium -march=pentium `sdl-config --cflags` `gtk-config --cflags` `glib-config --cflags` 
BOLODIR = ../../bolo/
GUIDIR = ../
WBNETDIR = ../../winbolonet/
GUIDIRLINUX = ./
SERVERDIR = ../../server
COMPRESSDIR = ../../lzw/
APPNAME = linbolo

all: linbolo

clean:
	rm *.o
	rm linbolo
	

linbolo:    global.o bases.o util.o crc.o pillbox.o lgm.o shells.o tank.o \
	allience.o bolo_map.o building.o explosions.o floodfill.o \
	gametype.o grass.o labels.o messages.o mines.o minesexp.o \
	netmt.o netpnb.o players.o rubble.o \
	screenbrainmap.o screenbullet.o screencalc.o screenlgm.o \
	screentank.o scroll.o sounddist.o starts.o swamp.o \
	tankexp.o treegrow.o playersrejoin.o network.o screen.o \
	main.o draw.o lang.o netclient.o clientmutex.o framemutex.o \
	dcodlzw.o ecodlzw.o input.o cursor.o sound.o preferences.o \
	dialogsysteminfo.o dialognetworkinfo.o dialoggameinfo.o \
	dialogmessages.o dialogopening.o dialoggamesetup.o \
	dialogudpsetup.o dialogtrackersetup.o dialogabout.o winbolonetthread.o \
	messagebox.o dialogpassword.o dialogsetname.o dialogalliance.o \
	currentgames.o dialoggamefinder.o brainshandler.o dialogkeysetup.o \
	log.o winbolonet.o udppackets.o winbolonetevents.o serverfrontend.o \
	http.o dnslookups.o dialogwinbolonet.o
	${CC} -o ${APPNAME} `sdl-config --libs` `gtk-config --libs` ${CFLAGS} \
	-lSDL_ttf -lSDL_mixer `glib-config --libs gthread` \
	-L -L/usr/X11/lib -lX11 \
	global.o bases.o util.o crc.o pillbox.o lgm.o \
	shells.o tank.o allience.o bolo_map.o building.o \
	explosions.o floodfill.o gametype.o grass.o labels.o \
	messages.o mines.o minesexp.o  netmt.o \
	netpnb.o players.o rubble.o screenbrainmap.o \
	screenbullet.o screencalc.o screenlgm.o screentank.o \
	scroll.o sounddist.o starts.o swamp.o tankexp.o \
	treegrow.o playersrejoin.o network.o screen.o \
	lang.o main.o draw.o netclient.o clientmutex.o framemutex.o \
	dcodlzw.o ecodlzw.o input.o cursor.o sound.o \
	dialogsysteminfo.o dialognetworkinfo.o dialoggameinfo.o \
	dialogmessages.o dialogopening.o dialoggamesetup.o winbolonetthread.o \
	dialogudpsetup.o dialogtrackersetup.o dialogabout.o preferences.o \
	messagebox.o dialogpassword.o dialogsetname.o dialogalliance.o \
	currentgames.o dialoggamefinder.o brainshandler.o dialogkeysetup.o \
	log.o http.o winbolonet.o winbolonetevents.o udppackets.o \
	serverfrontend.o dnslookups.o dialogwinbolonet.o

winbolonet.o: ${WBNETDIR}winbolonet.c ${WBNETDIR}winbolonet.h
	${CC} ${CFLAGS} -c ${WBNETDIR}winbolonet.c

winbolonetthread.o: ${WBNETDIR}winbolonetthread.c ${WBNETDIR}winbolonetthread.h
	${CC} ${CFLAGS} -c ${WBNETDIR}winbolonetthread.c

http.o: ${WBNETDIR}http.c ${WBNETDIR}http.h
	${CC} ${CFLAGS} -c ${WBNETDIR}http.c

winbolonetevents.o: ${WBNETDIR}winbolonetevents.c ${WBNETDIR}winbolonetevents.h
	${CC} ${CFLAGS} -c ${WBNETDIR}winbolonetevents.c

main.o: main.c 
	${CC} ${CFLAGS} -c main.c

dialogkeysetup.o: dialogkeysetup.c dialogkeysetup.h 
	${CC} ${CFLAGS} -c dialogkeysetup.c

dnslookups.o: dnslookups.c 
	${CC} ${CFLAGS} -c dnslookups.c

brainshandler.o: brainshandler.c
	${CC} ${CFLAGS} -c brainshandler.c

preferences.o: preferences.c preferences.h
	${CC} ${CFLAGS} -c preferences.c

currentgames.o: ../win32/currentgames.c ../currentgames.h
	${CC} ${CFLAGS} -c ../win32/currentgames.c

dialogalliance.o: dialogalliance.c dialogalliance.h
	${CC} ${CFLAGS} -c dialogalliance.c

dialoggamefinder.o: dialoggamefinder.c dialoggamefinder.h
	${CC} ${CFLAGS} -c dialoggamefinder.c
	
dialogpassword.o: dialogpassword.c
	${CC} ${CFLAGS} -c dialogpassword.c

dialogsetname.o: dialogsetname.c dialogsetname.h
	${CC} ${CFLAGS} -c dialogsetname.c

dialoggamesetup.o: dialoggamesetup.c dialoggamesetup.h
	${CC} ${CFLAGS} -c dialoggamesetup.c

dialogtrackersetup.o: dialogtrackersetup.c dialogtrackersetup.h
	${CC} ${CFLAGS} -c dialogtrackersetup.c

dialogudpsetup.o: dialogudpsetup.c
	${CC} ${CFLAGS} -c dialogudpsetup.c

messagebox.o: messagebox.c messagebox.h
	${CC} ${CFLAGS} -c messagebox.c

dialogsysteminfo.o: dialogsysteminfo.c dialogsysteminfo.h
	${CC} ${CFLAGS} -c dialogsysteminfo.c

dialogabout.o: dialogabout.c dialogabout.h
	${CC} ${CFLAGS} -c dialogabout.c

dialogopening.o: dialogopening.c
	${CC} ${CFLAGS} -c dialogopening.c

dialogmessages.o: dialogmessages.c dialogmessages.h
	${CC} ${CFLAGS} -c dialogmessages.c

dialognetworkinfo.o: dialognetworkinfo.c dialognetworkinfo.h
	${CC} ${CFLAGS} -c dialognetworkinfo.c

dialogwinbolonet.o: dialogwinbolonet.c dialogwinbolonet.h
	${CC} ${CFLAGS} -c dialogwinbolonet.c

dialoggameinfo.o: dialoggameinfo.c dialoggameinfo.h
	${CC} ${CFLAGS} -c dialoggameinfo.c

draw.o: draw.c draw.h
	${CC} ${CFLAGS} -c draw.c

input.o: input.c input.h
	${CC} ${CFLAGS} -c input.c

sound.o: sound.c sound.h
	${CC} ${CFLAGS} -c sound.c

cursor.o: cursor.c cursor.h
	${CC} ${CFLAGS} -c cursor.c

lang.o: ${GUIDIRLINUX}lang.c
	${CC} ${CFLGS} -c ${GUIDIRLINUX}lang.c

netclient.o: netclient.c
	${CC} ${CFLAGS} -c netclient.c
dcodlzw.o: ${COMPRESSDIR}dcodlzw.c
	${CC} ${CFLAGS} -c ${COMPRESSDIR}dcodlzw.c

ecodlzw.o: ${COMPRESSDIR}ecodlzw.c
	${CC} ${CFLAGS} -c ${COMPRESSDIR}ecodlzw.c

framemutex.o: framemutex.c framemutex.h
	${CC} ${CFLAGS} -c framemutex.c

clientmutex.o: clientmutex.c ${GUIDIR}clientmutex.h
	${CC} ${CFLAGS} -c clientmutex.c
global.o: ${BOLODIR}global.c ${BOLODIR}global.h
	${CC} ${CFLAGS} -c ${BOLODIR}global.c

util.o:	${BOLODIR}util.c ${BOLODIR}util.h
	${CC} ${CFLAGS} -c ${BOLODIR}util.c

crc.o: ${BOLODIR}crc.c ${BOLODIR}crc.h
	${CC} ${CFLAGS} -c ${BOLODIR}crc.c

log.o: ${BOLODIR}log.c ${BOLODIR}log.h
	${CC} ${CFLAGS} -c ${BOLODIR}log.c

udppackets.o: ${BOLODIR}udppackets.c ${BOLODIR}udppackets.h
	${CC} ${CFLAGS} -c ${BOLODIR}udppackets.c

bases.o: ${BOLODIR}bases.h ${BOLODIR}bases.c
	${CC} ${CFLAGS} -c ${BOLODIR}bases.c
serverfrontend.o: ${BOLODIR}serverfrontend.c
	${CC} ${CFLAGS} -c ${BOLODIR}serverfrontend.c

lgm.o: ${BOLODIR}lgm.c ${BOLODIR}lgm.h
	${CC} ${CFLAGS} -c ${BOLODIR}lgm.c

pillbox.o: ${BOLODIR}pillbox.c ${BOLODIR}pillbox.h
	${CC} ${CFLAGS} -c ${BOLODIR}pillbox.c

shells.o: ${BOLODIR}shells.c ${BOLODIR}shells.h
	${CC} ${CFLAGS} -c ${BOLODIR}shells.c

tank.o: ${BOLODIR}tank.c ${BOLODIR}tank.h
	${CC} ${CFLAGS} -c ${BOLODIR}tank.c

allience.o: ${BOLODIR}allience.c ${BOLODIR}allience.h
	${CC} ${CFLAGS} -c ${BOLODIR}allience.c

bolo_map.o: ${BOLODIR}bolo_map.c ${BOLODIR}bolo_map.h
	${CC} ${CFLAGS} -c ${BOLODIR}bolo_map.c

building.o: ${BOLODIR}building.c ${BOLODIR}building.h
	${CC} ${CFLAGS} -c ${BOLODIR}building.c

explosions.o: ${BOLODIR}explosions.c ${BOLODIR}explosions.h
	${CC} ${CFLAGS} -c ${BOLODIR}explosions.c

floodfill.o: ${BOLODIR}floodfill.c ${BOLODIR}floodfill.h
	${CC} ${CFLAGS} -c ${BOLODIR}floodfill.c

gametype.o: ${BOLODIR}gametype.c ${BOLODIR}gametype.h
	${CC} ${CFLAGS} -c ${BOLODIR}gametype.c

grass.o: ${BOLODIR}grass.c ${BOLODIR}grass.h
	${CC} ${CFLAGS} -c ${BOLODIR}grass.c

labels.o: ${BOLODIR}labels.c ${BOLODIR}labels.h
	${CC} ${CFLAGS} -c ${BOLODIR}labels.c

messages.o: ${BOLODIR}messages.c ${BOLODIR}messages.h
	${CC} ${CFLAGS} -c ${BOLODIR}messages.c
	
mines.o: ${BOLODIR}mines.c ${BOLODIR}mines.h
	${CC} ${CFLAGS} -c ${BOLODIR}mines.c

minesexp.o: ${BOLODIR}minesexp.c ${BOLODIR}minesexp.h
	${CC} ${CFLAGS} -c ${BOLODIR}minesexp.c

netmt.o: ${BOLODIR}netmt.c ${BOLODIR}netmt.h
	${CC} ${CFLAGS} -c ${BOLODIR}netmt.c
	
netpnb.o: ${BOLODIR}netpnb.c ${BOLODIR}netpnb.h
	${CC} ${CFLAGS} -c ${BOLODIR}netpnb.c

players.o: ${BOLODIR}players.c ${BOLODIR}players.h
	${CC} ${CFLAGS} -c ${BOLODIR}players.c
	
rubble.o: ${BOLODIR}rubble.c ${BOLODIR}rubble.h
	${CC} ${CFLAGS} -c ${BOLODIR}rubble.c
	
screenbrainmap.o: ${BOLODIR}screenbrainmap.c ${BOLODIR}screenbrainmap.h
	${CC} ${CFLAGS} -c ${BOLODIR}screenbrainmap.c
	
screenbullet.o: ${BOLODIR}screenbullet.c ${BOLODIR}screenbullet.h
	${CC} ${CFLAGS} -c ${BOLODIR}screenbullet.c
	
screencalc.o: ${BOLODIR}screencalc.c ${BOLODIR}screencalc.h
	${CC} ${CFLAGS} -c ${BOLODIR}screencalc.c
	
screenlgm.o: ${BOLODIR}screenlgm.c ${BOLODIR}screenlgm.h
	${CC} ${CFLAGS} -c ${BOLODIR}screenlgm.c

screentank.o: ${BOLODIR}screentank.c ${BOLODIR}screentank.h
	${CC} ${CFLAGS} -c ${BOLODIR}screentank.c

scroll.o: ${BOLODIR}scroll.c ${BOLODIR}scroll.h
	${CC} ${CFLAGS} -c ${BOLODIR}scroll.c
	
sounddist.o: ${BOLODIR}sounddist.c ${BOLODIR}sounddist.h
	${CC} ${CFLAGS} -c ${BOLODIR}sounddist.c

starts.o: ${BOLODIR}starts.c ${BOLODIR}starts.h
	${CC} ${CFLAGS} -c ${BOLODIR}starts.c

swamp.o: ${BOLODIR}swamp.c ${BOLODIR}swamp.h
	${CC} ${CFLAGS} -c ${BOLODIR}swamp.c

tankexp.o: ${BOLODIR}tankexp.c ${BOLODIR}tankexp.h
	${CC} ${CFLAGS} -c ${BOLODIR}tankexp.c

treegrow.o: ${BOLODIR}treegrow.c ${BOLODIR}treegrow.h
	${CC} ${CFLAGS} -c ${BOLODIR}treegrow.c

playersrejoin.o: ${BOLODIR}playersrejoin.c ${BOLODIR}playersrejoin.h
	${CC} ${CFLAGS} -c ${BOLODIR}playersrejoin.c

network.o: ${BOLODIR}network.c ${BOLODIR}network.h
	${CC} ${CFLAGS} -c ${BOLODIR}network.c

screen.o: ${BOLODIR}screen.c ${BOLODIR}screen.h
	${CC} ${CFLAGS} -c ${BOLODIR}screen.c

