# +=============================================+
# |                                             |
# |          The __    ____  ____  __           |
# |             / /   / __ \/ __ \/ /           |
# |            / /   / / / / /_/ / /            |
# |           / /___/ /_/ / ____/ /___          |
# |          /_____/_____/_/   /_____/          |
# |               Programming Language          |
# |          http://www.ldpl-lang.org/          |
# |                                             |
# +=============================================+

# +----------+
# | MAKEFILE |
# +----------+
# Usage:
# make [x86=true] [version='"<version>"']
# make install

# --- Version Information ---
VERSION = '"5.2.1"'  #LDPL Version
VERSIONNAME = '"Groovy Gualicho"' #LDPL Version Name

# --- Options ---
# Alternative version name (make version="<version>")
ifdef version
VERSION = '"$(version)"'
endif
# x86 cross compilation options (make x86=true)
ifdef x86
CROSS = -m32
VERSION = '"$(version) (i386)"'
endif
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

# --- Makefile Data ---
SOURCE = ldpl.cpp
OUT = ldpl
LPMLOCATION = ~/ldpl/lpm/
LDPLLIBLOCATION = /lib/ldpl

# --- Compilation Flags ---
FLAGS = -Wall -std=gnu++11 -fpermissive -DVERSION=$(VERSION) -DVERSIONNAME=$(VERSIONNAME) -DCOMPILEDATE='"$(shell date +%Y-%m-%d)"' -DCOMPILEHOUR='"$(shell date +%H:%M:%S)"' -DLPMLOCATION='"$(DESTDIR)$(PREFIX)$(LPMLOCATION)"' -DLDPLLIBLOCATION='"$(DESTDIR)$(PREFIX)$(LDPLLIBLOCATION)"'

# --- Build Rules ---
# Build LDPL
all:
	cd src && $(CXX) $(FLAGS) $(CROSS) $(SOURCE) -o $(OUT) $(LFLAGS)
	mkdir -p build
	mv src/$(OUT) build

# Delete built file
clean:
	rm -rf build

install: build/ldpl
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 775 build/ldpl $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/lib/ldpl
	install src/ldpl_lib/ldpl_lib.cpp $(DESTDIR)$(PREFIX)$(LDPLLIBLOCATION)
	install src/ldpl_lib/BigInt.hpp $(DESTDIR)$(PREFIX)$(LDPLLIBLOCATION)
	
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/ldpl
	rm -rf $(DESTDIR)$(PREFIX)$(LDPLLIBLOCATION)
