#ifndef TILE_H_
#define	TILE_H_

#define	TILE_MOVE_LEFT		(1)
#define	TILE_MOVE_RIGHT		(2)
#define	TILE_MOVE_UP		(4)
#define	TILE_MOVE_DOWN		(8)
#define	TILE_BARRIER		(16)
#define	TILE_HARMFUL		(32)

#define	TILE_MAX		(64)

#define	TILE_DOOR_RED		(16)
#define	TILE_DOOR_GREEN		(18)
#define	TILE_DOOR_BLUE		(20)
#define	TILE_DOOR_YELLOW	(22)
#define	TILE_DOOR_PURPLE	(24)
#define	TILE_DOOR_ORANGE	(26)


int tile_collision(int tile);
void tile_draw(int tile, int x, int y);
int tile_init();
int tile_load(char *path);

#endif