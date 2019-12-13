# +---------------+
# | LDPL Makefile |
# +---------------+------+
# | LDPL Version Details |
# +----------------------+

VERSION = '"4.5-dev"'  #LDPL Version
VERSIONNAME = '"Friendly Falcarius"' #LDPL Version Name

# +-----------------+
# | The other stuff |
# +-----------------+

OBJS = ldpl.o
SOURCE = ldpl.cpp
OUT = ldpl
LPMLOCATION = "~/ldpl/lpm/"
# x86 cross compilation options (make x86=true)
ifdef x86
CROSS = -m32
CROSS = -m32
endif
FLAGS = -g -c -Wall -std=gnu++11 -fpermissive -DVERSION=$(VERSION) -DVERSIONNAME=$(VERSIONNAME) -DCOMPILEDATE='"$(shell date +%Y-%m-%d)"' -DCOMPILEHOUR='"$(shell date +%H:%M:%S)"' -DLPMLOCATION='$(LPMLOCATION)'
#Flags for static compilation on Windows and Linux (not Android)
ifneq ($(shell uname -s),Darwin)
ifneq ($(shell uname -o),Android)
LFLAGS = -static-libgcc -static-libstdc++
endif
endif
#PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(PREFIX),)
PREFIX := /usr/local
endif

all: $(OBJS) ldpl.1
	$(CXX) -g $(OBJS) -o $(OUT) $(LFLAGS) $(CROSS)
	./ldpl ../lpm/lpm.ldpl -o=lpm

ldpl.o: ldpl.cpp ldpl_lib.cpp
	awk -f lib-to-string.awk ldpl_lib.cpp > ldpl_included_lib.cpp
	$(CXX) $(FLAGS) $(CROSS) $(SOURCE)

ldpl.1:
	cd .. && cd man && sh generateMan.sh
	cp ../man/ldpl.1 ldpl.1

clean:
	rm -f ldpl.o ldpl ldpl_included_lib.cpp lpm ldpl.1

install: ldpl ldpl.1
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 775 ldpl $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 775 lpm $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/share/man/man1/
	install ldpl.1 $(DESTDIR)$(PREFIX)/share/man/man1/
ifneq ($(shell uname -s),Darwin)
	mandb
endif

uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/ldpl
	rm $(DESTDIR)$(PREFIX)/bin/lpm
	rm $(DESTDIR)$(PREFIX)/share/man/man1/ldpl.1
ifneq ($(shell uname -s),Darwin)
	mandb
endif
