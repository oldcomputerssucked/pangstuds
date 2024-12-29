#include <XLIB\xlib.h>
#include <XLIB\xtext.h>

#include <string.h>

#include "palette.h"

static void _screen_puts(char *str, int y, char color) {
	int i, x, len;

	len = strlen(str);
	for (i = 0; i < len; i++) {
		x = 160 - (len << 2) + (i << 3);
		x_char_put(str[i], x, y, HiddenPageOffs, color);
	}

}

int screen_pause_show() {
	int i;

	for (i = 0; i < 4; i++) {
		x_page_flip(0, 0);
		while (StartAddressFlag);
		x_rect_fill(0, 0, 320, 240, HiddenPageOffs, 0);
		_screen_puts("*** PAUSED ***", 80, PALETTE_GRAY);
		_screen_puts("Press ESC to resume", 104, PALETTE_GRAY);
		_screen_puts("Press Backspace to exit", 120, PALETTE_GRAY);
	}

	return 1;
}


int screen_gameover_show() {
	int i;

	for (i = 0; i < 4; i++) {
		x_page_flip(0, 0);
		while (StartAddressFlag);
		x_rect_fill(0, 0, 320, 240, HiddenPageOffs, 0);
		_screen_puts("*** GAME OVER ***", 80, PALETTE_RED);
		_screen_puts("Press ESC to exit", 104, PALETTE_RED);
	}

	return;
}


int screen_win_show() {
	int i;

	for (i = 0; i < 2; i++) {
		x_page_flip(0, 0);
		while (StartAddressFlag);
		x_rect_fill(0, 0, 320, 240, HiddenPageOffs, 0);
		_screen_puts("*** GAME COMPLETED ***", 80, PALETTE_GREEN);
		_screen_puts("Press ESC to exit", 104, PALETTE_GREEN);
	}

	return;
}