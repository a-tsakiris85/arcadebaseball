#include "game.h"
#include <stdint.h>
#include <stdlib.h>

/*
init_game_state() allocates memory for and creates a new instance 
of a game structure with the default values (which correspond to the
start of a baseball game).
*/
game_t * init_game_state() {
	game_t * game = (game_t *) malloc (sizeof(game_t));
	game->score1 = 0;
	game->score2 = 0;
	game->inning = 1;
	game->batter = 1;
	game->balls = 0;
	game->strikes = 0;
	game->outs = 0;
	game->first = 0;
	game->second = 0;
	game->third = 0;
	return game;
}

//val 1,2,3,4 = (single, double, triple, HR), 5=walk
int moveRunners(game_t * state, int val) {
	int runs_scored = 0;
	if(val == 1) {
		if(state->third) {
			runs_scored++;
			state->third = 0;
		}
		state->third = state->second;
		state->second = state->first;
		state->first = 1;
	}
	else if(val == 2) {
		runs_scored += state->third + state->second;
		state->third = state->first;
		state->second =1;
		state->first = 0;
	}
	else if (val == 3) {
		runs_scored += state->third + state->second + state->first;
		state->third = 1;
		state->second = 0;
		state->first = 0;
	}
	else if(val == 4) {
		runs_scored += state->third + state->second + state->first + 1;
		state->third = 0;
		state->second = 0;
		state->first = 0;
	}
	else if(val == 5) {
		if(!state->first) {
			state->first = 1;
		}
		else if(!state->second) {
			state->second = 1;
		}
		else if(!state->third) {
			state->third = 1;
		}
		else {
			runs_scored++;
		}
	}
	
	return runs_scored;
}

void updateGameState(game_t * state, result_t * result) {
	//HIT
	int runs_scored = 0;
	if(result->hit) {
		runs_scored = moveRunners(state, result->hit);
		state->strikes=0;
		state->balls =0;
	}
	//OUT
	if(result->out) {
		state->outs++;
		state->strikes=0;
		state->balls =0;
	}
	else if(result->strike) {
		if(result->strike == 1) {
			state->strikes++;
			if(state->strikes == 3) {
				state->outs++;
				state->strikes = 0;
				state->balls = 0;
			}
		}
		else {
			if(state->strikes < 2) {
				state->strikes++;
			}
		}
	}
	else if(result->ball) {
		if(state->balls == 3) {
			state->balls = 0;
			runs_scored = moveRunners(state, 5);
		}
		else state->balls++;
	}
	
	if(state->batter == 1) {
		state->score1 += runs_scored;
		if(state->outs >= 3) {
			state->batter = 2;
			state->outs = 0;
			state->first = 0;
			state->second = 0;
			state->third = 0;
		}
	}
	else {
		state->score2 += runs_scored;
		if(state->outs >= 3) {
			state->batter = 1;
			state->outs = 0;
			state->first = 0;
			state->second = 0;
			state->third = 0;
			state->inning++; //player 2 bats second.
		}
	}
}