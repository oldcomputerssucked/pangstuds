#include "tile.h"
#include "logic.h"
#include "level.h"
#include "object.h"
#include "player.h"
#include "rspeed_l.h"
#include "follow.h"
#include "status.h"

#include <XLIB\xlib.h>
#include <XLIB\xmouse.h>


static unsigned char _age[OBJECT_MAX];
static unsigned char _health[OBJECT_MAX];


int logic_init() {
	return 1;
}


static void _player_delta(int my_x, int my_y, int *dx, int *dy) {
	int px, py;
	px = (object_data[player_object].cur.x >> 4) + 5;
	py = (object_data[player_object].cur.y >> 4) + 5;
	*dx = px - my_x;
	*dy = py - my_y;
}


int logic_create(int object, int logic) {
	int dx, dy, sx, sy;
	struct Object *o;
	if (logic < 0 || logic >= LOGIC_MAX)
		return 0;
	o = &object_data[object];

	switch (logic) {
		case LOGIC_BOUNCE:
			o->speed_x = 16;
			o->speed_y = 8;
			break;
		case LOGIC_PLAYER:
			return player_create(object);
		case LOGIC_PLAYER_BULLET:
			_age[object] = 0;
			dx = object_data[player_object].cur.x >> 4;
			dy = object_data[player_object].cur.y >> 4;
			dx += 5;
			dy += 5;
			dx = (mouse_x()) - dx;
			dy = (mouse_y()) - dy;
			rspeed_calc(dx, dy, &sx, &sy, 32);
			o->speed_x = sx;
			o->speed_y = sy;
			break;
		case LOGIC_ENEMY_BULLET:
			if (player_object < 0) {
				object_delete(object);
				break;
			}

			_age[object] = 0;
			dx = (o->cur.x >> 4) + 2;
			dy = (o->cur.y >> 4) + 2;
			dx += 5;
			dy += 5;
			dx = (object_data[player_object].cur.x >> 4) - dx;
			dy = (object_data[player_object].cur.y >> 4) - dy;
			rspeed_calc(dx, dy, &sx, &sy, 32);
			o->speed_x = sx;
			o->speed_y = sy;
			break;
		case LOGIC_ENEMY_TURRET:
			_age[object] = 0;
			_health[object] = 2;
			break;
		case LOGIC_HEDGEHOG:
			follow_init(object);
			_health[object] = 6;
			break;
		case LOGIC_KEYDISK:
			if (level_keydisk & (1 << o->path)) {
				object_delete(object);
			}
			break;
		case LOGIC_LEVER:
			if (level_lever & (1 << o->path)) {
				object_delete(object);
			}
			break;
		case LOGIC_ENEMY_CRAWL_TURRET:
			_age[object] = 0;
			_health[object] = 6;
			follow_init(object);
			break;
		case LOGIC_ENEMY_BAD_TURRET:
			_age[object] = 0;
			_health[object] = 4;
			break;
		case LOGIC_ENEMY_SEEKING_BULLET:
			_age[object] = 0;
			break;
		default:
			break;
	}

	return 1;
}


int logic_loop(int object, int logic) {
	int x, y, dx, dy;

	if (logic < 0 || logic >= LOGIC_MAX)
		return 0;

	switch (logic) {
		case LOGIC_PLAYER:
			return player_loop(object);
		case LOGIC_PLAYER_BULLET:
		case LOGIC_ENEMY_BULLET:
			_age[object]++;
			if (_age[object] > 100)
				object_delete(object);
			break;
		case LOGIC_ENEMY_TURRET:
			_age[object]++;
			if (_age[object] > 60) {
				x = object_data[object].cur.x >> 4;
				y = object_data[object].cur.y >> 4;
				object_create(1, x + 5, y + 5, LOGIC_ENEMY_BULLET, 0);
				_age[object] = 0;
			}
			break;
		case LOGIC_HEDGEHOG:
			follow_path(object, 24);
			break;
		case LOGIC_KEYDISK:
		case LOGIC_LEVER:
			break;
		case LOGIC_ENEMY_CRAWL_TURRET:
			follow_path(object, 24);
			_age[object]++;
			if (_age[object] > 60) {
				x = object_data[object].cur.x >> 4;
				y = object_data[object].cur.y >> 4;
				object_create(1, x + 5, y + 5, LOGIC_ENEMY_BULLET, 0);
				_age[object] = 0;
			}
			break;
		case LOGIC_ENEMY_BAD_TURRET:
			_age[object]++;
			if (_age[object] > 120) {
				x = object_data[object].cur.x >> 4;
				y = object_data[object].cur.y >> 4;
				object_create(2, x + 5, y + 5, LOGIC_ENEMY_SEEKING_BULLET, 0);
				_age[object] = 0;
			}
			break;
		case LOGIC_ENEMY_SEEKING_BULLET:
			_age[object]++;
			if (_age[object] > 100)
				object_delete(object);
			x = object_data[object].cur.x >> 4;
			y = object_data[object].cur.y >> 4;
			if (!(_age[object] & 0x3)) {
				_player_delta(x + 3, y + 3, &dx, &dy);
				rspeed_calc(dx, dy, &dx, &dy, 24);
				object_data[object].speed_x = dx;
				object_data[object].speed_y = dy;
			}
			break;

		default:
			break;
	}

	return 1;
}


int logic_collision(int object, int other, int dx, int dy, int logic) {
	int s;
	struct Object *oth, *o;

	if (logic < 0 || logic >= LOGIC_MAX)
		return 0;

	oth = &object_data[other];
	o = &object_data[object];

	switch (logic) {
		case LOGIC_BOUNCE:
			if (other == -1) {
				if (dx) {
					s = o->speed_x;
					o->speed_x = -s;
				} else if (dy) {
					s = o->speed_y;
					o->speed_y = -s;
				}
				return 1;
			}
			break;
		case LOGIC_PLAYER:
			return player_coll(object, other, dx, dy);
		case LOGIC_ENEMY_TURRET:
		case LOGIC_ENEMY_BAD_TURRET:
			if (other >= 0) {
				if (oth->logic == LOGIC_PLAYER_BULLET) {
					object_delete(other);
					if (_health[object] <= 1)
						object_delete(object);
					_health[object]--;
					return 1;
				}
			}
			return 0;
		case LOGIC_PLAYER_BULLET:
			if (other == -1) {
				s = tile_collision(level_coll_last);
				if (s & TILE_BARRIER)
					return 0;
				object_delete(object);
				return 1;
			} else
				return 0;
		case LOGIC_ENEMY_BULLET:
		case LOGIC_ENEMY_SEEKING_BULLET:
			if (other == -1) {
				s = tile_collision(level_coll_last);
				if (s & TILE_BARRIER)
					return 0;
				object_delete(object);
				return 1;
			} else if (other == player_object) {
				player_kill(player_object);
				object_delete(object);
				return 1;
			} else if (oth->logic == LOGIC_PLAYER_BULLET) {
				object_delete(object);
				object_delete(other);
				return 1;
			} else
				return 0;
		case LOGIC_LEVER:
			if (other == player_object) {
				level_lever |= (1 << o->path);
				s = (o->path << 1) + TILE_DOOR_RED;
				level_tile_replace(s, s+1);
				object_delete(object);
			} else if (oth->logic == LOGIC_PLAYER_BULLET) {
				object_delete(other);
				object_delete(object);
				level_lever |= (1 << o->path);
				s = (o->path << 1) + TILE_DOOR_RED;
				level_tile_replace(s, s+1);
			}
			break;
		case LOGIC_KEYDISK:
			if (other == player_object) {
				level_keydisk |= (1 << o->path);
				s = (o->path << 1) + TILE_DOOR_RED;
				level_tile_replace(s, s+1);
				object_delete(object);
			}
			statusbar_update();
			break;
		case LOGIC_ENEMY_CRAWL_TURRET:
			if (oth->logic == LOGIC_PLAYER_BULLET) {
				if (_health[object] <= 1) {
					object_delete(other);
					object_delete(object);
				} else
					_health[object]--;
			}
			break;
		case LOGIC_HEDGEHOG:
			if (other == player_object)
				player_kill(other);
			if (oth->logic == LOGIC_PLAYER_BULLET) {
				object_delete(other);
				if (_health[object] <= 1)
					object_delete(object);
				_health[object]--;
			}
			break;
		default:
			break;
	}


	return 0;
}


int logic_delete(int object, int logic) {
	if (logic < 0 || logic >= LOGIC_MAX)
		return 0;

	switch (logic) {
		case LOGIC_PLAYER:
			return player_delete(object);
		default:
			break;

	}

	return 1;
}