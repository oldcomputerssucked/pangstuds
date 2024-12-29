#include <xlib/xlib_all.h>

static unsigned char _palette[768];
static unsigned char _fade_palette[768];
static unsigned char _last_color;
static unsigned char _system_level;

int palette_color_get(unsigned char r, unsigned char g, unsigned char b) {
	int i;

	for (i = 1; i <= _last_color; i++) {
		if (_palette[i*3] != r)
			continue;
		if (_palette[i*3+1] != g)
			continue;
		if (_palette[i*3+2] != b)
			continue;
		return i;
	}

	if (_last_color == 255)
		return 0;
	_palette[i*3] = r;
	_palette[i*3+1] = g;
	_palette[i*3+2] = b;
	_last_color = i;
	return i;
}


void palette_system_colors_mark() {
	_system_level = _last_color;
}


void palette_reset() {
	_last_color = _system_level;
}


void palette_update() {
	x_put_pal_raw(_palette, 256, 0);
}


void palette_fade(unsigned char amount) {
	int i;

	for (i = 0; i < 768; i++)
		_fade_palette[i] = (amount * _palette[i]) >> 6;
	x_put_pal_raw(_fade_palette, 256, 0);
}


void palette_init() {
	_last_color = 0;
	palette_color_get(0, 0, 0);
	palette_color_get(255, 255, 255);
	palette_color_get(128, 128, 128);
	palette_color_get(255, 0, 0);
	palette_color_get(0, 255, 0);
	palette_system_colors_mark();

	return;
}

