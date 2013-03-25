all:
	g++ *.cpp `pkg-config --cflags --libs gtk+-2.0` -lX11 -lXext -lSDL -lSDL_image -lSDLmain -o chess216

clean:
	rm *.o
