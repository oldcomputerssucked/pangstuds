#include <stdio.h>
#include <stdlib.h>
#include <XLIB\xlib.h>
#include <XLIB\xvsync.h>
#include <XLIB\xmouse.h>

#include "key.h"

#include "tile.h"
#include "level.h"
#include "logic.h"
#include "object.h"
#include "sprite.h"
#include "palette.h"
#include "mouse.h"
#include "camera.h"
#include "state.h"

volatile long frame_counter;
volatile char *error_string;

static int _exit_now;

static void vsync_handler() {
	frame_counter++;
}


void exit_now() {
	_exit_now = 1;
}

int main(int argc, char **argv) {
	int i;

	x_text_mode();
	x_mouse_init();
	if (!MouseInstalled) {
		printf("Mouse required\n");
		return 1;
	}
	x_hide_mouse();
	x_mouse_window(0, 0, 312, 208);
	if (!logic_init())
		return 1;
	if (!object_init())
		return 1;
	if (!tile_init())
		return 1;
	if (!sprite_init())
		return 1;
	if (!level_init())
		return 1;
	if (!state_init())
		return 1;

	Set_New_Int9();
	x_set_mode(X_MODE_320x240,340);
	x_set_splitscreen(224);
	x_install_vsync_handler(1);
	x_set_doublebuffer(340);
	palette_init();
	x_text_init();
	x_set_font(1);

	if (!mouse_init())
		goto error;

	if (!level_load("level1.lvl"))
		goto error;
	if (argc == 2) {
		if (!level_load(argv[1]))
			goto error;
	} else if (!level_load("level1.lvl"))
		goto error;
	palette_update();
	level_room_enter(0);

	for (;;) {
		if (_exit_now || keys[kF12])
			break;
		state_loop();
	}

error:
	Set_Old_Int9();
	x_text_mode();
	x_remove_vsync_handler();
	x_mouse_remove();
	printf("Hello.\n");
	return 0;
}