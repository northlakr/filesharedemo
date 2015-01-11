CC = gcc
log : log.o
	$(CC) -o log log.o
log.o : log.c log.h
	$(CC) -c log.c
clean :
	rm -rf *.o
