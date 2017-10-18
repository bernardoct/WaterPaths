#CC=g++
CC=icc
PCC=mpiCC

#the following is for normal use:
CFLAGS=-std=c++14 -O3 -g -march=native -restrict -simd -axCORE-AVX2 -Wall -Wextra -qopt-report=4# -check-pointers=rw
#CFLAGS=-std=c++14 -O3 -g -march=native -Wall -Wextra -mavx -flto-report # -check-pointers=rw

#the following is for using gprof:
#CFLAGS=-g -c -O0 -Wall
#LDFLAGS=-pg 

# List of sources and objects (include all .cpp files)
SOURCES=$(shell find ./src -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)

TARGET=triangleSimulation
EXECUTABLE=$(TARGET)

LIB_DIR=./lib/
LIBS=-lm

all: $(SOURCES) $(TARGET)

openmp: CFLAGS += -fopenmp
openmp: all

prof: CFLAGS += -fopenmp -p
prof: all

# How to make objects and executables
.cpp.o:
	$(CC) -c $(CFLAGS) $^ -o $@
	
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(CFLAGS) -o $@ -L$(LIB_DIR) $(LIBS) $(DEFINES)

clean:
	rm -rf $(shell find . -name "*.o") $(EXECUTABLE)
	rm -rf $(shell find . -name "*.optrpt") $(EXECUTABLE)
