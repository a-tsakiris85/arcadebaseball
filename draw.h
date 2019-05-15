#include "game.h"
#include "matrix.h"


void draw_state(game_t * state);
void draw_action(game_t * state);
void draw_result(result_t * current_result);
void delay(int x);
void do_action(void);

extern result_t * current_result;
extern game_t * game;


