# +----------------------+
# | LAPL Antlr4 Makefile |
# +----------------------+

all:
	mkdir -p build && mkdir -p run
	cd build && cmake .. -DANTLR_JAR_LOCATION="$(shell readlink -f ../antlr-4.8-complete.jar)" -DWITH_DEMO=False
	cd build && make && DESTDIR=../run make install

clean:
	rm -rf build
	rm -rf del
	rm -rf dist
	rm -rf run