# Conway's Game of Life
This is a simple implementation of Conway's Game of Life written in C89 (ANSI C).

### Build
The makefile has two targets: **console** and **pixel**.

To build one of the targets, simply run `make console` or `make pixel`.

The **console** target simply defines CONWAY_CONSOLE and compiles conway.c.
This option requires no dependencies and displays the output of the program through printf().
Press the enter key to progress the simulation, and press Ctrl+C to exit.

The **pixel** target defines CONWAY_PIXEL and compiles conway.c as well as pixel.c.
This option requires SDL2 as a dependency and displays the output of the program through a graphical window.
Press any key on your keyboard to progress the simulation and click on the screen to set any cell at runtime.

### Files
Aside from the Makefile, README.md, and LICENSE files, this repository contains three source files: `conway.c`, `pixel.h`, and `pixel.c`.

`conway.c` contains the entire implementation of Conway's Game of Life as well as the abstracted rendering code for both of
the targets (hidden behind **CONWAY_CONSOLE** and **CONWAY_PIXEL** ifdefs respectively).

`pixel.h` contains the API for the pixel renderer used in the **pixel** target. This api is completely decoupled from SDL2 as I might
go back and implement more backends later as a learning exercise.

`pixel.c` contains the SDL2 implementation of the pixel renderer.
