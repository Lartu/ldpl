OBJS = ldpl.o
SOURCE = ldpl.cpp
HEADER = ldpl.h nvm.h cpptrim.h
OUT = ldpl
CC = c++
FLAGS = -g -c -Wall -std=c++11
LFLAGS = -static-libgcc -static-libstdc++

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

ldpl.o: ldpl.cpp
	echo "If compilation fails under Windows, try removing the -std=c++11 flag."
	php lib-to-string.php > ldpl_included_lib.h
	$(CC) $(FLAGS) ldpl.cpp -fpermissive -DVERSION='"2.0.0"' -DCOMPILEDATE='"$(shell date)"' -DSTANDARD=19

clean:
	rm -f $(OBJS) $(OUT)
