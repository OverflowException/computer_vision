TARGET = mean_bg_seg

CXX = g++
INC = `pkg-config --cflags opencv` #and other include paths -I
LIB = `pkg-config --libs opencv` #and other lib paths and lib names -L -l

CFLAGS = -Wall -c -std=c++11
LFLAGS = -Wall -pthread

DEBUG = 

COMPILE = $(CXX) $(CFLAGS) $(INC) $(DEBUG) $^ -o $@
LINK = $(CXX) $(LFLAGS) $(LIB) $^ -o $@


.PHONY: all
all: $(TARGET) #and other targets

$(TARGET): mean_bg_seg.o
	$(LINK)
mean_bg_seg.o: mean_bg_seg.cpp
	$(COMPILE)

#$(shell mkdir -p bin)
#$(shell mkdir -p build)

clean:
	rm -frv *.o
#	rm -frv ./build/*.o
