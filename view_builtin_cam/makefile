#To build executable for raspberry pi, run
#	make RPI=1

TARGET = view

CXX = g++
INC = `pkg-config --cflags opencv` #and other include paths -I

ifdef RPI
LIB_RPICAM = -lraspicam_cv
DEBUG = -DRPI_CAM
else
LIB_RPICAM =
DEBUG = 
endif
LIB = `pkg-config --libs opencv` $(LIB_RPICAM) #and other lib paths and lib names -L -l

CFLAGS = -Wall -c -std=c++11
LFLAGS = -Wall -pthread

COMPILE = $(CXX) $(CFLAGS) $(INC) $(DEBUG) $^ -o $@
LINK = $(CXX) $(LFLAGS) $(LIB) $^ -o $@

$(TARGET): view.o
	$(LINK)
view.o: view.cpp
	$(COMPILE)

#$(shell mkdir -p bin)
#$(shell mkdir -p build)

clean:
	rm -frv *.o
#	rm -frv ./build/*.o
