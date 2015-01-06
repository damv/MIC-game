#include "../lib/c8051f310.h"
#include "spi.h"
#include "game.h"

void game_draw(unsigned short x, unsigned short y) {
    unsigned char i = 0, j = 0;

    screen_fillRect(x, y, 10, 10);

    for (i = 1; i < 9; i++) {
        screen_drawPixel(x + i, y + i, 0xffff);
    }
}
