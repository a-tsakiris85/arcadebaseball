#include "game.h"
#include <stdint.h>
#include <stdlib.h>

int sw1 = 0; // Switch 1 used for pitch selection
int sw2 = 0; // Switch 2 used for pitch selection

int display_state = 0; // 0: normal display (with score, bases, outs, etc.). 
											 // 1: pitching state
											 // 2: result state
int pitcher_pressed = 0; // Indicates whether or not the pitcher pressed SW3 indicating that they are ready to start their pitch.
int ball_loc = 0; // The position of the ball when the batter swung
int pressed_pitch_loc = -1; // The position at which the pitcher pressed SW2, influencing the speed of the resulting pitch.

int ball_row = -1;
int ball_col = -1;

int batted_row = -1;
int batted_col = -1;

int pitch[30][2];

result_t * current_result;
game_t * game; // used to keep track of the current game state

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

/*
random(high) returns a random number between 1 and high, inclusive
*/
int random(int high) {
	return (rand() % high) + 1;
}

/*
moveRunners(state,val) moves the runners on the bases in `state` according
to the value of `val`. 
val 1,2,3,4 = (single, double, triple, HR), 5=walk
*/
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

/*
updateGameState(state,result) updates `state` based on the outcome of the
current turn as described in `result`.
*/
void updateGameState(game_t * state, result_t * result) {
	//HIT
	int runs_scored = 0;
	if(result->hit) {
		runs_scored = moveRunners(state, result->hit);
		state->strikes=0;
		state->balls =0;
	}
	//OUT
	else if(result->out) {
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


/*
inBox(row,col) returns either 0, 1, 2, or 3 depending on the
location of the position (row,col) relative to the batter's box.
0: not in box
1: edge of the box
2: in the box
3: center of the box
*/
int inBox(int row, int col) {
	if(row < 9 && row > 6 && col < 28 && col > 26) {
		return 3;
	}
	if(row < 9 && row > 6 && col < 29 && col > 25) {
		return 2;
	}
	if(row > 5 && row < 10 && col < 30 && col > 24) {
		return 1;
	}
	return 0;
}


/*
produce_result(row,col,wasBall) updates the current result probabilistically
based on the position at which the batter swung (denoted by (row,col)) and whether
or not the pitch thrown was a ball.
*/
result_t * produce_result(int row, int col, int wasBall) {
	current_result->hit = 0;
	current_result->out = 0;
	current_result->strike = 0;
	current_result->ball = 0;
	int r = random(100);
	if(wasBall == 0) {
		if(batted_row == -1){ //called strike
			current_result->strike = 1;
			return current_result;
		}
		int a = inBox(row,col);
		if(a == 0) {
			current_result->strike = 1;
		}
		else if(a == 1) { //edge
			if(r <30) current_result->strike = 2; // 30% chance of foul ball
			else if (r < 70) current_result->out = 1; // 40% chance of an out
			else if (r < 87) current_result->hit = 1; // 17% chance of a single
			else if (r < 95) current_result->hit = 2; // 8% chance of a double
			else if (r < 99) current_result->hit = 3; // 3% chance of a triple
			else current_result->hit = 5; // 1% chance of a home run
		}
		
		else if (a == 2) { //center
			if (r <10) current_result->strike = 2; // 10% chance of a foul ball
			else if (r < 20) current_result->out = 1; // 10% chance of an out
			else if (r < 55) current_result->hit = 1; // 35% chance of a single
			else if (r < 75) current_result->hit = 2; // 20% chance of a double
			else if (r < 90) current_result->hit = 3; // 15% chance of a triple
			else if (r < 99) current_result->hit = 4; // 10% chance of a home run
			else current_result->hit = 1;
		}
		else if(a == 3) { //sweetspot
			if(r <5) current_result->strike = 2; // 5% chance of a foul ball
			else if (r < 10) current_result->out = 1; // 5% chance of an out
			else if (r < 30) current_result->hit = 1; // 20% chance of a single
			else if (r < 50) current_result->hit = 2; // 20% chance of a double
			else if (r < 70) current_result->hit = 3; // 20% chance of a triple
			else if (r < 99) current_result->hit = 4; // 30% chance of a home run
			else current_result->hit = 1;
		}
	}
	else { //was a ball
		if(batted_row > -1) current_result->strike=1; //swing and miss
		else current_result->ball = 1; //successfully didn't swing.
	}
	return current_result;
}

//meter speed (pitch speed)
int determine_speed(int a) {
	int pitch = (sw2 << 1) + sw1;
	int speed;
	// if pitcher pressed in RED area -> slow pitch
	if(a <= 4 || a >= 11) speed = 5000;
	// if pitcher pressed in YELLOW area -> medium pitch
	if(a <=6 || a >= 9) speed = 3000;
	// if pitcher pressed in GREEN area -> fast pitch
	if(a == 7 || a == 8) speed = 1800;
	
	if(pitch==0) /*fast ball*/ return speed;
	if(pitch==1) /*curve ball*/ return speed * 1.5;
	if(pitch==2) /*knuckle ball*/ return speed * 3.5;
	return 3000;
}

/* sw2 sw1: 00 = fast ball, 01 = curve ball, 10 = knuckle ball, 11 = splitter ball
						
*/
void create_pitch(int sw1, int sw2) {
	int chosen_pitch = (sw2 << 1) + sw1;
	int left;
	switch (chosen_pitch) {
		case 0:
			// fast ball!
			for(int i = 0; i < 50; i++) {
				pitch[i][0] = 0;
				pitch[i][1] = 1;
			}
			break;
		case 1:
			// curve ball!
			left = random(2)-1;
			for(int i = 0; i < 10; i++) {
				left = 0;
				pitch[i][0] = left==0 ? -1 : 1;
				pitch[i][1] = 1;
			}
			for(int i = 10; i < 20; i++) {
				pitch[i][0] = left==0 ? 1 : -1;
				pitch[i][1] = 1;
			}
			for(int i = 20; i < 30; i++) {
				pitch[i][0] = 0;
				pitch[i][1] = 1;
			}
			break;
		case 2:
			// knuckle ball
			for(int i = 0; i < 50; i++) {
				pitch[i][0] = random(3)-2;
				pitch[i][1] = 1;
			}
			break;
		}
}
