.PHONY : clean

CC = gcc

CFLAGS = -Wall -g

BIN = selectcli selectsrv
 

all : $(BIN)

%.o : %.c
	gcc -Wall -g -c $< -o $@

clean : 
	rm -f *.o $(BIN)


