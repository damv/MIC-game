#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../lib/c8051f310.h"

#include "spi.h"
#include "game.h"
#include "utils.h"


void game_init(Game* game, Player* player)
{
	unsigned char i;

    player->x = HX8340B_LCDWIDTH / 2;
    player->y = HX8340B_LCDHEIGHT - 20;
    player->color = COLOR_BLUE;
    player->size = 10;

    game->score = 0;
    game->screenPos = SCREEN_SCROLLING_HEIGHT;
    game->prevScreenPos = SCREEN_SCROLLING_HEIGHT;
    game->player = player;
    game->over = 0;

    for (i = 0; i < SCREEN_SCROLLING_HEIGHT; i++) {
    	game->lines[i].x1 = 0;
    	game->lines[i].x2 = HX8340B_LCDWIDTH;
    }

    game_drawBackground();
}

int min(int a, int b) {
	return a < b ? a : b;
}

void game_update(Game* game, int acce_x)
{
	int screenSpeed = -(game->score / 1000);

	// save previous screen position
	game->prevScreenPos = game->screenPos;
    game->screenPos = positive_modulo((game->screenPos + screenSpeed - 1), SCREEN_SCROLLING_HEIGHT);

    game->player->x += (acce_x / 10) - 1; // calibration
    game->player->y = SCREEN_FIXED_TOP_HEIGHT + positive_modulo(game->screenPos + 160, SCREEN_SCROLLING_HEIGHT);

    game->score += -screenSpeed + 1;

    // check for collision
    if (game_isThereCollision(game)) {
    	game->over = 1;
    }
}

void game_draw(Game* game)
{
	screen_verticalScroll(game->screenPos);
	game_drawScore(game->score, HX8340B_LCDWIDTH + 2, 7);
	game_drawNewLines(game);
	game_drawPlayer(game->player);
}

void game_drawGameOver(unsigned long score)
{
	screen_fill(0);
	screen_verticalScroll(0);
	game_drawScore(score, HX8340B_LCDWIDTH / 2 + 20, HX8340B_LCDHEIGHT / 2);
	delay(1000);
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



void game_drawNewLines(Game* game)
{
	unsigned char middle, x1, x2, y, real_y;
	short i;
	float val = (float)game->score / 30.0;
	unsigned short numlines = positive_modulo(game->prevScreenPos - game->screenPos + 1, SCREEN_SCROLLING_HEIGHT);

	middle = sin(val / 5.0) * HX8340B_LCDWIDTH / 5 + HX8340B_LCDWIDTH / 2;

	x1 = middle - (1.5 + sin(val)) * 20.0;
	x2 = middle + (1.5 + cos(val * 1.2)) * 20.0;

	for (i = 0; i <= numlines - 1; i++) {
		y = positive_modulo(game->screenPos - i, SCREEN_SCROLLING_HEIGHT);
		game->lines[y].x1 = x1;
		game->lines[y].x2 = x2;

		real_y = SCREEN_FIXED_TOP_HEIGHT + y;
		screen_drawGameLine(real_y, x1, x2, COLOR_BLACK, COLOR_WHITE);
	}
}

void game_drawBackground()
{
	// fill top part of the screen with a different color
	screen_fillRect(0, 0, HX8340B_LCDWIDTH, SCREEN_FIXED_TOP_HEIGHT, COLOR_RED);

	// draw a rectangle around
	screen_drawFastHLine(0, 0, HX8340B_LCDWIDTH, COLOR_YELLOW);
	screen_drawFastHLine(0, SCREEN_FIXED_TOP_HEIGHT - 1, HX8340B_LCDWIDTH, COLOR_YELLOW);
	screen_drawFastVLine(0, 0, SCREEN_FIXED_TOP_HEIGHT, COLOR_YELLOW);
	screen_drawFastVLine(HX8340B_LCDWIDTH - 1, 0, SCREEN_FIXED_TOP_HEIGHT, COLOR_YELLOW);

	// fill the rest
	screen_fillRect(0, SCREEN_FIXED_TOP_HEIGHT, HX8340B_LCDWIDTH, SCREEN_SCROLLING_HEIGHT, COLOR_WHITE);
}

void game_drawScore(unsigned long score, unsigned char x, unsigned char y)
{
	unsigned char digit;
	unsigned char counter = 0;
	while (score != 0) {
		digit = score % 10;
		score /= 10;
		counter++;
		screen_drawNumber(x - 8 * (counter + 1), y, digit, COLOR_WHITE, COLOR_RED);
	}
}

void game_drawPlayer(Player* player)
{
	unsigned char top = player->y - player->size / 2;
	if (top < SCREEN_FIXED_TOP_HEIGHT) {
		top = SCREEN_FIXED_TOP_HEIGHT;
	}

	screen_fillRect(player->x - player->size / 2, top, player->size, player->size, player->color);
}

unsigned char game_isThereCollision(Game* game)
{
	signed short y = game->player->y;
	signed short x = game->player->x;
	unsigned short halfSize = game->player->size / 2;
	unsigned char x1, x2, index;

	index = y - SCREEN_FIXED_TOP_HEIGHT;

	x1 = (*game->lines)[index].x1;
	x2 = (*game->lines)[index].x2;

	return ( (x1 > x - halfSize) || (x2 < x + halfSize) );
}


