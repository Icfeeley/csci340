CC = gcc
CFLAGS = -g -Wall

all: hw7

#Make executable
hw7: mem.o main.o
	$(CC) $(CFLAGS) -o hw7 mem.o main.o

#Make object files
mem.o: mem.c mem.h
	$(CC) $(CFLAGS) -c mem.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

#Make clean
clean:
	$(RM) hw7 *.o *~
