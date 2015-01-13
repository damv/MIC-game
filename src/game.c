#include "../lib/c8051f310.h"
#include "spi.h"
#include "game.h"


void game_drawAccelerometerValues(int x, int y)
{
    // erase lines
    screen_drawFastHLine(0, HX8340B_LCDHEIGHT / 2, HX8340B_LCDWIDTH, 0x0000);
	screen_drawFastVLine(HX8340B_LCDWIDTH / 2, 0, HX8340B_LCDHEIGHT, 0x0000);

    // draw lines
	if (x < 0) {
		screen_drawFastHLine(HX8340B_LCDWIDTH / 2 + x, HX8340B_LCDHEIGHT / 2, -x, 0xffff);
	}
	else {
		screen_drawFastHLine(HX8340B_LCDWIDTH / 2, HX8340B_LCDHEIGHT / 2, x, 0xffff);
	}

	if (y < 0) {
		screen_drawFastVLine(HX8340B_LCDWIDTH / 2, HX8340B_LCDHEIGHT / 2 + y, -y, 0xffff);
	}
	else {
		screen_drawFastVLine(HX8340B_LCDWIDTH / 2, HX8340B_LCDHEIGHT / 2, y, 0xffff);
	}
}

void game_draw(Player player)
{
    game_drawGUI();
    game_drawPlayer(player);
}

void game_drawBackground()
{
	screen_fillRect(10, 10, HX8340B_LCDWIDTH - 20, HX8340B_LCDHEIGHT - 20, 0xf350);
	screen_fillRect(20, 20, HX8340B_LCDWIDTH - 40, HX8340B_LCDHEIGHT - 40, 0x0bd7);
}

void game_drawGUI()
{
	int i;
	for (i = 0; i < 10; i++) {
		screen_drawNumber(2 + 10 * i, 2, i, 0x1736, 0x8276);
	}
}

void game_drawPlayer(Player player)
{
	screen_fillRect(player.x - player.size / 2, player.y - player.size / 2, player.size, player.size, player.color);
}
