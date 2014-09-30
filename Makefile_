CC=g++
CFLAGS=-c -Wall -m32 -O2  -lX11 -lXext -lSDL -lSDL_image -lSDLmain
LDFLAGS=
SOURCES=client.cpp controller.cpp game.cpp gfx.cpp gtk.cpp main.cpp sha256.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=hello

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

(.cpp.o):
    $(CC) $(CFLAGS) $< -o $@
