#include <stdio.h>

#include <XLIB\xlib.h>
#include <XLIB\xtext.h>

#include "palette.h"
#include "level.h"
#include "state.h"

struct Button {
	int	level;
	int	x;
	int	y;
};

static struct Button _button[6] = {
	{ 1,	40,	90 },
	{ 2,	200,	90 },
	{ 3,	40,	140 },
	{ 4,	200,	140 },
	{ 5,	40,	190 },
	{ 6,	200,	190 }
};


static void _screen_puts_x(char *str, int x, int y, char color) {
	int i, len;

	len = strlen(str);
	for (i = 0; i < len; i++) {
		x_char_put(str[i], x + (i << 3), y, HiddenPageOffs, color);
	}

}


static void _screen_puts(char *str, int y, char color) {
	int i, x, len;

	len = strlen(str);
	x = 160 - (len << 2);
	_screen_puts_x(str, x, y, color);
}


static int _level_visited(int level) {
	return !!(level_visited & (1 << (level - 1)));
}


static void _draw_level_button(int level, int x, int y) {
	char buff[16];

	if (_level_visited(level))
		x_rect_fill(x, y, x + 80, y + 40, HiddenPageOffs, PALETTE_GREEN);
	else
		x_rect_fill(x, y, x + 80, y + 40, HiddenPageOffs, PALETTE_GRAY);

	sprintf(buff, "Level %i", level);

	_screen_puts_x(buff, x + 12, y + 12, PALETTE_WHITE);
}


int mainmenu_loop() {
	int x, y, l, i;

	x = mouse_x();
	y = mouse_y();
	l = mouse_left();

	if (!l)
		return 1;

	for (i = 0; i < 6; i++) {
		if (x < _button[i].x || y < _button[i].y)
			continue;

		if (x > _button[i].x + 80 || y > _button[i].y + 40)
			continue;
		if (!_level_visited(_button[i].level))
			continue;
		break;
	}

	if (i == 6)
		return 1;

	player_init();
	state_room_enter(_button[i].level*100);
	state_next = STATE_LEVEL_LOAD;
	palette_fade(0);

	return 1;
}


int mainmenu_draw() {
	int i;
	x_rect_fill(0, 0, 320, 240, HiddenPageOffs, 0);
	_screen_puts("*** PANGSTUDS ***", 60, PALETTE_WHITE);


	for (i = 0; i < 6; i++)
		_draw_level_button(_button[i].level, _button[i].x, _button[i].y);

	return 1;
}