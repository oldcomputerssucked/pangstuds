#ifndef OBJECT_H_
#define	OBJECT_H_

#define	OBJECT_MAX	(32)

struct ObjectState {
	int		x;
	int		y;
	int		sprite;
};


struct Object {
	struct ObjectState	cur;
	struct ObjectState 	last;
	struct ObjectState	last2;

	int			speed_x;
	int			speed_y;
	int			col_x;
	int			col_y;
	int			col_w;
	int			col_h;

	int			logic;
	int			path;
	int			removal_flag;
};

int object_init();
int object_move();
int object_draw();
int object_create(int sprite, int x, int y, int logic, int path);
int object_delete(int obj);


extern struct Object *object_data;

#endif