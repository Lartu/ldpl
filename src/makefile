# +---------------+
# | LDPL Makefile |
# +---------------+------+
# | LDPL Version Details |
# +----------------------+

VERSION = '"3.1.0-dev"'  #LDPL Version
VERSIONNAME = '"Diligent Dreadnoughtus"' #LDPL Version Name

# +-----------------+
# | The other stuff |
# +-----------------+

OBJS = ldpl.o
SOURCE = ldpl.cpp
OUT = ldpl
FLAGS = -g -c -Wall -std=gnu++11 -fpermissive -DVERSION=$(VERSION) -DVERSIONNAME=$(VERSIONNAME) -DCOMPILEDATE='"$(shell date +%Y-%m-%d)"' -DCOMPILEHOUR='"$(shell date +%H:%M:%S)"'
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

all: $(OBJS)
	$(CXX) -g $(OBJS) -o $(OUT) $(LFLAGS)

ldpl_included_lib.cpp: ldpl_lib.cpp
	awk -f lib-to-string.awk ldpl_lib.cpp > ldpl_included_lib.cpp

ldpl.o: ldpl.cpp ldpl_included_lib.cpp
	$(CXX) $(FLAGS) $(SOURCE)


clean:
	rm -f ldpl.o ldpl ldpl_included_lib.cpp

install: ldpl ../man/ldpl.1
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 775 ldpl $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/share/man/man1/
	install ../man/ldpl.1 $(DESTDIR)$(PREFIX)/share/man/man1/
ifneq ($(shell uname -s),Darwin)
	mandb
endif

uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/ldpl
	rm $(DESTDIR)$(PREFIX)/share/man/man1/ldpl.1
ifneq ($(shell uname -s),Darwin)
	mandb
endif
