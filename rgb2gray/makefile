TARGET = rgb2gray

CXX = g++
INC = `pkg-config --cflags opencv`
LIB = `pkg-config --libs opencv`

CFLAGS = -Wall -c -std=c++11
LFLAGS = -Wall -pthread

DEBUG = 

COMPILE = $(CXX) $(CFLAGS) $(INC) $(DEBUG) $^ -o $@
LINK = $(CXX) $(LFLAGS) $(LIB) $^ -o $@

$(TARGET): rgb2gray.o
	$(LINK)
rgb2gray.o: rgb2gray.cpp
	$(COMPILE)


clean:
	rm -fv *.o

