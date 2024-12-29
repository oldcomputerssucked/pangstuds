unsigned char _speed_lut[512] = {
	90, 90,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	128, 0,
	0, 128,
	90, 90,
	114, 57,
	121, 40,
	124, 31,
	125, 25,
	126, 21,
	126, 18,
	127, 15,
	127, 14,
	127, 12,
	127, 11,
	127, 10,
	127, 9,
	127, 9,
	127, 8,
	0, 128,
	57, 114,
	90, 90,
	106, 71,
	114, 57,
	118, 47,
	121, 40,
	123, 35,
	124, 31,
	124, 27,
	125, 25,
	125, 22,
	126, 21,
	126, 19,
	126, 18,
	126, 16,
	0, 128,
	40, 121,
	71, 106,
	90, 90,
	102, 76,
	109, 65,
	114, 57,
	117, 50,
	119, 44,
	121, 40,
	122, 36,
	123, 33,
	124, 31,
	124, 28,
	125, 26,
	125, 25,
	0, 128,
	31, 124,
	57, 114,
	76, 102,
	90, 90,
	99, 79,
	106, 71,
	111, 63,
	114, 57,
	116, 51,
	118, 47,
	120, 43,
	121, 40,
	122, 37,
	123, 35,
	123, 32,
	0, 128,
	25, 125,
	47, 118,
	65, 109,
	79, 99,
	90, 90,
	98, 81,
	104, 74,
	108, 67,
	111, 62,
	114, 57,
	116, 52,
	118, 49,
	119, 45,
	120, 43,
	121, 40,
	0, 128,
	21, 126,
	40, 121,
	57, 114,
	71, 106,
	81, 98,
	90, 90,
	97, 83,
	102, 76,
	106, 71,
	109, 65,
	112, 61,
	114, 57,
	116, 53,
	117, 50,
	118, 47,
	0, 128,
	18, 126,
	35, 123,
	50, 117,
	63, 111,
	74, 104,
	83, 97,
	90, 90,
	96, 84,
	101, 78,
	104, 73,
	107, 68,
	110, 64,
	112, 60,
	114, 57,
	115, 54,
	0, 128,
	15, 127,
	31, 124,
	44, 119,
	57, 114,
	67, 108,
	76, 102,
	84, 96,
	90, 90,
	95, 85,
	99, 79,
	103, 75,
	106, 71,
	109, 67,
	111, 63,
	112, 60,
	0, 128,
	14, 127,
	27, 124,
	40, 121,
	51, 116,
	62, 111,
	71, 106,
	78, 101,
	85, 95,
	90, 90,
	95, 85,
	99, 81,
	102, 76,
	105, 72,
	107, 69,
	109, 65,
	0, 128,
	12, 127,
	25, 125,
	36, 122,
	47, 118,
	57, 114,
	65, 109,
	73, 104,
	79, 99,
	85, 95,
	90, 90,
	94, 86,
	98, 81,
	101, 78,
	104, 74,
	106, 71,
	0, 128,
	11, 127,
	22, 125,
	33, 123,
	43, 120,
	52, 116,
	61, 112,
	68, 107,
	75, 103,
	81, 99,
	86, 94,
	90, 90,
	94, 86,
	97, 82,
	100, 79,
	103, 75,
	0, 128,
	10, 127,
	21, 126,
	31, 124,
	40, 121,
	49, 118,
	57, 114,
	64, 110,
	71, 106,
	76, 102,
	81, 98,
	86, 94,
	90, 90,
	94, 86,
	97, 83,
	99, 79,
	0, 128,
	9, 127,
	19, 126,
	28, 124,
	37, 122,
	45, 119,
	53, 116,
	60, 112,
	67, 109,
	72, 105,
	78, 101,
	82, 97,
	86, 94,
	90, 90,
	93, 87,
	96, 83,
	0, 128,
	9, 127,
	18, 126,
	26, 125,
	35, 123,
	43, 120,
	50, 117,
	57, 114,
	63, 111,
	69, 107,
	74, 104,
	79, 100,
	83, 97,
	87, 93,
	90, 90,
	93, 87,
	0, 128,
	8, 127,
	16, 126,
	25, 125,
	32, 123,
	40, 121,
	47, 118,
	54, 115,
	60, 112,
	65, 109,
	71, 106,
	75, 103,
	79, 99,
	83, 96,
	87, 93,
	90, 90
};


void rspeed_calc(int dx, int dy, int *sx, int *sy, int speed) {
	int sign_x, sign_y, sz;
	int ent;

	sign_x = (dx < 0) ? -1 : 1;
	dx *= sign_x;
	sign_y = (dy < 0) ? -1 : 1;
	dy *= sign_y;
	sz = dx > dy ? dx : dy;
	if (!sz) {
		*sx = *sy = 0;
		return;
	}

	dx = 15 * dx / sz;
	dy = 15 * dy / sz;
	ent = (dx + (dy << 4)) * 2;
	*sx = (speed * _speed_lut[ent] * sign_x) >> 7;
	*sy = (speed * _speed_lut[ent + 1] * sign_y) >> 7;
	return;
}