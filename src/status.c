#include <XLIB\xlib.h>

#include "sprite.h"
#include "player.h"
#include "level.h"

void statusbar_show() {
	x_show_splitscreen();
}


void statusbar_hide() {
	x_hide_splitscreen();
}


void statusbar_update() {
	int i;

	x_rect_fill(0, 0, 320, 16, 0, 0);

	for (i = 0; i < player_lives; i++)
		sprite_draw_statusbar(0, 2 + i*16, 2);

	for (i = 0; i < 6; i++)
		if (level_keydisk & (1 << i))
			sprite_draw_statusbar(6 + i, 320-16*6 + 2, 2);
}