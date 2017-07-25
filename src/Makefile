CC=g++
PCC=mpiCC

#the following is for normal use:
CFLAGS=-std=c++14 -O3 -openmp

#the following is for using gprof:
#CFLAGS=-g -c -O0 -Wall
#LDFLAGS=-pg 

# List of sources and objects (include all .cpp files)
SOURCES=$(shell find . -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)

TARGET=triangleSimulation

LIB_DIR=./lib/
LIBS=-lm

all: $(SOURCES) $(TARGET)

# parallel: CC=mpiCC
parallel: LIBS += -lborgms
parallel: CFLAGS += -DPARALLEL -ipo
parallel: all

debug: CFLAGS=-O3
debug: CFLAGS += -g
debug: all

# How to make objects and executables
.cpp.o:
	$(CC) -c $(CFLAGS) $^ -o $@
	
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(CFLAGS) -o $@ -L$(LIB_DIR) $(LIBS) $(DEFINES)

clean:
	rm -rf $(shell find . -name "*.o") $(EXECUTABLE)
