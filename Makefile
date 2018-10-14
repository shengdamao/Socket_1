.PHONY : clean

CC = gcc

CFLAGS = -Wall -g

BIN = rlinesrv rlinecli
 

all : $(BIN)

%.o : %.c
	gcc -Wall -g -c $< -o $@

clean : 
	rm -f *.o $(BIN)


