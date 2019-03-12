# +---------------+
# | LDPL Makefile |
# +---------------+------+
# | LDPL Version Details |
# +----------------------+

VERSION = '"2.0.2"' #LDPL Version
LDPLSTD = 19		 #LDPL Standard

# +-----------------+
# | The other stuff |
# +-----------------+

OBJS = ldpl.o
SOURCE = ldpl.cpp
OUT = ldpl
FLAGS = -g -c -Wall -std=c++11 -fpermissive -DVERSION=$(VERSION) -DSTANDARD=$(LDPLSTD) -DCOMPILEDATE='"$(shell date)"'
LFLAGS = -static-libgcc -static-libstdc++ 

all: $(OBJS)
	$(CXX) -g $(OBJS) -o $(OUT) $(LFLAGS)

ldpl_included_lib.cpp: ldpl_lib.cpp
	php lib-to-string.php > ldpl_included_lib.cpp

ldpl.o: ldpl.cpp ldpl_included_lib.cpp
	$(CXX) $(FLAGS) $(SOURCE)


clean:
	rm -f ldpl.o ldpl ldpl_included_lib.cpp
