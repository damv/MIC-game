#ifndef __GAME_H__
#define __GAME_H__

typedef struct Player {
	unsigned short x;
	unsigned short y;
	unsigned short size;
	unsigned short color;
} Player;

typedef struct Game {
	unsigned long score;
} Game;


void game_draw(Player player);
void game_drawAccelerometerValues(int x, int y);
void game_drawGUI();
void game_drawPlayer(Player player);

#endif
