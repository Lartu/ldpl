SOURCE = ldpl.cpp
OUT=ldpl
STANDARD="19"
COMPILEDATE="$(shell date)"
VERSION="1.1.2"

CXXFLAGS += -Wall -DVERSION='$(VERSION)' -DCOMPILEDATE='$(COMPILEDATE)' \
	    -DSTANDARD=$(STANDARD)

ifneq ($(STATIC), n)
	LDFLAGS += -static
	CXXFLAGS += -static-libgcc -static-libstdc++ -std=c++11
endif

all: $(OUT)

clean:
	rm -f $(OUT)
