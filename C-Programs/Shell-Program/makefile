CC=gcc
CFLAGS=-I.
DEPS = include.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

MS1: main.o getfunc.o infofuncs.o putfunc.o calcfunc.o
	$(CC) -o MS1 main.o getfunc.o infofuncs.o putfunc.o calcfunc.c