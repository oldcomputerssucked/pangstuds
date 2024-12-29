#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_SEPARATOR "\r\n\t "
static char _input_line[128];
static int sprite_mode, used_colors;
static unsigned char _pixels[40000];
static unsigned char _palette[768], _palette2[768];
static unsigned int _graphics_w, _graphics_h, _tile_w, _tile_h, _tiles;
static unsigned char _collision_data[256];

extern unsigned _stklen = 4096;


static int _get_collision() {
	char *tok;
	unsigned char col;

	col = 0;
	if (!(tok = strtok(NULL, WORD_SEPARATOR)))
		return 0;
	if (strchr(tok, 'L'))
		col |= 1;
	if (strchr(tok, 'R'))
		col |= 2;
	if (strchr(tok, 'U'))
		col |= 4;
	if (strchr(tok, 'D'))
		col |= 8;
	if (strchr(tok, 'B'))
		col |= 16;
	if (strchr(tok, 'H'))
		col |= 32;
	return col;

}

static int _load_graphics() {
	char *fname;
	unsigned char buf[4];
	FILE *fp;

	if (!(fname = strtok(NULL, WORD_SEPARATOR))) {
		printf("Graphics command needs a file name\n");
		return 0;
	}

	if (!(fp = fopen(fname, "rb"))) {
		printf("Could not open graphics file %s\n", fname);
		return 0;
	}

	if (fread(buf, 1, 4, fp) != 4) {
		printf("Graphics file %s too short\n", fname);
		return 0;
	}

	printf("Header: %X %X %X %X\n", buf[0], buf[1], buf[2], buf[3]);
	_graphics_w = ((int) buf[0]) | (((int) buf[1]) << 8);
	_graphics_w++;
	_graphics_h = ((int) buf[2]) | (((int) buf[3]) << 8);
	_graphics_h++;
	printf("Picture is %i x %i\n", _graphics_w, _graphics_h);
	if (((long) _graphics_w) * ((long) _graphics_h) > 40000UL) {
		printf("Graphics file %s too large\n", fname);
		return 0;
	}

	if (fread(_pixels, _graphics_w, _graphics_h, fp) < _graphics_h) {
		printf("Graphics file %s ended unexpectedly\n", fname);
		return 0;
	}

	fseek(fp, -768, SEEK_END);
	if (fread(_palette, 3, 256, fp) < 256) {
		printf("Graphics file %s missing palette\n", fname);
		return 0;
	}

	return 1;
}

static int _add_color(int color) {
	int i;

	if (sprite_mode && color == _pixels[0])
		return 0;

	for (i = sprite_mode; i < used_colors; i++) {
		if (_palette2[i*3] != _palette[color*3])
			continue;
		if (_palette2[i*3+1] != _palette[color*3+1])
			continue;
		if (_palette2[i*3+2] != _palette[color*3+2])
			continue;
		//printf("Found color %i (%i) %i %i %i\n", color, i, _palette[color*3], _palette[color*3+1], _palette[color*3+2]);
		return i;
	}

	printf("Adding color %i\n", color);
	if (used_colors == 255) {
		printf("Too many colors\n");
		exit(1);
	}

	_palette2[i*3] = _palette[color*3];
	_palette2[i*3+1] = _palette[color*3+1];
	_palette2[i*3+2] = _palette[color*3+2];
	used_colors = i + 1;

	return i;
}


static int _write_tile(FILE *fp) {
	int tile_x, tile_y, i, j, q, w, h, col_x, col_y, col_w, col_h;
	char *str;

	if (!(str = strtok(NULL, WORD_SEPARATOR))) {
		printf("Tile needs X and Y position\n");
		return 0;
	}

	tile_x = atoi(str);

	if (!(str = strtok(NULL, WORD_SEPARATOR))) {
		printf("Tile needs X and Y position\n");
		return 0;
	}

	tile_y = atoi(str);

	if (sprite_mode) {
		if (!(str = strtok(NULL, WORD_SEPARATOR))) {
			printf("Sprite tile needs width and height\n");
			return 0;
		}

		w = atoi(str);

		if (!(str = strtok(NULL, WORD_SEPARATOR))) {
			printf("Sprite tile needs width and height\n");
			return 0;
		}

		h = atoi(str);
		_tile_w = w;
		_tile_h = h;

		if (!(str = strtok(NULL, WORD_SEPARATOR))) {
			printf("Sprite tile needs collision X and Y\n");
			return 0;
		}

		col_x = atoi(str);

		if (!(str = strtok(NULL, WORD_SEPARATOR))) {
			printf("Sprite tile needs collision X and Y\n");
			return 0;
		}

		col_y = atoi(str);

		if (!(str = strtok(NULL, WORD_SEPARATOR))) {
			printf("Sprite tile needs collision W and H\n");
			return 0;
		}

		col_w = atoi(str);

		if (!(str = strtok(NULL, WORD_SEPARATOR))) {
			printf("Sprite tile needs collision W and H\n");
			return 0;
		}

		col_h = atoi(str);
	}

	if (!sprite_mode)
		_collision_data[_tiles] = _get_collision();
	else {
		fputc(_tile_w, fp);
		fputc(_tile_w, fp);
		fputc(col_x, fp);
		fputc(col_y, fp);
		fputc(col_w, fp);
		fputc(col_h, fp);
	}

	if (tile_x + _tile_w > _graphics_w) {
		printf("Tile is outside graphics area\n");
		return 0;
	}

	if (tile_y + _tile_h > _graphics_h) {
		printf("Tile is outside graphics area\n");
		return 0;
	}

	printf("Adding tile at %i %i (%i %i)\n", tile_x, tile_y, _tile_w, _tile_h);

	for (i = 0; i < _tile_h; i++)
		for (j = 0; j < _tile_w; j++) {
			q = (i + tile_y) * _graphics_w + (j + tile_x);
			fputc(_add_color(_pixels[q]), fp);
		}

	_tiles++;
	return 1;
}

static int _get_word(FILE *fp, char **word) {
	if (!fgets(_input_line, 128, fp))
		return 0;
	if (!((*word) = strtok(_input_line, WORD_SEPARATOR)))
		return 0;
	return 1;
}

int main(int argc, char **argv) {
	FILE *fp, *out;
	char *cmd;
	unsigned char c;

	used_colors = 0;
	_tiles = 0;

	if (argc < 2) {
		printf("Graphics specification required\n");
		return 1;
	}

	if (!(fp = fopen(argv[1], "r"))) {
		printf("Unable to open %s\n", argv[1]);
		return 1;
	}

	if (!_get_word(fp, &cmd)) {
		printf("Missing graphics mode\n");
		return 1;
	}

	if (!strcmp(cmd, "TILE")) {
		sprite_mode = 0;
		_tile_w = _tile_h = 16;
	} else if (!strcmp(cmd, "SPRITE")) {
		sprite_mode = 1;
		_tile_w = _tile_h = 16;
	} else {
		printf("Invalid graphics mode %s\n", cmd);
		return 1;
	}

	if (!_get_word(fp, &cmd)) {
		printf("Output file missing\n");
		return 1;
	}

	if (!(out = fopen(cmd, "w+b"))) {
		printf("Could not open %s\n", cmd);
		return 1;
	}

	if (sprite_mode)
		putc('S', out);
	else
		putc('G', out);
	fputc(_tile_w, out);
	fputc(_tile_h, out);
	fputc(0, out);
	if (sprite_mode)
		putc(0, out);
	while (_get_word(fp, &cmd)) {
		if (!strcmp(cmd, "GRAPHICS")) {
			if (!_load_graphics())
				goto error;
		} else if (!strcmp(cmd, "TILE")) {
			if (!_write_tile(out))
				goto error;
		} else {
			printf("Unsupported operation %s\n", cmd);
			goto error;
		}
	}

	c = used_colors;
	printf("Used colors: %i\n", used_colors);
	if (!sprite_mode)
		fwrite(&c, 1, 1, out);
	fwrite(_palette2, 3, used_colors, out);
	if (!sprite_mode)
		fwrite(_collision_data, _tiles, 1, out);
	fseek(out, 3, SEEK_SET);
	fputc(_tiles, out);
	if (sprite_mode)
		fputc(c, out);

	fclose(out);

	return 0;
error:
	fclose(out);
	return 1;
}
