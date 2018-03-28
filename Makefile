CFLAGS=-std=c++14 -fopenmp

# List of sources and objects (include all .cpp files)
SOURCES=$(shell find ./src -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)

TARGET=triangleSimulation
EXECUTABLE=$(TARGET)

LIB_DIR=./lib
LIBS=-lm

all: $(SOURCES) $(TARGET)

borg: CC=mpicxx
borg: LIBS += -lborgmm
borg: CFLAGS += -DPARALLEL -fopenmp
borg: all

gcc: CC=g++
gcc: CFLAGS+=-O3 -march=native
gcc: all

intel: CC=icc
intel: CFLAGS+=-O3 -xAVX
intel: all

gcc-debug: CC=g++
gcc-debug: CFLAGS+=-O0 -march=native -g
gcc-debug: all

intel-debug: CC=icc
intel-debug: CFLAGS+=-O0 -xAVX -g
intel-debug: all

pchecking: CC=icc
pchecking: CFLAGS+=-O0 -xAVX -g -traceback -check-pointers=rw -check-pointers-undimensioned -check-pointers-dangling=all -rdynamic
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
