#include <stdlib.h>

#include "logic.h"
#include "object.h"


struct Object *object_data;

int object_clear() {
	int i;

	for (i = 0; i < OBJECT_MAX; i++) {
		object_data[i].cur.sprite = -1;
		object_data[i].last = object_data[i].cur;
		object_data[i].last2 = object_data[i].cur;

	}
	return 1;
}


int object_init() {
	if (!(object_data = malloc(sizeof(*object_data) * OBJECT_MAX)))
		return 0;
	return object_clear();
}


static int _handle_coll(int obj, int other, int dx, int dy) {
	if (object_data[obj].removal_flag)
		return 0;
	logic_collision(other, obj, 0, 0, object_data[other].logic);
	return logic_collision(obj, other, dx, dy, object_data[obj].logic);
}


static int _check_obj_coll(int obj, int new_x, int new_y) {
	int i, other_x, other_y, this_x, this_y, collision;
	struct Object *this, *other;

	this = &object_data[obj];
	this_x = new_x + this->col_x;
	this_y = new_y + this->col_y;

	collision = 0;

	for (i = 0; i < OBJECT_MAX; i++) {
		if (i == obj)
			continue;
		other = &object_data[i];
		if (other->cur.sprite < 0)
			continue;
		other_x = (other->cur.x >> 4) + other->col_x;
		other_y = (other->cur.y >> 4) + other->col_y;
		if (other_x + other->col_w < this_x)
			continue;
		if (other_y + other->col_h < this_y)
			continue;
		if (this_x + this->col_w < other_x)
			continue;
		if (this_y + this->col_h < other_y)
			continue;
		if (_handle_coll(obj, i, 0, 0))
			collision = 1;
	}

	return collision;
}


static void _do_move(int obj, int dx, int dy) {
	int max_dx, max_dy, step_x, step_y;
	int i, cur_x, cur_y, last_x, last_y;
	struct Object *o;

	o = &object_data[obj];
	max_dx = max_dy = 15; /* Can't move by more than a tile */
	if (o->col_w - 1 < max_dx)
		max_dx = o->col_w - 1;
	if (o->col_h - 1 < max_dy)
		max_dy = o->col_h - 1;


	/* Needs to be in 12.4 fixed point format */
	max_dx <<= 4;
	max_dy <<= 4;

	step_x = (dx < 0) ? -max_dx : max_dx;
	step_y = (dy < 0) ? -max_dy : max_dy;

	for (i = dx; i; i -= step_x) {
		if (abs(i) < max_dx)
			step_x = i;

		cur_x = (o->cur.x + step_x);
		last_x = o->cur.x;
		cur_y = o->cur.y;
		if (cur_x == last_x)
			goto skip_col_x;

		if (level_coll(last_x, cur_y, o->col_w, o->col_h, step_x, 0))
			if (_handle_coll(obj, -1, step_x, 0))
				goto skip_move_x;

		if (_check_obj_coll(obj, cur_x >> 4, cur_y >> 4))
			goto skip_move_x;

	skip_col_x:
		o->cur.x += step_x;

	skip_move_x:
		continue;
	}

	for (i = dy; i; i -= step_y) {
		if (abs(i) < max_dy)
			step_y = i;

		cur_y = (o->cur.y + step_y);
		last_y = o->cur.y;
		cur_x = o->cur.x;
		if (cur_y == last_y)
			goto skip_col_y;

		if (level_coll(cur_x, last_y, o->col_w, o->col_h, 0, step_y))
			if (_handle_coll(obj, -1, 0, step_y))
				goto skip_move_y;

		if (_check_obj_coll(obj, cur_x >> 4, cur_y >> 4))
			goto skip_move_y;

	skip_col_y:
		o->cur.y += step_y;

	skip_move_y:
		continue;
	}

	return;
}


int object_move() {
	int i, clean;
	struct ObjectState cur, last;

	for (i = 0; i < OBJECT_MAX; i++) {
		if (object_data[i].cur.sprite == -1)
			goto nomove;
		if (object_data[i].removal_flag) {
			object_data[i].cur.sprite = -1;
			logic_delete(i, object_data[i].logic);
			clean = 1;
			goto doclean;
		}

		logic_loop(i, object_data[i].logic);
		_do_move(i, object_data[i].speed_x, object_data[i].speed_y);

	nomove:
		clean = 0;
		if (object_data[i].last2.sprite == -1)
			goto noclean;
	doclean:
		cur = object_data[i].cur;
		last = object_data[i].last2;
		if (cur.sprite != last.sprite)
			clean = 1;
		if ((cur.x >> 4) != (last.x >> 4))
			clean = 1;
		if ((cur.y >> 4) != (last.y >> 4))
			clean = 1;
		if (!clean)
			goto noclean;

		sprite_cleanup(last.sprite, last.x >> 4, last.y >> 4);
	noclean:
		continue;

	}

	return 1;
}


int object_draw() {
	int i, x, y;

	for (i = 0; i < OBJECT_MAX; i++) {
		object_data[i].last2 = object_data[i].last;
		object_data[i].last = object_data[i].cur;

		if (object_data[i].cur.sprite < 0)
			continue;
		if (object_data[i].removal_flag)
			continue;
		x = object_data[i].cur.x >> 4;
		y = object_data[i].cur.y >> 4;
		sprite_draw(object_data[i].cur.sprite, x, y);
	}

	return 1;
}


int object_create(int sprite, int x, int y, int logic, int path) {
	int i, col_x, col_y, col_w, col_h;

	for (i = 0; i < OBJECT_MAX; i++) {
		if (object_data[i].cur.sprite >= 0)
			continue;
		if (!sprite_col_get(sprite, &col_x, &col_y, &col_w, &col_h))
			return -1;
		object_data[i].removal_flag = 0;
		object_data[i].col_x = col_x;
		object_data[i].col_y = col_y;
		object_data[i].col_w = col_w;
		object_data[i].col_h = col_h;
		object_data[i].cur.x = x << 4;
		object_data[i].cur.y = y << 4;
		object_data[i].speed_x = 0;
		object_data[i].speed_y = 0;
		object_data[i].cur.sprite = sprite;
		object_data[i].logic = logic;
		object_data[i].path = path;
		logic_create(i, logic);
		return i;
	}

	return -1;
}


int object_delete(int obj) {
	if (obj < 0 || obj >= OBJECT_MAX)
		return 0;
	object_data[obj].removal_flag = 1;
	return 1;
}