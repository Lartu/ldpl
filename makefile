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

# --- Version Information ---
VERSION = '"4.5-dev"'  #LDPL Version
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
LPMLOCATION = "~/ldpl/lpm/"
LDPLLIBLOCATION = "/usr/local/lib/ldpl"

# --- Compilation Flags ---
FLAGS = -Wall -std=gnu++11 -fpermissive -DVERSION=$(VERSION) -DVERSIONNAME=$(VERSIONNAME) -DCOMPILEDATE='"$(shell date +%Y-%m-%d)"' -DCOMPILEHOUR='"$(shell date +%H:%M:%S)"' -DLPMLOCATION='$(LPMLOCATION)' -DLDPLLIBLOCATION='$(LDPLLIBLOCATION)'
#Original Flags
#FLAGS = -g -c -Wall -std=gnu++11 -fpermissive -DVERSION=$(VERSION) -DVERSIONNAME=$(VERSIONNAME) -DCOMPILEDATE='"$(shell date +%Y-%m-%d)"' -DCOMPILEHOUR='"$(shell date +%H:%M:%S)"' -DLPMLOCATION='$(LPMLOCATION)'


# --- Build Rules ---
# Build LDPL
all:
	cd src && $(CXX) $(FLAGS) $(CROSS) $(SOURCE) -o $(OUT) $(LFLAGS)
	mkdir -p build
	mv src/$(OUT) build

# Rebuild the LDPL manual page
man:
	cd cd man && sh generateMan.sh

# Delete built file
clean:
	rm -rf build

install: build/ldpl man/ldpl.1
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 775 build/ldpl $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/share/man/man1/
	install man/ldpl.1 $(DESTDIR)$(PREFIX)/share/man/man1/
	install -d $(DESTDIR)$(PREFIX)/lib/ldpl
	install src/ldpl_lib/ldpl_lib.cpp $(LDPLLIBLOCATION)
	
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/ldpl
	rm $(DESTDIR)$(PREFIX)/share/man/man1/ldpl.1
