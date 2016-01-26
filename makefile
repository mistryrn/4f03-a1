CFLAGS=-g -O2 -Wall -Wno-unused-variable
CC=mpicc

PROGRAM_NAME= ppmf
OBJS = main.o readwriteppm.o processimage.o mean.o median.o

$(PROGRAM_NAME): $(OBJS)
	$(CC) -o $@ $? 

clean:
	rm  *.o $(PROGRAM_NAME) *~