
CC = g++  # compiler
CFLAGS = -std=c++20 -g -I./include -I./test -I/usr/include/FL # compile options
FLTKLIBS = $(shell fltk-config --use-images --ldstaticflags)
LIBS = $(FLTKLIBS) -llua5.3
SOURCES = src/main.cpp src/geometry.cpp src/luaapi.cpp src/optics.cpp src/gui.cpp src/utils.cpp src/panel.cpp   # source files

OBJECTS = $(SOURCES:src/%.cpp=build/%.o)
EXECUTABLE = build/program

all: $(EXECUTABLE)

# link
$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LIBS) 

# compile
build/%.o: src/%.cpp
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)