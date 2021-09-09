error:
	@echo "Please choose one of the two available build targets:"
	@echo "    1. console - no dependencies, output in console"
	@echo "    2. pixel   - dependency on SDL2, output in dedicated window"

console: conway.c
	cc -ansi -pedantic -g -Wall -DCONWAY_CONSOLE conway.c -o console

# SDL2 has some undesireable defaults on windows, the sed removes those flags
pixel: SDL_CFLAGS:=$(shell pkg-config --cflags SDL2)
pixel: SDL_LIBS:=$(shell pkg-config --libs SDL2 | sed 's/-mwindows//')
pixel: conway.c pixel.c pixel.h
	cc -ansi -pedantic -g -Wall -DCONWAY_PIXEL $(SDL_CFLAGS) conway.c pixel.c $(SDL_LIBS) -o pixel

clean:
	rm console 2> /dev/null
	rm pixel 2> /dev/null