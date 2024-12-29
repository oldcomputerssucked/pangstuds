#include"level.h"
#include "object.h"
#include "player.h"

#define SCREEN_HEIGHT	(224)
#define	SCREEN_MID	(112)
#define	CAMERA_DIFF	(80)

int camera_pos;

int camera_loop() {
	struct Object *o;
	int o_y, diff_y;

	if (player_object < 0) {
		return 1;
	}

	o = &object_data[player_object];
	o_y = (o->cur.y >> 4) + 5;
	camera_pos = o_y - SCREEN_MID;

	if (camera_pos < 0)
		camera_pos = 0;
	if (camera_pos > (ROOM_H << 4) - SCREEN_HEIGHT)
		camera_pos = (ROOM_H<<4) - SCREEN_HEIGHT;
	return 1;
}


int camera_y() {
	return camera_pos;
}