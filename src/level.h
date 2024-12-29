#ifndef LEVEL_H_
#define	LEVEL_H_

#define	LEVEL_PATH_MAX	(6)
#define	LEVEL_PATH_COORD_MAX (20)
#define	LEVEL_OBJECT_MAX (10)
#define	LEVEL_ROOM_MAX	(20)
#define	ROOM_W		(20)
#define	ROOM_H		(20)

#define	LEVEL_TILE_MASK	(0x3F)
#define	LEVEL_FLAG_DRAW	(0x80)

struct Path {
	int		coord[LEVEL_PATH_COORD_MAX];
	int		coords;
};


struct ObjectD {
	unsigned char	sprite;
	unsigned char	path;
	unsigned char	logic;
	int		x;
	int		y;

};


struct Room {
	unsigned char	room_data[ROOM_W*ROOM_H];
	struct ObjectD	object[LEVEL_OBJECT_MAX];
	int		objects;
	struct Path	path[LEVEL_PATH_MAX];
	int		paths;

	int		exit_left;
	int		exit_right;
	int		exit_up;
	int		exit_down;
};


struct Level {
	struct Room	room[LEVEL_ROOM_MAX];
	int		rooms;
};


int level_room_enter(int room);
int level_load(char *name);
int level_init();
int level_room_draw();
int level_room_redraw();
int level_room_update(int x, int y, int w, int h);
int level_tile_replace(unsigned char old, unsigned char new);
int level_coll(int x, int y, int w, int h, int dx, int dy);



extern struct Level *level_data;
extern struct Room *level_room_cur;
extern unsigned char level_coll_last;
extern unsigned char level_lever;
extern unsigned char level_keydisk;
extern unsigned char level_visited;


#endif