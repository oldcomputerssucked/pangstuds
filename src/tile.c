#include <stdio.h>
#include <stdlib.h>

#include <xlib\xlib.h>
#include "tile.h"
#include "palette.h"

struct Tile {
	unsigned char	w;
	unsigned char	h;
	unsigned char	data[256];
};

static struct Tile *_tile;
static unsigned char _collision[TILE_MAX];

int tile_collision(int tile) {
	if (tile >= TILE_MAX || tile < 0)
		return 0xF;
	return _collision[tile];
}


void tile_draw(int tile, int x, int y) {
	x_put_pbm(x, y, HiddenPageOffs, &_tile[tile]);
}


int tile_init() {
	if (!(_tile = malloc(TILE_MAX * sizeof(*_tile)))) {
		printf("Insufficient memory\n");
		return 0;
	}

	return 1;
}


int tile_load(char *path) {
	FILE *fp;
	unsigned char data[258], r, g, b;
	unsigned char pal_lut[256];
	unsigned int i, j, tiles, colors;

	palette_reset();

	if (!(fp = fopen(path, "rb"))) {
		return 0;
	}

	if (fgetc(fp) != 'G') {
		return 0;
	}

	if (fgetc(fp) != 16) {
		return 0;
	}

	if (fgetc(fp) != 16) {
		return 0;
	}

	if ((tiles = fgetc(fp)) > TILE_MAX) {
		return 0;
	}

	fseek(fp, 4 + tiles * 256, SEEK_SET);
	colors = fgetc(fp);
	for (i = 0; i < colors; i++) {
		r = fgetc(fp);
		g = fgetc(fp);
		b = fgetc(fp);
		pal_lut[i] = palette_color_get(r, g, b);
	}

	fread(&_collision, 1, tiles, fp);
	fseek(fp, 4, SEEK_SET);
	data[0] = 16;
	data[1] = 16;
	for (i = 0; i < tiles; i++) {
		fread(&data[2], 1, 256, fp);
		for (j = 0; j < 256; j++)
			data[2+j] = pal_lut[data[2+j]];
		x_bm_to_pbm(data, &_tile[i]);
	}

	fclose(fp);
	return 1;
}