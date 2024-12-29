#include <XLIB\xlib.h>
#include <XLIB\xvsync.h>


#include <string.h>

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
#include "player.h"
#include "screen.h"
#include "mainmenu.h"


enum StateNum state_current, state_next;
static int _fade_count;
static long last_ticks, cur_ticks;
static int _new_room;

int state_init() {
	_fade_count = 0;
	state_current = STATE_NOP;
	state_next = STATE_MAIN_MENU;
	cur_ticks = VsyncIntTicks;

	return 1;
}


static int _wait_vsync() {
	while (StartAddressFlag);
	return 1;
}


static int _draw_loop() {
	while (StartAddressFlag);
	level_room_draw();
	object_draw();
	mouse_draw();
	camera_loop();
	return 1;
}


static int _draw_menu() {
	while (StartAddressFlag);
	mainmenu_draw();
	mouse_draw();
	camera_pos = 0;
	return 1;
}

int state_loop() {
	int i, level;
	char newlevel[16];

	last_ticks = cur_ticks;
	cur_ticks = VsyncIntTicks;

	switch (state_current) {
		case STATE_LEVEL_FADEIN:
			x_page_flip(0, camera_y());
			mouse_loop();
			_draw_loop();
			palette_fade(_fade_count);

			_fade_count += 4;
			if (_fade_count == 64)
				state_next = STATE_LEVEL;
			break;
		case STATE_LEVEL:
			x_page_flip(0, camera_y());

			mouse_loop();
			for (i = 0; i < (VsyncIntTicks - last_ticks); i++)
				object_move();
			_draw_loop();
			if (keys[kESC])
				state_next = STATE_LEVEL_TO_PAUSE;
			if (keys[kF5])
				player_kill(player_object);
			break;
		case STATE_LEVEL_FADEOUT:
		case STATE_LEVEL_TO_PAUSE:
			x_page_flip(0, camera_y());
			mouse_loop();
			_draw_loop();
			palette_fade(64 - _fade_count);

			_fade_count += 4;
			if (_fade_count == 64) {
				if (state_current == STATE_LEVEL_FADEOUT)
					state_next = STATE_LEVEL_LOAD;
				if (state_current == STATE_LEVEL_TO_PAUSE)
					state_next = STATE_PAUSE_FADEIN;
			}
			break;
		case STATE_PAUSE_FADEIN:
/*			x_page_flip(0, 0);*/
			_wait_vsync();
			palette_fade(_fade_count);

			_fade_count += 4;
			if (_fade_count == 64)
				if (state_current == STATE_PAUSE_FADEIN)
					state_next = STATE_LEVEL_PAUSED;
			break;
		case STATE_PAUSE_TO_LEVEL:
		case STATE_PAUSE_TO_EXIT:
/*			x_page_flip(0, 0);
			_wait_vsync();*/
			palette_fade(64 - _fade_count);
			_fade_count += 4;
			if (_fade_count == 64) {
				if (state_current == STATE_PAUSE_TO_LEVEL) {
					for (i = 0; i < 2; i++) {
						x_page_flip(0, 0);
						while (StartAddressFlag);
						level_room_redraw();
						level_room_draw();
					}
					state_next = STATE_LEVEL_FADEIN;
				} else if (state_current == STATE_PAUSE_TO_EXIT)
					state_next = STATE_MAIN_MENU;
				else
					exit_now();
			}
			break;
		case STATE_LEVEL_LOAD:
			state_next = STATE_LEVEL_FADEIN;
			break;
		case STATE_LEVEL_PAUSED:
			if (keys[kESC])
				state_next = STATE_PAUSE_TO_LEVEL;
			else if (keys[kBACKSPACE])
				state_next = STATE_PAUSE_TO_EXIT;
			break;
		case STATE_GAMEOVER:
		case STATE_WIN:
			if (keys[kESC])
				state_next = STATE_MAIN_MENU;
			break;
		case STATE_MAIN_MENU:
			x_page_flip(0, 0);
			mouse_loop();
			mainmenu_loop();
			_draw_menu();
			if (keys[kESC])
				exit_now();
			break;
		default:
			break;
	}

	if (state_current != state_next) {
		switch (state_next) {
			case STATE_LEVEL_FADEIN:
			case STATE_LEVEL_FADEOUT:
			case STATE_LEVEL_TO_PAUSE:
				_fade_count = 0;
				statusbar_update();
				statusbar_show();
				break;
			case STATE_LEVEL:
				palette_update();
				break;
			case STATE_LEVEL_LOAD:
				if (!player_lives) {
					state_next = STATE_GAMEOVER;
					palette_update();
					statusbar_hide();
					screen_gameover_show();
					break;
				}

				if (_new_room % 100 == 99) {
					state_next = STATE_WIN;
					palette_update();
					statusbar_hide();
					screen_win_show();
					break;
				}

				if (_new_room < 0)
					exit_now();
				if (_new_room > 99) {
					level = _new_room / 100;
					sprintf(newlevel, "level%i.lvl", level);
					level_visited |= (1 << (level - 1));
					if (!level_load(newlevel))
						exit_now();
					level_room_enter(_new_room % 100);
				} else
					level_room_enter(_new_room);
				statusbar_update();
				statusbar_show();
				break;
			case STATE_LEVEL_PAUSED:
				break;
			case STATE_PAUSE_FADEIN:
				screen_pause_show();
				statusbar_hide();
				_fade_count = 0;
				break;
			case STATE_PAUSE_TO_LEVEL:
			case STATE_PAUSE_TO_EXIT:
				_fade_count = 0;
				break;
			case STATE_MAIN_MENU:
				statusbar_hide();
				palette_update();
				break;
			default:
				break;
		}
		state_current = state_next;
	}

	return 1;
}


int state_room_enter(int room) {
	_new_room = room;
	state_next = STATE_LEVEL_FADEOUT;

	return 1;
}