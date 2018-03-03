#CC=g++
CC=icc

#the following is for normal use:
CFLAGS=-std=c++14 -O0 -march=native -qopt-report=5 -g #-check-pointers=rw
#CFLAGS=-std=c++14 -O0 -march=native -Wall -Wextra -flto-report#-check-pointers=rw


# List of sources and objects (include all .cpp files)
SOURCES=$(shell find ./src -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)

TARGET=triangleSimulation
EXECUTABLE=$(TARGET)

LIB_DIR=./lib
LIBS=-lm

all: $(SOURCES) $(TARGET)

borg: CC=mpiicpc
borg: LIBS += -lborgmm
borg: CFLAGS += -DPARALLEL
borg: all

openmp: CFLAGS += -fopenmp
openmp: all

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
