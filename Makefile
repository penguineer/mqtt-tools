# http://stackoverflow.com/questions/2145590/what-is-the-purpose-of-phony-in-a-makefile

DEBUG   = -O3                                                                   
CC      = gcc                                                                   
INCLUDE = -I/usr/local/include                                                  
CFLAGS  = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe                              
                                                                                
LDFLAGS = -L/usr/local/lib                                                      
LDLIBS    = -lpthread -lm -lmosquitto


.phony: clean

all: mqtt-clock

clean:
	rm mqtt-clock *.o

mqtt-clock: mqtt-clock.o mosqhelper.o mosqagent.o
	@$(CC) -o $@ mqtt-clock.o mosqhelper.o mosqagent.o $(LDFLAGS) $(LDLIBS)

mqtt-clock.o: mqtt-clock.c
	@$(CC) -c mqtt-clock.c -o $@

mosqhelper.o: mosqhelper.c mosqhelper.h
	@$(CC) -c mosqhelper.c -o $@

mosqagent.o: mosqagent.c mosqagent.h
	@$(CC) -c mosqagent.c -o $@
