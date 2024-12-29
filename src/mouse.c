#include <stdlib.h>

#include <XLIB\xlib.h>
#include <XLIB\xmouse.h>

#include "level.h"
#include "camera.h"

/*


 ## ##
## # ##
#  #  #
 ## ##
#  #  #
## # ##
 #####

*/

static int last_x, cur_x, last_y, cur_y;

unsigned char mouse_cursor[51] = {
	7, 7,
	0, 4, 4, 0, 4, 4, 0,
	4, 4, 0, 4, 0, 4, 4,
	4, 0, 0, 4, 0, 0, 4,
	0, 4, 4, 0, 4, 4, 0,
	4, 0, 0, 4, 0, 0, 4,
	4, 4, 0, 4, 0, 4, 4,
	0, 4, 4, 4, 4, 4, 0,
};

static char *_cmouse_cursor;

int mouse_init() {
	int sz;
	sz = x_sizeof_cbitmap(ScrnLogicalByteWidth, mouse_cursor);

	if (!(_cmouse_cursor = malloc(sz))) {
		/*printf("Out of memory\n");*/
		return 0;
	}

	x_compile_bitmap(ScrnLogicalByteWidth, mouse_cursor, _cmouse_cursor);
	return 1;
}


int mouse_left() {
	return !!(MouseButtonStatus & LEFT_PRESSED);
}


int mouse_right() {
	return !!(MouseButtonStatus & RIGHT_PRESSED);
}


int mouse_x() {
	return MouseX + 3;
}


int mouse_y() {
	return MouseY + camera_y() + 3;
}


int mouse_loop() {
	level_room_update(last_x, last_y, 7, 7);
	last_x = cur_x;
	last_y = cur_y;
	if (MouseX > 312)
		MouseX = 312;
	if (MouseY > 208)
		MouseY = 208;
	cur_x = MouseX;
	cur_y = MouseY + camera_y();

	return 1;
}


int mouse_draw() {
	x_put_cbitmap(cur_x, cur_y, HiddenPageOffs, _cmouse_cursor);

	return 1;
}