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


# Makefile for LinBolo Server
# Author: John Morrison 

CC = gcc
CCC = checkergcc
CFLAGS = -g `sdl-config --cflags`
#CFLAGS = -O3 -s -ffast-math -fomit-frame-pointer `sdl-config --cflags`
#CFLAGS = -g
BOLODIR = ../bolo/
GUIDIR = ../gui/
WBNETDIR = ../winbolonet/
GUIDIRLINUX = ../gui/linux/
SERVERDIR = ./
COMPRESSDIR = ../lzw/
SERVERNAME = linbolods
ZLIBDIR = ../zlib/

all: linbolods

b: global.o bases.o util.o crc.o pillbox.o lgm.o shells.o tank.o \
	allience.o bolo_map.o building.o explosions.o floodfill.o \
	gametype.o grass.o labels.o messages.o mines.o minesexp.o \
	netmt.o netplayers.o netpnb.o players.o rubble.o \
	screenbrainmap.o screenbullet.o screencalc.o screenlgm.o \
	screentank.o scroll.o sounddist.o starts.o swamp.o \
	tankexp.o treegrow.o dcodlzw.o ecodlzw.o playersrejoin.o \
	lang.o winbolonetevents.o winbolonetthread.o backend.o

server: servercore.o servermessages.o serverfrontend.o servermain.o \
	servernet.o servertransport.o threads.o log.o udppackets.o \
	winbolonet.o http.o preferences.o
	
zlib: 	adler32.o compress.o crc32.o deflate.o gzio.o \
	inffast.o inflate.o inftrees.o trees.o ioapi.o uncompr.o \
	unzip.o zip.o zutil.o 
	
linbolods: zlib b server
	${CC} `sdl-config --libs` -o ${SERVERNAME} \
	global.o bases.o util.o crc.o pillbox.o lgm.o shells.o tank.o \
	allience.o bolo_map.o building.o explosions.o floodfill.o \
	gametype.o grass.o labels.o mines.o minesexp.o  netmt.o netplayers.o \
	netpnb.o players.o rubble.o screenbrainmap.o  starts.o swamp.o \
	tankexp.o playersrejoin.o treegrow.o servercore.o servermessages.o \
	serverfrontend.o servermain.o servernet.o servertransport.o threads.o \
	dcodlzw.o ecodlzw.o lang.o log.o udppackets.o winbolonetevents.o \
	winbolonet.o http.o screenbullet.o preferences.o winbolonetthread.o \
	backend.o adler32.o compress.o crc32.o deflate.o gzio.o  \
	inffast.o inflate.o inftrees.o trees.o ioapi.o uncompr.o \
	unzip.o zip.o zutil.o
		
clean: 
	rm *.o
	rm linbolods
		

#zlib
adler32.o: ${ZLIBDIR}adler32.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}adler32.c
compress.o: ${ZLIBDIR}compress.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}compress.c
crc32.o: ${ZLIBDIR}crc32.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}crc32.c
deflate.o: ${ZLIBDIR}deflate.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}deflate.c
gzio.o: ${ZLIBDIR}gzio.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}gzio.c
infback.o: ${ZLIBDIR}infback.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}infback.c

inffast.o: ${ZLIBDIR}inffast.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}inffast.c
inflate.o: ${ZLIBDIR}inflate.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}inflate.c
inftrees.o: ${ZLIBDIR}inftrees.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}inftrees.c
trees.o: ${ZLIBDIR}trees.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}trees.c
ioapi.o: ${ZLIBDIR}ioapi.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}ioapi.c
uncompr.o: ${ZLIBDIR}uncompr.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}uncompr.c
unzip.o: ${ZLIBDIR}unzip.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}unzip.c
zip.o: ${ZLIBDIR}zip.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}zip.c
zutil.o: ${ZLIBDIR}zutil.c
	${CC} ${CFLAGS} -c ${ZLIBDIR}zutil.c


# gui lang

lang.o: ${GUIDIRLINUX}lang.c ${GUIDIR}lang.h
	${CC} ${CFLAGS} -c ${GUIDIRLINUX}lang.c

preferences.o: ${GUIDIRLINUX}preferences.c ${GUIDIRLINUX}preferences.h
	${CC} ${CFLAGS} -c ${GUIDIRLINUX}preferences.c

#winbolonet
winbolonet.o: ${WBNETDIR}winbolonet.c ${WBNETDIR}winbolonet.h
	${CC} ${CFLAGS} -c ${WBNETDIR}winbolonet.c

winbolonetthread.o: ${WBNETDIR}winbolonetthread.c ${WBNETDIR}winbolonetthread.h
	${CC} ${CFLAGS} -c ${WBNETDIR}winbolonetthread.c

winbolonetevents.o: ${WBNETDIR}winbolonetevents.c ${WBNETDIR}winbolonetevents.h
	${CC} ${CFLAGS} -c ${WBNETDIR}winbolonetevents.c

http.o: ${WBNETDIR}http.c ${WBNETDIR}http.h
	${CC} ${CFLAGS} -c ${WBNETDIR}http.c


#Bolo
global.o: ${BOLODIR}global.c ${BOLODIR}global.h
	${CC} ${CFLAGS} -c ${BOLODIR}global.c

backend.o: ${BOLODIR}backend.c 
	${CC} ${CFLAGS} -c ${BOLODIR}backend.c

util.o:	${BOLODIR}util.c ${BOLODIR}util.h
	${CC} ${CFLAGS} -c ${BOLODIR}util.c
crc.o: ${BOLODIR}crc.c ${BOLODIR}crc.h
	${CC} ${CFLAGS} -c ${BOLODIR}crc.c

screenbullet.o: ${BOLODIR}screenbullet.c ${BOLODIR}screenbullet.h
	${CC} ${CFLAGS} -c ${BOLODIR}screenbullet.c


playersrejoin.o: ${BOLODIR}playersrejoin.c ${BOLODIR}playersrejoin.h
	${CC} ${CFLAGS} -c ${BOLODIR}playersrejoin.c
	
bases.o: ${BOLODIR}bases.h ${BOLODIR}bases.c
	${CC} ${CFLAGS} -c ${BOLODIR}bases.c

lgm.o: ${BOLODIR}lgm.c ${BOLODIR}lgm.h
	${CC} ${CFLAGS} -c ${BOLODIR}lgm.c

pillbox.o: ${BOLODIR}pillbox.c ${BOLODIR}pillbox.h
	${CC} ${CFLAGS} -c ${BOLODIR}pillbox.c

shells.o: ${BOLODIR}shells.c ${BOLODIR}shells.h
	${CC} ${CFLAGS} -c ${BOLODIR}shells.c

tank.o: ${BOLODIR}tank.c ${BOLODIR}tank.h
	${CC} ${CFLAGS} -c ${BOLODIR}tank.c

log.o: ${BOLODIR}log.c ${BOLODIR}log.h
	${CC} ${CFLAGS} -c ${BOLODIR}log.c

udppackets.o: ${BOLODIR}udppackets.c ${BOLODIR}udppackets.h
	${CC} ${CFLAGS} -c ${BOLODIR}udppackets.c

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
	
netplayers.o: ${BOLODIR}netplayers.c ${BOLODIR}netplayers.h
	${CC} ${CFLAGS} -c ${BOLODIR}netplayers.c
	
netpnb.o: ${BOLODIR}netpnb.c ${BOLODIR}netpnb.h
	${CC} ${CFLAGS} -c ${BOLODIR}netpnb.c
	
network.o: ${BOLODIR}network.c ${BOLODIR}network.h
	${CC} ${CFLAGS} -c ${BOLODIR}network.c

players.o: ${BOLODIR}players.c ${BOLODIR}players.h
	${CC} ${CFLAGS} -c ${BOLODIR}players.c
	
rubble.o: ${BOLODIR}rubble.c ${BOLODIR}rubble.h
	${CC} ${CFLAGS} -c ${BOLODIR}rubble.c
	
screenbrainmap.o: ${BOLODIR}screenbrainmap.c ${BOLODIR}screenbrainmap.h
	${CC} ${CFLAGS} -c ${BOLODIR}screenbrainmap.c
	
screencalc.o: ${BOLODIR}screencalc.c ${BOLODIR}screencalc.h
	${CC} ${CFLAGS} -c ${BOLODIR}screencalc.c
	
screenlgm.o: ${BOLODIR}screenlgm.c ${BOLODIR}screenlgm.h
	${CC} ${CFLAGS} -c ${BOLODIR}screenlgm.c

screentank.o: ${BOLODIR}screentank.c ${BOLODIR}screentank.h
	${CC} ${CFLAGS} -c ${BOLODIR}screentank.c

screen.o: ${BOLODIR}screen.c ${BOLODIR}screen.h
	${CC} ${CFLAGS} -c ${BOLODIR}screen.c

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

dcodlzw.o: ${COMPRESSDIR}dcodlzw.c
	${CC} ${CFLAGS} -c ${COMPRESSDIR}dcodlzw.c

ecodlzw.o: ${COMPRESSDIR}ecodlzw.c
	${CC} ${CFLAGS} -c ${COMPRESSDIR}ecodlzw.c



#
# Server components 
#
servercore.o: ${SERVERDIR}servercore.c ${SERVERDIR}servercore.h
	${CC} ${CFLAGS} -c ${SERVERDIR}servercore.c
servermessages.o: ${SERVERDIR}servermessages.c ${BOLODIR}messages.h
	 ${CC} ${CFLAGS} -c ${SERVERDIR}servermessages.c
serverfrontend.o: ${SERVERDIR}serverfrontend.c 
	${CC} ${CFLAGS} -c ${SERVERDIR}serverfrontend.c
	 
servermain.o: ${SERVERDIR}servermain.c
	${CC} ${CFLAGS} -c ${SERVERDIR}servermain.c

servernet.o: ${SERVERDIR}servernet.c ${SERVERDIR}servernet.h
	${CC}  ${CFLAGS} -c ${SERVERDIR}servernet.c

servertransport.o: ${SERVERDIR}servertransport.c ${SERVERDIR}servertransport.h
	${CC}  ${CFLAGS} -c ${SERVERDIR}servertransport.c
	
threads.o: ${SERVERDIR}threads.c ${SERVERDIR}threads.h
	${CC}  ${CFLAGS} -c ${SERVERDIR}threads.c

