#ifndef PALETTE_H_
#define	PALETTE_H_

#define	PALETTE_BLACK 	1
#define	PALETTE_WHITE	2
#define	PALETTE_GRAY	3
#define	PALETTE_RED	4
#define	PALETTE_GREEN	5


int palette_color_get(unsigned char r, unsigned char g, unsigned char b);
void palette_system_colors_mark();
void palette_reset();
void palette_init();
void palette_fade(unsigned char amount);

#endif