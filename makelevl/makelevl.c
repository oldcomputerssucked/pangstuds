#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	WORD_SEPARATOR	"\r\n\t ,"
#define	WRITE_WORD(word, fp) (putc(word & 0xFF, fp), putc(word >> 8, fp))

unsigned char _tile_translation[256];


struct Path {
	int		coordinate[20];
	int		coordinates;
};

struct Object {
	int		sprite;
	int 		x;
	int		y;
	int		logic;
	int		path;
};

struct Room {
	unsigned char	room_data[20*20];
	unsigned short	exit_left;
	unsigned short	exit_right;
	unsigned short	exit_up;
	unsigned short	exit_down;
	struct Object	object[10];
	int		objects;
	struct Path	path[6];
	int		paths;
};

struct Room _room_data[20];
static char _line_buffer[128];
static int _cur_room;
static int _cur_object;
static int _cur_path;

static char *_get_word(FILE *fp) {
	if (!(fgets(_line_buffer, 128, fp)))
		return NULL;
	return strtok(_line_buffer, WORD_SEPARATOR);
}

static int _do_room(FILE *fp) {
	int i, j, tile;
	char *room_str;

	if (_cur_room == 20) {
		printf("Too many rooms\n");
		return 0;
	}

	for (i = 0; i < 20; i++) {
		if (!(room_str = _get_word(fp))) {
			printf("Missing room data\n");
			return 0;
		}

		if (strlen(room_str) != 20) {
			printf("Room data incorrect length\n");
			return 0;
		}

		for (j = 0; j < 20; j++) {
			tile = _tile_translation[room_str[j]];
			_room_data[_cur_room].room_data[i*20+j] = tile;
		}
	}
	_room_data[_cur_room].objects = _cur_object;
	_room_data[_cur_room].paths = _cur_path;
	_cur_room++;
	_cur_object = 0;
	_cur_path = 0;
	return 1;
}

static int _get_num(int *num) {
	char *tok;

	if (!(tok = strtok(NULL, WORD_SEPARATOR)))
		return 0;
	*num = atoi(tok);
	return 1;
}

static int _do_object() {
	int sprite, x, y, logic, path;

	if (_cur_object == 10) {
		printf("Too many objects\n");
		return 0;
	}

	if (!_get_num(&sprite) || !_get_num(&x) || !_get_num(&y) ||
		!_get_num(&logic) || !_get_num(&path)) {
		printf("Object needs sprite, x, y, logic and path\n");
		return 0;
	}

	_room_data[_cur_room].object[_cur_object].sprite = sprite;
	_room_data[_cur_room].object[_cur_object].x = x;
	_room_data[_cur_room].object[_cur_object].y = y;
	_room_data[_cur_room].object[_cur_object].logic = logic;
	_room_data[_cur_room].object[_cur_object].path = path;

	_cur_object++;

	return 1;
}


static int _do_assign() {
	int ch, tile;
	char *chs, *tiles;

	chs = strtok(NULL, WORD_SEPARATOR);
	tiles = strtok(NULL, WORD_SEPARATOR);
	if (!chs || !tiles) {
		printf("ASSIGN requires a character and a tile number\n");
		return 0;
	}

	_tile_translation[*chs] = atoi(tiles);
	return 1;
}


static int _do_exit(int num) {
	int n;

	if (!_get_num(&n)) {
		printf("Exit needs a destination level and room\n");
		return 0;
	}

	if (num == 0)
		_room_data[_cur_room].exit_left = n;
	else if (num == 1)
		_room_data[_cur_room].exit_right = n;
	else if (num == 2)
		_room_data[_cur_room].exit_up = n;
	else if (num == 3)
		_room_data[_cur_room].exit_down = n;
	else
		return 0;
	return 1;
}


int _do_path() {
	int i, c;

	if (_cur_path == 6) {
		printf("Too many paths\n");
		return 0;
	}

	for (i = 0; i < 20; i++) {
		if (!_get_num(&c))
			break;
		_room_data[_cur_room].path[_cur_path].coordinate[i] = c;
	}

	_room_data[_cur_room].path[_cur_path].coordinates = i;
	_cur_path++;
	return 1;
}


void _write_level(FILE *out) {
	int i, j, k, n, c;

	putc(_cur_room, out);

	for (i = 0; i < _cur_room; i++) {
		fwrite(_room_data[i].room_data, 400, 1, out);
		WRITE_WORD(_room_data[i].exit_left, out);
		WRITE_WORD(_room_data[i].exit_right, out);
		WRITE_WORD(_room_data[i].exit_up, out);
		WRITE_WORD(_room_data[i].exit_down, out);
		putc(_room_data[i].objects, out);
		for (j = 0; j < _room_data[i].objects; j++) {
			putc(_room_data[i].object[j].sprite, out);
			WRITE_WORD(_room_data[i].object[j].x, out);
			WRITE_WORD(_room_data[i].object[j].y, out);
			putc(_room_data[i].object[j].logic, out);
			putc(_room_data[i].object[j].path, out);
		}

		putc(_room_data[i].paths, out);
		for (j = 0; j < _room_data[i].paths; j++) {
			n = _room_data[i].path[j].coordinates;
			putc(n, out);
			for (k = 0; k < n; k++) {
				c = _room_data[i].path[j].coordinate[k];
				WRITE_WORD(c, out);
			}
		}
	}
}

int main(int argc, char **argv) {
	FILE *fp, *out;
	char *cmd, fname[13];

	if (argc < 2) {
		printf("Level specification needed\n");
		return 1;
	}

	if (!(fp = fopen(argv[1], "r"))) {
		printf("Can't open %s\n", argv[1]);
		return 1;
	}

	if (!(cmd = _get_word(fp))) {
		printf("Output file required\n");
		return 1;
	}

	if (!(out = fopen(cmd, "w+b"))) {
		printf("Can't open output %s\n", argv[1]);
		return 1;
	}

	if (!(cmd = _get_word(fp))) {
		printf("Tile data required\n");
		return 1;
	}

	if (strlen(cmd) > 12) {
		printf("Tile data file name too long\n");
		return 1;
	}

	strcpy(fname, cmd);
	fwrite(fname, 12, 1, out);

	if (!(cmd = _get_word(fp))) {
		printf("Sprite data required\n");
		return 1;
	}

	if (strlen(cmd) > 12) {
		printf("Sprite data file name too long\n");
		return 1;
	}

	strcpy(fname, cmd);
	fwrite(fname, 12, 1, out);

	while (!(feof(fp))) {
		if (!(cmd = _get_word(fp)))
			continue;
		if (!(strcmp(cmd, "ASSIGN"))) {
			if (!(_do_assign()))
				break;
		} else if (!(strcmp(cmd, "ROOMDATA"))) {
			if (!(_do_room(fp)))
				break;
		} else if (!(strcmp(cmd, "OBJECT"))) {
			if (!(_do_object()))
				break;
		} else if (!(strcmp(cmd, "PATH"))) {
			if (!(_do_path()))
				break;
		} else if (!(strcmp(cmd, "EXITL"))) {
			if (!(_do_exit(0)))
				break;
		} else if (!(strcmp(cmd, "EXITR"))) {
			if (!(_do_exit(1)))
				break;
		} else if (!(strcmp(cmd, "EXITU"))) {
			if (!(_do_exit(2)))
				break;
		} else if (!(strcmp(cmd, "EXITD"))) {
			if (!(_do_exit(3)))
				break;
		} else if (!(strcmp(cmd, "REM"))) {
		} else {
			printf("Unhandled command %s\n", cmd);
			break;
		}
	}

	if (!feof(fp))
		return 1;

	_write_level(out);
	fclose(out);

	return 0;
}