CC = gcc
OBJ = main.o log.o
fileshare : $(OBJ)
	$(CC) -o  fileshare $(OBJ) -lpthread
main.o : log.h
log.o : log.h
.PHONY : clean
clean :
	rm -rf fileshare *.log *.o
