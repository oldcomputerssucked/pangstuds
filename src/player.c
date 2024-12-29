#include <key.h>
#include <stdlib.h>


#include "tile.h"
#include "logic.h"
#include "level.h"
#include "player.h"
#include "object.h"
#include "mouse.h"
#include "state.h"
#include "status.h"

static int _enter_x, _enter_y, _enter_speed;
int player_object;
int player_lives;
static int shoot;

int player_init() {
	_enter_x = _enter_y = -1;
	player_object = -1;
	player_lives = PLAYER_LIVES;
	return 1;
}


int player_create(int obj) {
	player_object = obj;

	if (_enter_x != -1 && _enter_y != -1) {
		if (_enter_x < 8) {
			object_data[obj].cur.x = ((ROOM_W << 4) - 22) << 4;
			object_data[obj].cur.y = _enter_y << 4;
		} else if (_enter_x >= (ROOM_W << 4) - 20) {
			object_data[obj].cur.x = (8 << 4);
			object_data[obj].cur.y = _enter_y << 4;
		} else if (_enter_y < 8) {
			object_data[obj].cur.x = _enter_x << 4;
			object_data[obj].cur.y = ((ROOM_H << 4) - 22) << 4;
		} else if (_enter_y >= (ROOM_H << 4) - 20) {
			object_data[obj].cur.x = _enter_x << 4;
			object_data[obj].cur.y = (8 << 4);
		}

		object_data[obj].speed_y = _enter_speed;
	}

	return 1;
}


static int _enter_room(struct Object *p, int room) {
	_enter_x = p->cur.x >> 4;
	_enter_y = p->cur.y >> 4;
	_enter_speed = p->speed_y;
	state_room_enter(room);

	return 1;
}


int player_loop(int obj) {
	int fast, sign, px, py;
	struct Object *p;

	fast = mouse_right();
	p = &object_data[obj];
	px = p->cur.x >> 4;
	py = p->cur.y >> 4;
	sign = p->speed_y < 0 ? -1 : 1;

	if (keys[kLEFTCTRL]) {
		p->speed_x = fast ? -16 : -12;
		p->speed_y = fast ? 16*sign : 12*sign;
	} else if (keys[kLEFTALT]) {
		p->speed_x = fast ? 16 : 12;
		p->speed_y = fast ? 16*sign : 12*sign;
	} else {
		p->speed_x = 0;
		p->speed_y = fast ? 24*sign : 16*sign;
	}

	if (!mouse_left())
		shoot = 0;
	if (mouse_left() && !shoot) {
		object_create(1, px + 4, py + 4, LOGIC_PLAYER_BULLET, 0);
		shoot = 1;
	}

	if (px < 8)
		_enter_room(p, level_room_cur->exit_left);
	else if (px + 11 > (ROOM_W << 4) - 8)
		_enter_room(p, level_room_cur->exit_right);
	else if (py < 8)
		_enter_room(p, level_room_cur->exit_up);
	else if (py + 11 > (ROOM_H << 4) - 8)
		_enter_room(p, level_room_cur->exit_down);


	return 1;
}


int player_coll(int obj, int other, int dx, int dy) {
	int sy;

	if (other == -1) {
		if (tile_collision(level_coll_last) & TILE_HARMFUL) {
			player_kill(obj);
		} else if (dy) {
			sy = object_data[obj].speed_y;
			object_data[obj].speed_y = -sy;
		}

		return 1;
	}

	return 0;
}


int player_delete(int obj) {
	player_object = -1;

	return 1;
}



int player_kill(int obj) {
	player_lives--;
	player_delete(obj);
	state_room_enter(0);
	_enter_x = _enter_y = -1;
	statusbar_update();
	return 1;
}