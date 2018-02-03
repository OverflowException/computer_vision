#To build executable for raspberry pi, run
#	make RPI=1

TARGET_1 = bin/static_mean_seg
TARGET_2 = bin/dynamic_mean_seg

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
all: $(TARGET_1) $(TARGET_2)#and other targets

$(TARGET_1): build/static_mean_seg.o
	$(LINK)
build/static_mean_seg.o: src/static_mean_seg.cpp
	$(COMPILE)

$(TARGET_2): build/dynamic_mean_seg.o
	$(LINK)
build/dynamic_mean_seg.o: src/dynamic_mean_seg.cpp
	$(COMPILE)


$(shell mkdir -p bin)
$(shell mkdir -p build)

clean:
#	rm -frv *.o
	rm -frv ./build/*.o
