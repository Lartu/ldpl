OBJS = ldpl.o
SOURCE = ldpl.cpp
HEADER = ldpl.h nvm.h cpptrim.h
OUT = ldpl
CC = g++
FLAGS = -g -c -Wall
LFLAGS = -ldl

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)
	
ldpl.o: ldpl.cpp
	$(CC) $(FLAGS) ldpl.cpp -fpermissive
	
clean:
	rm -f $(OBJS) $(OUT)
