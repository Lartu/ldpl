default: lib

build: hello.cpp
	c++ hello.cpp -std=gnu++11 -o hello

lib: hello.cpp
	make clean
	c++ -c hello.cpp -std=gnu++11 -o hello.o
	ar -cvq hello.a hello.o

test: lib
	cd ../../src && make 
	../../src/ldpl -i=hello.a -o=hello hello.ldpl
	./hello

clean:
	rm -f hello.{o,a} hello-bin hello
