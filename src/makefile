SOURCE = ldpl.cpp

OUT=ldpl
STANDARD="11"
COMPILEDATE="$(shell date)"
VERSION="1.1.2"

CXXFLAGS += -Wall -DVERSION='$(VERSION)' -DCOMPILEDATE='$(COMPILEDATE)' \
	    -DSTANDARD=$(STANDARD)

ifneq ($(STATIC), n)
	LDFLAGS += -static
	CXXFLAGS += -static-libgcc -static-libstdc++
endif

all: $(OUT)

clean:
	rm -f $(OUT)
