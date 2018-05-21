CFLAGS=-std=c++14 #-fsanitize=address -fno-omit-frame-pointer #-fno-sanitize-address-use-after-scope

# List of sources and objects (include all .cpp files)
SOURCES=$(shell find ./src -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)

TARGET=triangleSimulation
EXECUTABLE=$(TARGET)

LIB_DIR=./lib
LIBS=-static-libasan -lm

all: $(SOURCES) $(TARGET)

borg: CC=mpicxx
borg: LIBS += -lborgmm
borg: CFLAGS += -DPARALLEL -fopenmp -march=ivybridge -O0 -g
borg: all

gcc: CC=g++
gcc: CFLAGS+=-O3 -march=native -fopenmp
gcc: all

intel: CC=icc
intel: CFLAGS+=-O3 ${TACC_VEC_FLAGS} -qopenmp
intel: all

gcc-debug: CC=g++
gcc-debug: CFLAGS+=-O0 -march=ivybridge -g -fopenmp
gcc-debug: all

intel-debug: CC=icc
intel-debug: CFLAGS+=-O1 -xAVX -g -qopenmp
#intel-debug: CFLAGS+=-O3 ${TACC_VEC_FLAGS} -g -qopenmp
intel-debug: all

pchecking: CC=icc
pchecking: CFLAGS+=-O0 ${TACC_VEC_FLAGS} -g -traceback -check-pointers=rw -check-pointers-undimensioned -check-pointers-dangling=all -rdynamic -qopenmp
pchecking: all

prof: CFLAGS += -fopenmp -p
prof: all

# How to make objects and executables
.cpp.o:
	$(CC) -c $(CFLAGS) $^ -o $@
	
$(TARGET): $(OBJECTS)
	$(CC) -I. $(LDFLAGS) $(OBJECTS) $(CFLAGS) -o $@ -L$(LIB_DIR) $(LIBS) $(DEFINES)

clean:
	rm -rf $(shell find . -name "*.o") $(EXECUTABLE)
	rm -rf $(shell find . -name "*.optrpt") $(EXECUTABLE)
