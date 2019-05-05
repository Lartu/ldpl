# +---------------+
# | LDPL Makefile |
# +---------------+------+
# | LDPL Version Details |
# +----------------------+

VERSION = '"3.0.4"'  #LDPL Version
VERSIONNAME = '"Busy Brontosaurus"' #LDPL Version Name

# +-----------------+
# | The other stuff |
# +-----------------+

OBJS = ldpl.o
SOURCE = ldpl.cpp
OUT = ldpl
FLAGS = -g -c -Wall -std=gnu++11 -fpermissive -DVERSION=$(VERSION) -DVERSIONNAME=$(VERSIONNAME) -DCOMPILEDATE='"$(shell date +%Y-%m-%d)"' -DCOMPILEHOUR='"$(shell date +%H:%M:%S)"'
ifneq ($(shell uname -s),Darwin)
ifneq ($(shell uname -o),Android)
	LFLAGS = -static-libgcc -static-libstdc++
endif
endif

all: $(OBJS)
	$(CXX) -g $(OBJS) -o $(OUT) $(LFLAGS)

ldpl_included_lib.cpp: ldpl_lib.cpp
	awk -f lib-to-string.awk ldpl_lib.cpp > ldpl_included_lib.cpp

ldpl.o: ldpl.cpp ldpl_included_lib.cpp
	$(CXX) $(FLAGS) $(SOURCE)


clean:
	rm -f ldpl.o ldpl ldpl_included_lib.cpp
