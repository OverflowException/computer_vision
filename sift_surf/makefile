#To build executable for raspberry pi, run
#	make RPI=1

TARGET1 = bin/sift_match
TARGET2 = bin/surf_match

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


.PHONY: all
all: $(TARGET1) $(TARGET2) #and other targets

$(TARGET1): build/sift_match.o
	$(LINK)
$(TARGET2): build/surf_match.o
	$(LINK)

build/sift_match.o: src/sift_match.cpp
	$(COMPILE)
build/surf_match.o: src/surf_match.cpp
	$(COMPILE)


clean:
	rm -frv build/*
