#include <math.h>
#include "../lib/c8051f310.h"

#include "spi.h"
#include "game.h"
#include "utils.h"


void game_init(Game* game, Player* player)
{
    player->x = HX8340B_LCDWIDTH / 2;
    player->y = HX8340B_LCDHEIGHT - 20;
    player->color = 0xaaaa;
    player->size = 10;

    game->score = 0;
    game->screenPos = 0;
    game->prevScreenPos = 0;
    game->player = player;
    game->over = 0;

    game_drawBackground();
}

void game_update(Game* game, int acce_x, int acce_y)
{
	int screenSpeed;

	if (game->over) {
		// don't update
		return;
	}

	screenSpeed = (-7 + acce_x / 30 <= 0) ? -7 + acce_y / 30 : 0;

	// save previous screen position
	game->prevScreenPos = game->screenPos;

    game->screenPos = positive_modulo((game->screenPos + screenSpeed - 1), SCREEN_SCROLLING_HEIGHT);

    game->player->x += acce_x / 10;
    game->player->y = positive_modulo(game->screenPos + 180, SCREEN_SCROLLING_HEIGHT);

    game->score += 1;

    // check for collision
    if (game->player->x < 0 || game->player->x > HX8340B_LCDWIDTH) {
    	game->over = 1;
    }
}

void game_draw(Game* game)
{
    if (game->over) {
    	game_drawGameOver(game->score);
    }
    else {
    	game_drawGUI(game->score);
		game_drawPlayer(game->player);
		game_drawNewLines(game->screenPos, positive_modulo(game->prevScreenPos - game->screenPos, SCREEN_SCROLLING_HEIGHT));
		screen_verticalScroll(game->screenPos);
    }
}

void game_drawGameOver(int score)
{
	screen_verticalScroll(0);
	screen_fill(0);
}

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



void game_drawNewLines(unsigned short screenPos, unsigned short numlines)
{
	unsigned short x1, x2, y;
	x1 = rand_a_b(20, 30);
	x2 = rand_a_b(130, 150);

	while (numlines--) {
		y = (SCREEN_FIXED_TOP_HEIGHT + screenPos + numlines) % SCREEN_SCROLLING_HEIGHT;
		screen_drawGameLine(y, x1, x2, 0xffff, 0x0000);
	}
}

void game_drawBackground()
{
	// fill top part of the screen with a different color
	screen_fillRect(0, 0, HX8340B_LCDWIDTH, SCREEN_FIXED_TOP_HEIGHT, 0x3405);

	screen_fillRect(0, SCREEN_FIXED_TOP_HEIGHT, HX8340B_LCDWIDTH, SCREEN_SCROLLING_HEIGHT, 0x0000);
}

void game_drawGUI(unsigned long score)
{
	unsigned char digit;
	unsigned char counter = 0;
	while (score != 0) {
		digit = score % 10;
		score /= 10;
		counter++;
		screen_drawNumber(HX8340B_LCDWIDTH - 10 * (counter + 1), 2, digit, 0x1736, 0x3405);
	}
}

void game_drawPlayer(Player* player)
{
	screen_fillRect(player->x - player->size / 2, player->y - player->size / 2, player->size, player->size, player->color);
}
