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
	GameLine line = { 0, 0 };

    player->x = HX8340B_LCDWIDTH / 2;
    player->y = HX8340B_LCDHEIGHT - 20;
    player->color = COLOR_CYAN;
    player->size = 10;

    game->score = 0;
    game->screenPos = SCREEN_SCROLLING_HEIGHT;
    game->prevScreenPos = SCREEN_SCROLLING_HEIGHT;
    game->player = player;
    game->over = 0;

    for (i = 0; i < SCREEN_SCROLLING_HEIGHT; i++) {
    	game->lines[i] = line;
    }

    game_drawBackground();
}

int min(int a, int b) {
	return a < b ? a : b;
}

void game_update(Game* game, int acce_x, int acce_y)
{
	int screenSpeed = min(-7 + acce_y / 40, 0);

	// save previous screen position
	game->prevScreenPos = game->screenPos;
    game->screenPos = positive_modulo((game->screenPos + screenSpeed - 1), SCREEN_SCROLLING_HEIGHT);

    game->player->x += acce_x / 10;
    game->player->y = SCREEN_FIXED_TOP_HEIGHT + positive_modulo(game->screenPos + 160, SCREEN_SCROLLING_HEIGHT);

    game->score += 1;

    // check for collision
    if (game_isThereCollision) {
    	game->over = 1;
    }
}

void game_draw(Game* game)
{
	screen_verticalScroll(game->screenPos);
	game_drawGUI(game->score);
	game_drawNewLines(game);
	game_drawPlayer(game->player);
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



void game_drawNewLines(Game* game)
{
	unsigned short x1, x2, y;
	float val = (float)game->score / 20.0;
	unsigned short numlines = positive_modulo(game->prevScreenPos - game->screenPos + 1, SCREEN_SCROLLING_HEIGHT);

	x1 = (1 + sin(val)) * 30.0;
	x2 = HX8340B_LCDWIDTH - (1 + cos(val * 1.2)) * 30.0;

	while (numlines--) {
		y = SCREEN_FIXED_TOP_HEIGHT + positive_modulo((game->screenPos - numlines), SCREEN_SCROLLING_HEIGHT);
		screen_drawGameLine(y, x1, x2, 0xffff, 0x0000);
	}
}

void game_drawBackground()
{
	// fill top part of the screen with a different color
	screen_fillRect(0, 0, HX8340B_LCDWIDTH, SCREEN_FIXED_TOP_HEIGHT, COLOR_RED);
	screen_fillRect(0, SCREEN_FIXED_TOP_HEIGHT, HX8340B_LCDWIDTH, SCREEN_SCROLLING_HEIGHT, COLOR_BLACK);
}

void game_drawGUI(unsigned long score)
{
	unsigned char digit;
	unsigned char counter = 0;
	while (score != 0) {
		digit = score % 10;
		score /= 10;
		counter++;
		screen_drawNumber(HX8340B_LCDWIDTH - 10 * (counter + 1), 2, digit, COLOR_WHITE, COLOR_RED);
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

bool game_isThereCollision(Game* game)
{
	signed short y = game->player->y;
	signed short x = game->player->x;
	unsigned int screenPos = game->screenPos; 
	GameLine line = game->lines[ (screenPos+y) %SCREEN_SCROLLING_HEIGHT ]

	unsigned short demiSize = game->player.size /2;
	return ( (line.x1 > x-demiSize) || (line.x2 < x+demiSize) ) 
}


