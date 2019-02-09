OBJS = ldpl.o
SOURCE = ldpl.cpp
HEADER = ldpl.h nvm.h cpptrim.h
OUT = ldpl
CC = g++
FLAGS = -g -c -Wall
LFLAGS =

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)
	rm ldpl.o
	
ldpl.o: ldpl.cpp
	$(CC) $(FLAGS) ldpl.cpp -fpermissive -DVERSION='"1.0.0"' -DCOMPILEDATE='"$(shell date)"' -DSTANDARD=19
	
clean:
	rm -f $(OBJS) $(OUT)
