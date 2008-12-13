# Makefile for Tracker
CC = gcc
LFLAGS = -pthread 
#Debug
CFLAGS = -g -Wall -ansi -pedantic 
#Release
#CFLAGS = -Wall -ansi -pedantic -O3 -s -ffast-math -fomit-frame-pointer -mpentium -march=pentium 
OUTPUT = tracker

all: tracker

clean:
	rm *.o
	rm ${OUTPUT}


tracker: global.o bans.o currentgames.o main.o tcp.o udp.o http.o \
	debug.o util.o stats.o irc.o interesting.o
	${CC} ${CFLAGS} ${LFLAGS} -o ${OUTPUT} \
	global.o bans.o currentgames.o main.o tcp.o udp.o http.o \
	debug.o util.o	stats.o irc.o interesting.o

global.o: global.c global.h
	${CC} ${CFLAGS} -c global.c

bans.o: bans.c bans.h
	${CC} ${CFLAGS} -c bans.c

irc.o: irc.c irc.h
	${CC} ${CFLAGS} -c irc.c
	
currentgames.o: currentgames.c currentgames.h
	${CC} ${CFLAGS} -c currentgames.c

main.o: main.c 
	${CC} ${CFLAGS} -c main.c

tcp.o: tcp.c tcp.h
	${CC} ${CFLAGS} -c tcp.c

udp.o: udp.c udp.h
	${CC} ${CFLAGS} -c udp.c

http.o: http.c http.h
	${CC} ${CFLAGS} -c http.c

debug.o: debug.c debug.h
	${CC} ${CFLAGS} -c debug.c

util.o: util.c util.h
	${CC} ${CFLAGS} -c util.c

stats.o: stats.c stats.h
	${CC} ${CFLAGS} -c stats.c

interesting.o: interesting.c interesting.h
	${CC} ${CFLAGS} -c interesting.c

