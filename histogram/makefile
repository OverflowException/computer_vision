TARGET_1 = bin/example_histo
TARGET_2 = bin/static_gray_histo
TARGET_3 = bin/static_rgb_histo

CXX = g++
INC = `pkg-config --cflags opencv` #and other include paths -I
LIB = `pkg-config --libs opencv` #and other lib paths and lib names -L -l

CFLAGS = -Wall -c -std=c++11
LFLAGS = -Wall -pthread

DEBUG = 

COMPILE = $(CXX) $(CFLAGS) $(INC) $(DEBUG) $^ -o $@
LINK = $(CXX) $(LFLAGS) $(LIB) $^ -o $@

.PHONY: all
all: $(TARGET_1) $(TARGET_2) $(TARGET_3)

$(TARGET_1): build/example_histo.o
	$(LINK)
build/example_histo.o: src/example_histo.cpp
	$(COMPILE)

$(TARGET_2): build/static_gray_histo.o
	$(LINK)
build/static_gray_histo.o: src/static_gray_histo.cpp
	$(COMPILE)

$(TARGET_3): build/static_rgb_histo.o
	$(LINK)
build/static_rgb_histo.o: src/static_rgb_histo.cpp
	$(COMPILE)

$(shell mkdir -p bin)
$(shell mkdir -p build)

clean:
	rm -frv ./build/*.o
