#ifndef __GAME_H__
#define __GAME_H__

typedef struct Player {
	signed short x;
	signed short y;
	unsigned short size;
	unsigned short color;
} Player;

typedef struct GameLine {
	unsigned char x1;
	unsigned char x2;
} GameLine;

typedef struct Game {
	unsigned long score;
	unsigned int screenPos;
	unsigned int prevScreenPos;
	unsigned short over;
	Player* player;
	GameLine lines[SCREEN_SCROLLING_HEIGHT];
} Game;

void game_init(Game* game, Player* player);
void game_update(Game* game, int acce_x);
void game_draw(Game* game);

void game_drawGameOver(unsigned long score);
void game_drawBackground();
void game_drawAccelerometerValues(int x, int y);
void game_drawScore(unsigned long score, unsigned char x, unsigned char y);
void game_drawPlayer(Player* player);
void game_drawNewLines(Game* game);
unsigned char game_isThereCollision(Game* game);

#endif
