#include <stdio.h>
#include <stdlib.h>

#include <xlib\xlib.h>
#include <xlib\xvsync.h>

#include "tile.h"
#include "sprite.h"
#include "level.h"
#include "object.h"
#include "player.h"

struct Level *level_data;
struct Room *level_room_cur;
unsigned char level_coll_last;
unsigned char level_lever;
unsigned char level_keydisk;
unsigned char level_visited;

extern volatile char *error_string;

static int _read_word(FILE *fp) {
	unsigned char h, l;

	l = fgetc(fp);
	h = fgetc(fp);
	return (((int) h) << 8) | ((int) l);
}


int level_init() {
	FILE *fp;
	if (!(level_data = calloc(1, sizeof(*level_data)))) {
		printf("Out of memory\n");
		return 0;
	}

	level_room_cur = &level_data->room[0];
	level_visited |= 1;

	if (!!(fp = fopen("PANGSTUD.SAV", "rb"))) {
		level_keydisk = fgetc(fp);
		level_visited = fgetc(fp);
		fclose(fp);
	}

	return 1;
}


int level_load(char *name) {
	FILE *fp;
	struct ObjectD *obj;
	struct Path *path;
	char fname[13];
	int i, j, k;

	if (!!(fp = fopen("PANGSTUD.SAV", "wb"))) {
		fputc(level_keydisk, fp);
		fputc(level_visited, fp);
		fclose(fp);
	}

	if (!(fp = fopen(name, "rb")))
		return 0;
	fread(fname, 12, 1, fp);
	fname[12] = 0;
	if (!tile_load(fname))
		goto error;
	fread(fname, 12, 1, fp);
	fname[12] = 0;
	if (!sprite_load(fname))
		goto error;

	level_data->rooms = fgetc(fp);
	if (level_data->rooms > LEVEL_ROOM_MAX)
		return 0;
	for (i = 0; i < level_data->rooms; i++) {
		level_room_cur = &level_data->room[i];
		fread(level_room_cur->room_data, ROOM_W, ROOM_H, fp);
		level_room_cur->exit_left = _read_word(fp);
		level_room_cur->exit_right = _read_word(fp);
		level_room_cur->exit_up = _read_word(fp);
		level_room_cur->exit_down = _read_word(fp);

		level_room_cur->objects = fgetc(fp);
		if (level_room_cur->objects > LEVEL_OBJECT_MAX)
			goto error;
		obj = &level_room_cur->object[0];
		for (j = 0; j < level_room_cur->objects; j++) {
			obj[j].sprite = fgetc(fp);
			obj[j].x = _read_word(fp);
			obj[j].y = _read_word(fp);
			obj[j].logic = fgetc(fp);
			obj[j].path = fgetc(fp);
		}

		level_room_cur->paths = fgetc(fp);
		if (level_room_cur->paths > LEVEL_PATH_MAX)
			goto error;
		path = &level_room_cur->path[0];
		for (j = 0; j < level_room_cur->paths; j++) {
			path[j].coords = fgetc(fp);
			if (path[j].coords > LEVEL_PATH_COORD_MAX)
				goto error;
			for (k = 0; k < path[j].coords; k++)
				path[j].coord[k] = _read_word(fp);
		}
	}

	level_lever = 0;
	for (i = 0; i < 6; i++)
		if (level_keydisk & (1 << i)) {
			j = TILE_DOOR_RED + (i<<1);
			level_tile_replace(j, j + 1);
		}
	/*player_init();*/
	fclose(fp);
	return 1;
error:
	fclose(fp);
	return 0;
}


int level_room_enter(int room) {
	int i;
	struct ObjectD *o;

	if (room < 0 || room > level_data->rooms)
		return 0;
	object_clear();
	level_room_cur = &level_data->room[room];

	for (i = 0; i < 2; i++) {
		x_page_flip(0, 0);
		while (StartAddressFlag);
		level_room_redraw();
	}


	for (i = 0; i < level_room_cur->objects; i++) {
		o = &level_room_cur->object[i];
		object_create(o->sprite, o->x, o->y, o->logic, o->path);
	}

	return 1;
}


int level_room_draw() {
	int i, j, k;
	unsigned char t;

	for (i = k = 0; i < ROOM_H; i++) {
		for (j = 0; j < ROOM_W; j++, k++) {
			t = level_room_cur->room_data[k];
			if (!(t & LEVEL_FLAG_DRAW))
				continue;
			tile_draw(t & LEVEL_TILE_MASK, j << 4, i << 4);
			level_room_cur->room_data[k] &= LEVEL_TILE_MASK;
		}
	}
	return 1;
}


int level_room_redraw() {
		int i, j, k;
	unsigned char t;

	for (i = k = 0; i < ROOM_H; i++) {
		for (j = 0; j < ROOM_W; j++, k++) {
			t = level_room_cur->room_data[k];
			tile_draw(t & LEVEL_TILE_MASK, j << 4, i << 4);
		}
	}
	return 1;
}


int level_room_update(int x, int y, int w, int h) {
	unsigned char tx, ty, tx2, ty2;
	unsigned char *room_row;
	int i, j, k;

	w -= 1;
	h -= 1;
	if (x < 0 || y < 0)
		return 0;
	if (x + w > (ROOM_W << 4) || y + h > (ROOM_H << 4))
		return 0;

	tx = (x >> 4);
	ty = (y >> 4);
	tx2 = ((x + w) >> 4);
	ty2 = ((y + h) >> 4);


	for (i = ty; i <= ty2; i++) {
		room_row = &level_room_cur->room_data[i * ROOM_W];
		for (j = tx; j <= tx2; j++)
			room_row[j] |= LEVEL_FLAG_DRAW;
	}

	return 1;
}


int level_tile_replace(unsigned char old, unsigned char new) {
	int i, j;
	unsigned char *rdata;
	struct Room *room;

	for (i = 0; i < level_data->rooms; i++) {
		rdata = &level_data->room[i].room_data[0];
		for (j = 0; j < ROOM_W*ROOM_H; j++)
			if ((rdata[j] & LEVEL_TILE_MASK) == old)
				rdata[j] = new;
	}

	return 1;
}


int level_coll(int x, int y, int w, int h, int dx, int dy) {
	unsigned char tx, ty, tx2, ty2;
	int xpx, ypx, coll;
	unsigned char flag, c;
	int i, t;

	xpx = (x + dx) >> 4;
	ypx = (y + dy) >> 4;
	level_coll_last = 255; /* Special value for room boundary */

	if (xpx < 0 || ypx < 0)
		return 1;
	if (xpx + w > (ROOM_W << 4) || ypx + h > (ROOM_H << 4))
		return 1;
	if (!dx && !dy)
		return 0;

	if (dx < 0) {
		flag = TILE_MOVE_LEFT;
		tx = (x + dx) >> 8;
		tx2 = (x >> 8);
		if (tx == tx2)
			return 0;
		ty = ypx >> 4;
		ty2 = (ypx + h) >> 4;
	} else if (dx > 0) {
		flag = TILE_MOVE_RIGHT;
		tx = (xpx + w) >> 4;
		tx2 = ((x >> 4) + w) >> 4;
		if (tx == tx2)
			return 0;
		ty = ypx >> 4;
		ty2 = (ypx + h) >> 4;
	} else if (dy < 0) {
		flag = TILE_MOVE_UP;
		ty = (y + dy) >> 8;
		ty2 = (y >> 8);
		if (ty == ty2)
			return 0;
		tx = xpx >> 4;
		tx2 = (xpx + w) >> 4;
	} else {
		flag = TILE_MOVE_DOWN;
		ty = (ypx + h) >> 4;
		ty2 = ((y >> 4) + h) >> 4;
		if (ty == ty2)
			return 0;
		tx = xpx >> 4;
		tx2 = (xpx + w) >> 4;
	}

	t = ty * ROOM_W + tx;
	coll = 0;

	if (dx) {
		for (i = ty; i <= ty2; i++, t += ROOM_W) {
			c = level_room_cur->room_data[t] & LEVEL_TILE_MASK;
			if (tile_collision(c) & flag) {
				if (tile_collision(c) & 0xF0) {
					level_coll_last = c;
					coll = !coll ? 2 : coll;
				} else
					coll = 1;
			}
		}
	} else {
		for (i = tx; i <= tx2; i++, t++) {
			c = level_room_cur->room_data[t] & LEVEL_TILE_MASK;
			if (tile_collision(c) & flag) {
				if (tile_collision(c) & 0xF0) {
					level_coll_last = c;
					coll = !coll ? 2 : coll;
				} else
					coll = 1;
			}
		}
	}

	if (coll == 1)
		level_coll_last = 255;

	return coll;
}