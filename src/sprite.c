#include <stdio.h>
#include <stdlib.h>

#include <xlib\xlib.h>
#include <xlib\xcbitmap.h>

#include "level.h"
#include "sprite.h"


struct Sprite {
	void		*csprite;
	unsigned char	w;
	unsigned char	h;
	unsigned char	col_x;
	unsigned char	col_y;
	unsigned char	col_w;
	unsigned char	col_h;
};


struct SpriteData {
	struct Sprite	sprite[SPRITE_MAX];
	int		sprites;
};


static char *_load_buffer;
struct SpriteData sprite_data;

int sprite_init() {
	_load_buffer = calloc(1, 19000);
	if (!_load_buffer) {
		printf("Out of memory\n");
		return 0;
	}

	return 1;
}


int sprite_free() {
	int i;

	for (i = 0; i < sprite_data.sprites; i++) {
		if (!sprite_data.sprite[i].csprite)
			continue;
		free(sprite_data.sprite[i].csprite);
		sprite_data.sprite[i].csprite = NULL;
	}

	return 1;
}


int sprite_load(char *name) {
	FILE *fp;
	int pal_entries, i, j, n;
	unsigned int sz;
	unsigned char pal_lut[256], r, g, b;

	sprite_free();

	if (!(fp = fopen(name, "rb")))
		return 0;
	if (fgetc(fp) != 'S')
		goto error;
	fgetc(fp);
	fgetc(fp);
	sprite_data.sprites = fgetc(fp);
	pal_entries = fgetc(fp);
	if (sprite_data.sprites > SPRITE_MAX) {
		sprite_data.sprites = 0;
		goto error;
	}

	fseek(fp, -3*pal_entries, SEEK_END);
	for (i = 0; i < pal_entries; i++) {
		r = fgetc(fp);
		g = fgetc(fp);
		b = fgetc(fp);
		pal_lut[i] = palette_color_get(r, g, b);
	}

	pal_lut[0] = 0;
	fseek(fp, 5, SEEK_SET);

	for (i = 0; i < sprite_data.sprites; i++) {
		sprite_data.sprite[i].w = fgetc(fp);
		sprite_data.sprite[i].h = fgetc(fp);
		sprite_data.sprite[i].col_x = fgetc(fp);
		sprite_data.sprite[i].col_y = fgetc(fp);
		sprite_data.sprite[i].col_w = fgetc(fp);
		sprite_data.sprite[i].col_h = fgetc(fp);

		n = sprite_data.sprite[i].w;
		n *= sprite_data.sprite[i].h;
		if (n > 18998 || n < 0)
			goto error;
		_load_buffer[0] = sprite_data.sprite[i].w;
		_load_buffer[1] = sprite_data.sprite[i].h;
		fread(&_load_buffer[2], _load_buffer[0], _load_buffer[1], fp);
		for (j = 0; j < n; j++)
			_load_buffer[2 + j] = pal_lut[_load_buffer[2 + j]];
		sz = x_sizeof_cbitmap(ScrnLogicalByteWidth, _load_buffer);
		if (!(sprite_data.sprite[i].csprite = malloc(sz)))
			goto error;
		x_compile_bitmap(ScrnLogicalByteWidth, _load_buffer, sprite_data.sprite[i].csprite);
	}

	fclose(fp);
	return 1;
error:
	fclose(fp);
	return 0;
}


int sprite_cleanup(int sprite, int old_x, int old_y) {
	int w, h;

	if (sprite < 0 || sprite > sprite_data.sprites)
		return 0;

	w = sprite_data.sprite[sprite].w;
	h = sprite_data.sprite[sprite].h;
	return level_room_update(old_x, old_y, w, h);
}


int sprite_draw_statusbar(int sprite, int x, int y) {
	int x2, y2;

	if (sprite < 0 || sprite >= sprite_data.sprites)
		return 0;
	x2 = x + sprite_data.sprite[sprite].w;
	y2 = y + sprite_data.sprite[sprite].h;
	if (x < 0 || y < 0 || x2 >= (ROOM_W << 4) || y2 >= 16)
		return 0;
	if (!sprite_data.sprite[sprite].csprite)
		return 0;
	x_put_cbitmap(x, y, 0, sprite_data.sprite[sprite].csprite);

	return 1;
}


int sprite_draw(int sprite, int x, int y) {
	int x2, y2;

	if (sprite < 0 || sprite >= sprite_data.sprites)
		return 0;
	x2 = x + sprite_data.sprite[sprite].w;
	y2 = y + sprite_data.sprite[sprite].h;
	if (x < 0 || y < 0 || x2 >= (ROOM_W << 4) || y2 >= (ROOM_H << 4))
		return 0;
	if (!sprite_data.sprite[sprite].csprite)
		return 0;
	x_put_cbitmap(x, y, HiddenPageOffs, sprite_data.sprite[sprite].csprite);

	return 1;
}


int sprite_col_get(int sprite, int *x, int *y, int *w, int *h) {
	if (sprite < 0 || sprite >= sprite_data.sprites) {
		*x = *y = 0;
		*w = *h = 1;
		return 0;
	}

	*x = sprite_data.sprite[sprite].col_x;
	*y = sprite_data.sprite[sprite].col_y;
	*w = sprite_data.sprite[sprite].col_w;
	*h = sprite_data.sprite[sprite].col_h;
	return 1;
}