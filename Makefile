CFLAGS=-std=c++14 #-fsanitize=address -fno-omit-frame-pointer #-fno-sanitize-address-use-after-scope
LDFLAGS=-lstdc++

# List of sources and objects (include all .cpp files)
SOURCES=$(shell find ./src -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)

TARGET=waterpaths
EXECUTABLE=$(TARGET)

LIB_DIR=./lib
LIBS=-static-libasan -lm

all: $(SOURCES) $(TARGET)

###### Make arguments to toggle WaterPaths development features on and off. ######
# -DBORG_INPUT_FILE_DEBUG: activate some verbose to ease debugging how decision  #
#     variables from optimization are coupled with the input file.               #
# -DPROFILE: activate Valgrind's import and instrumentation start and end.       #
##################################################################################

borg: CC=mpicxx
borg: LIBS += -lborgms
borg: CFLAGS += -DPARALLEL -fopenmp -march=native -O2 
borg: all

gcc: CC=g++
gcc: CFLAGS+=-O2 -march=native -fopenmp
gcc: all

intel: CC=icc
intel: CFLAGS+=-O2 ${TACC_VEC_FLAGS} -qopenmp
intel: all

gcc-debug: CC=g++
gcc-debug: CFLAGS+=-Og -march=native -g -fopenmp
gcc-debug: all

intel-debug: CC=icc
intel-debug: CFLAGS+=-O1 -xAVX -g -qopenmp
intel-debug: all

pchecking: CC=icc
pchecking: CFLAGS+=-O0 ${TACC_VEC_FLAGS} -g -traceback -check-pointers=rw -check-pointers-undimensioned -check-pointers-dangling=all -rdynamic -qopenmp
pchecking: all

prof: CFLAGS += -fopenmp -pg
prof: all

# How to make objects and executables
.cpp.o:
	$(CC) -c $(CFLAGS) $^ -o $@

$(TARGET): $(OBJECTS)
	$(CC) -I. $(LDFLAGS) $(OBJECTS) $(CFLAGS) -o $@ -L$(LIB_DIR) $(LIBS) $(DEFINES)

clean:
	rm -rf $(shell find . -name "*.o") $(EXECUTABLE)
	rm -rf $(shell find . -name "*.optrpt") $(EXECUTABLE)
clean-aux:
	rm -rf $(shell find . -name "*~")
	rm -rf $(shell find . -name "*.*swp")
