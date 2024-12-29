#ifndef SPRITE_H_
#define	SPRITE_H_


#define	SPRITE_MAX	(20)

int sprite_init();
int sprite_free();
int sprite_load(char *name);
int sprite_cleanup(int sprite, int old_x, int old_y);
int sprite_draw(int sprite, int x, int y);
int sprite_draw_statusbar(int sprite, int x, int y);
int sprite_col_get(int sprite, int *x, int *y, int *w, int *h);



#endif