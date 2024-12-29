#ifndef PLAYER_H_
#define	PLAYER_H_

#define	PLAYER_LIVES	5

int player_init();
int player_create(int obj);
int player_loop(int obj);
int player_coll(int obj, int other, int dx, int dy);
int player_delete(int obj);
int player_kill(int obj);

extern int player_object;
extern int player_lives;

#endif