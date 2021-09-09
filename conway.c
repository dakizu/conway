#include <stdio.h>
#include <string.h>

#ifdef CONWAY_PIXEL
#include "pixel.h"
#endif

#define BOARD_WIDTH 50
#define BOARD_HEIGHT 50
#define BOARD_SIZE (BOARD_WIDTH * BOARD_HEIGHT)

#define BOARD_SET(x, y) board[((y)*BOARD_WIDTH) + (x)] = 1
#define BOARD_UNSET(x, y) board[((y)*BOARD_WIDTH) + (x)] = 0
#define BOARD_TOGGLE(x, y) (board[((y)*BOARD_WIDTH) + (x)]) ? (BOARD_UNSET(x, y)) : (BOARD_SET(x, y))

#define BOARD_GLIDER(x, y)   \
    BOARD_SET(x - 1, y + 0); \
    BOARD_SET(x + 0, y + 1); \
    BOARD_SET(x + 1, y - 1); \
    BOARD_SET(x + 1, y + 0); \
    BOARD_SET(x + 1, y + 1);

char board[BOARD_SIZE];
char working_board[BOARD_SIZE];

int neighbors(int cell) {
    int res = 0;

    /* The compiler shouls optimize this to a single division. */
    int y = cell / BOARD_WIDTH;
    int x = cell % BOARD_WIDTH;

    int top = y == 0;
    int bottom = y == BOARD_HEIGHT - 1;
    int left = x == 0;
    int right = x == BOARD_WIDTH - 1;

    if (!top) {
        res += board[cell - BOARD_WIDTH]; /* Top cell */
        if (!left) {
            res += board[cell - BOARD_WIDTH - 1]; /* Top-left cell */
        }
        if (!right) {
            res += board[cell - BOARD_WIDTH + 1]; /* Top-right cell */
        }
    }
    if (!bottom) {
        res += board[cell + BOARD_WIDTH]; /* Bottom cell */
        if (!left) {
            res += board[cell + BOARD_WIDTH - 1]; /* Bottom-left cell */
        }
        if (!right) {
            res += board[cell + BOARD_WIDTH + 1]; /* Bottom-right cell */
        }
    }
    if (!left) {
        res += board[cell - 1]; /* Left cell */
    }
    if (!right) {
        res += board[cell + 1]; /* Right cell */
    }

    return res;
}

/* Change the initial state of the simulation here */
void init_board() {
    BOARD_GLIDER(1, 2);
    BOARD_GLIDER(5, 6);
}

void display() {
    int x, y, i;
    for (y = 0; y < BOARD_HEIGHT; ++y) {
        for (x = 0; x < BOARD_WIDTH; ++x) {
            i = (y * BOARD_WIDTH) + x;
#ifdef CONWAY_CONSOLE
            printf(" %c", board[i] ? '*' : '.');
#endif
#ifdef CONWAY_PIXEL
            pixel_set_color(x, y,
                            board[i] ? PIXEL_COLOR_RED : PIXEL_COLOR_BLACK);
#endif
        }
#ifdef CONWAY_CONSOLE
        printf("\n");
#endif
    }
    printf("\n");
}

void step() {
    int i;
    for (i = 0; i < BOARD_SIZE; ++i) {
        int n = neighbors(i);
        if (board[i] == 1 && (n < 2 || n > 3))
            working_board[i] = 0;
        else if (board[i] == 0 && n == 3)
            working_board[i] = 1;
    }
    memcpy(board, working_board, BOARD_SIZE);
}

void init_sync() { memcpy(working_board, board, BOARD_SIZE); }

#ifdef CONWAY_CONSOLE

int main() {
    init_board();
    init_sync();
    display();
    getchar();
    while (1) {
        step();
        display();
        getchar();
    }
}

#endif

#ifdef CONWAY_PIXEL

void mouse_pressed_cb(int x, int y, PixelMouseButton button) {
    if (button == PIXEL_MOUSE_BUTTON_LEFT) BOARD_SET(x, y);
    if (button == PIXEL_MOUSE_BUTTON_RIGHT) BOARD_UNSET(x, y);
    if (button == PIXEL_MOUSE_BUTTON_MIDDLE) BOARD_TOGGLE(x, y);
    init_sync();
    display();
    pixel_refresh_screen();
}

PixelInit pixel_init() {
    PixelInit res;

    init_board();
    init_sync();

    res.window_width = 500;
    res.window_height = 500;
    res.window_title = "Conway's Game of Life";
    res.pixel_scale = 10;
    res.mouse_cb = &mouse_pressed_cb;
    return res;
}

PixelLoop pixel_loop() {
    if (pixel_is_initial_frame()) {
        display();
        return PIXEL_LOOP_WAIT_FOR_INPUT;
    }
    step();
    display();
    return PIXEL_LOOP_WAIT_FOR_INPUT;
}

void pixel_term() {}

#endif
