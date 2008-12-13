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
#

CC = gcc
CFLAGS = -g 
#CFLAGS = -s -O3 -ffast-math -fomit-frame-pointer 
APPNAME = wome.cgi

all: wome

install:
	cp wome.cgi /usr/lib/cgi-bin

uninstall:
	rm /usr/lib/cgi-bin/${APPNAME}
clean:
	rm *.o
	rm ${APPNAME}

wome: global.o bases.o pillbox.o bolo_map.o main.o starts.o base64.o \
	screen.o cgi-lib.o cgi-llist.o string-lib.o html-lib.o
	${CC} -lm -o ${APPNAME} ${CFLAG} \
	global.o bases.o pillbox.o bolo_map.o main.o starts.o base64.o \
	screen.o cgi-lib.o cgi-llist.o string-lib.o html-lib.o

main.o: main.c 
	${CC} ${CFLAGS} -c main.c

global.o: global.c global.h
	${CC} ${CFLAGS} -c global.c

bases.o: bases.h bases.c
	${CC} ${CFLAGS} -c bases.c

pillbox.o: pillbox.c pillbox.h
	${CC} ${CFLAGS} -c pillbox.c
	
bolo_map.o: bolo_map.c bolo_map.h
	${CC} ${CFLAGS} -c bolo_map.c

base64.o: base64.c base64.h
	${CC} ${CFLAGS} -c base64.c
	
starts.o: starts.c starts.h
	${CC} ${CFLAGS} -c starts.c

screen.o: screen.c screen.h
	${CC} ${CFLAGS} -c screen.c


cgi-lib.o: cgi-lib.c cgi-lib.h
	${CC} ${CFLAGS} -c cgi-lib.c


cgi-llist.o: cgi-llist.c cgi-llist.h
	${CC} ${CFLAGS} -c cgi-llist.c
	
	
string-lib.o: string-lib.c string-lib.h
	${CC} ${CFLAGS} -c string-lib.c

html-lib.o: html-lib.h html-lib.c
	${CC} -c ${CFLAGS} -c html-lib.c


