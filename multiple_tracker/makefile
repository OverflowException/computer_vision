#To build executable for raspberry pi, run
#	make RPI=1

TARGET = ./bin/mulititracker

CXX = g++
INC = `pkg-config --cflags opencv` -I./include/ #and other include paths -I
LIB = `pkg-config --libs opencv` $(LIB_RPICAM) #and other lib paths and lib names -L -l

CFLAGS = -Wall -c -std=c++11
LFLAGS = -Wall -pthread

COMPILE = $(CXX) $(CFLAGS) $(INC) $(DEBUG) $^ -o $@
LINK = $(CXX) $(LFLAGS) $(LIB) $^ -o $@


.PHONY: all
all: $(TARGET) #and other targets

$(TARGET): ./build/multitracker.o
	$(LINK)
./build/multitracker.o: ./src/multitracker.cpp
	$(COMPILE)

#[other targets]

$(shell mkdir -p bin)
$(shell mkdir -p build)

clean:
	rm -frv ./build/*.o
