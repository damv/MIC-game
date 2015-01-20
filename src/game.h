#ifndef __GAME_H__
#define __GAME_H__

typedef struct Player {
	signed short x;
	signed short y;
	unsigned short size;
	unsigned short color;
} Player;

typedef struct Game {
	unsigned int score;
	unsigned int screenPos;
	unsigned int prevScreenPos;
	unsigned short over;
	Player* player;
} Game;

void game_init(Game* game, Player* player);
void game_update(Game* game, int acce_x, int acce_y);
void game_draw(Game* game);

void game_drawGameOver(int score);
void game_drawBackground();
void game_drawAccelerometerValues(int x, int y);
void game_drawGUI(unsigned long score);
void game_drawPlayer(Player* player);
void game_drawNewLines(Game* game, unsigned short numlines);

#endif
