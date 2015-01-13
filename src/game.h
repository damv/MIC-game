#ifndef __GAME_H__
#define __GAME_H__

typedef struct Player {
	unsigned short x;
	unsigned short y;
	unsigned short size;
	unsigned short color;
} Player;

typedef struct Game {
	unsigned int score;
	unsigned int screenPos;
	Player player;
} Game;

void game_init(Game* game);

void game_draw(Game game);
void game_drawBackground();
void game_drawAccelerometerValues(int x, int y);
void game_drawGUI(unsigned long score);
void game_drawPlayer(Player player);
void game_drawNewLines(unsigned short screenPos, unsigned short numlines);


#endif
