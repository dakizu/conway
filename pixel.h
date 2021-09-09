#ifndef PIXEL_H
#define PIXEL_H

typedef void(*PixelMouseCallback)(int,int);

/* Initialization data for this library. Returned from pixel_init. */
typedef struct {
    int window_width;
    int window_height;
    char *window_title;
    int pixel_scale;
    PixelMouseCallback mouse_cb;
} PixelInit;

/* Possible return values for pixel_loop.
    Continue will execute the next frame immediately.
    WaitForInput will execute the next frame when a key is pressed. */
typedef enum { PIXEL_LOOP_CONTINUE, PIXEL_LOOP_WAIT_FOR_INPUT } PixelLoop;

/* This function is called at the very beginning of the program. Use this for initialization. */
PixelInit pixel_init();
/* This function is called once every frame. Use this for core logic and rendering. */
PixelLoop pixel_loop();
/* This function is called at the very end of the program. Use this for deinitialization. */
void pixel_term();

/* A simple machine independent RGB color type. */
typedef struct PixelColor {
    unsigned char r, g, b;
} PixelColor;

/* Some preset colors. Defined inside pixel.c. */
extern const PixelColor PIXEL_COLOR_RED;
extern const PixelColor PIXEL_COLOR_BLACK;

/* The main function for drawing to the screen.
    Works slightly differently when called from within pixel_loop().
    The drawing surface is locked for the entireity of pixel_loop(),
    so calling this function during this time will simply draw the pixel.
    (This is also the reason pixel_refresh_screen() doesn't work
    in the same context.)
    Calling the function outside of pixel_loop() however, will lock and
    unlock the surface with each invocation. This is very inefficient,
    so try to do as much of the drawing in pixel_loop() as possible. */
void pixel_set_color(int x, int y, PixelColor color);
/* For use from within pixel_loop. */
int pixel_is_initial_frame();
/* Force the screen to update. Use when changing the pixels outside of pixel_loop().
    Won't work when called from inside pixel_loop() as it requires the surface
    to be unlocked..*/
void pixel_refresh_screen();

#endif /* PIXEL_H */