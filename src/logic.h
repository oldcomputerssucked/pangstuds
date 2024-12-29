#ifndef LOGIC_H_
#define	LOGIC_H_


enum Logic {
	LOGIC_NONE,
	LOGIC_BOUNCE,
	LOGIC_PLAYER,
	LOGIC_PLAYER_BULLET,
	LOGIC_ENEMY_TURRET,
	LOGIC_ENEMY_BULLET,
	LOGIC_HEDGEHOG,
	LOGIC_KEYDISK,
	LOGIC_LEVER,
	LOGIC_ENEMY_CRAWL_TURRET,
	LOGIC_ENEMY_BAD_TURRET,
	LOGIC_ENEMY_SEEKING_BULLET,
	LOGIC_MAX
};


int logic_init();
int logic_create(int object, int logic);
int logic_loop(int object, int logic);
int logic_collision(int object, int dx, int dy, int other, int logic);
int logic_delete(int object, int logic);


#endif