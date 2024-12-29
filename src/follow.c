#include "level.h"
#include "follow.h"
#include "object.h"
#include "rspeed_l.h"

static int _cur_node[OBJECT_MAX];


int follow_init(int object) {
	_cur_node[object] = 0;
	return 1;
}


int follow_path(int object, int speed) {
	struct Object *o;
	struct Path *p;
	int dx, dy, sx, sy, n, path;

	o = &object_data[object];
	path = o->path;
	if (path < 0 || path >= level_room_cur->paths)
		return 0;
	p = &level_room_cur->path[path];

	n = _cur_node[object];
	dx = p->coord[n << 1] - (o->cur.x >> 4);
	dy = p->coord[(n << 1) + 1] - (o->cur.y >> 4);
	if (!dx && !dy)
		_cur_node[object]++;
	if (_cur_node[object] >= (p->coords >> 1))
		_cur_node[object] = 0;
	if (!dx && !dy)
		return 1;

	rspeed_calc(dx, dy, &sx, &sy, speed);
	dx <<= 4;
	dy <<= 4;

	if (abs(dx) < abs(sx))
		sx = dx;
	if (abs(dy) < abs(sy))
		sy = dy;
	o->speed_x = sx;
	o->speed_y = sy;

	return 1;
}