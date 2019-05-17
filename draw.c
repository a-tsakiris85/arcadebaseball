#include "game.h"
#include "matrix.h"

void delay(int x) {
	disable_interrupt();
	for(int i = 0; i < x/100; i++) {
		for(int j = 0; j < 100; j++);
		updateDisplay();
	}
	enable_interrupt();
}

extern int pitch[100][2];
extern int sw1;
extern int sw2;
extern int display_state;
extern int pitcher_pressed;
extern int ball_loc;
extern int pressed_pitch_loc;

extern int ball_row;
extern int ball_col;

extern int batted_row;
extern int batted_col;

extern result_t * current_result;
extern game_t * game;


/*
	Draws the state of the game onto the matrix panel based on the input `state`
*/
void draw_state(game_t * state) {
	// Used to clear all LEDs on the board 
	fillRect(0,0,32,16,get_color(CLEAR));
	//Bases
	fillRect(1,7,2,2,state->first ? get_color(YELLOW) : get_color(WHITE));
	fillRect(7,1,2,2,state->second ? get_color(YELLOW) : get_color(WHITE));
	fillRect(13,7,2,2,state->third ? get_color(YELLOW) : get_color(WHITE));
	// Home plate
	fillRect(6,13,2,4,get_color(WHITE));
	drawPixel(7,15, get_color(WHITE));
	drawPixel(8,15, get_color(WHITE));
	
	//Balls
	for(int i = 0; i < state->balls; i++) {
		drawPixel(14-i,18,get_color(GREEN));
	}
	//Strikes
	for(int i = 0; i< state->strikes; i++) {
		drawPixel(14-i,19,get_color(RED));
	}
	//Outs
	for(int i =0; i < state->outs; i++) {
		drawPixel(1+i,18,get_color(RED));
	}
	
	// Draws the LEDs which correspond to the current inning
	fillRect(13 - state->inning + 1, 21, 1, state->inning, get_color(BLUE));
	fillRect(13 - state->inning + 2,22,1,state->inning-1,get_color(RED));
	if(state->batter == 2) {
		drawPixel(13 - state->inning + 1, 22, get_color(RED));
	}
	
	// Draws yellow turnstile to indicate which player is currently at bat
	if(state->batter == 1) {
		fillRect(7,27,3,1,get_color(YELLOW));
		drawPixel(8,28,get_color(YELLOW));	
	}
	else {
		fillRect(8,27,3,1,get_color(YELLOW));
		drawPixel(7,28,get_color(YELLOW));
	}
	// draws the current score of both players
	draw_num(state->score1, 11, 25, get_color(BLUE));
	draw_num(state->score2, 1, 25, get_color(RED));
}

/*
draw_action(state) is called after the better presses SW2 to indicate that they
are ready to begin the game. This function draws the mound, the batter's box, and
the pitcher's bar, and Red and Blue lines to indicate which player is at bat and
which player is pitching (determined by the input `state`).
*/
void draw_action(game_t * state) {
	// first clear all the LEDs on the board
	fillRect(0,0,32,16,get_color(CLEAR));
	
	// draw pitching / batting colors
	color_t * pitching_color = state->batter == 1 ? get_color(RED) : get_color(BLUE);
	color_t * batting_color = state->batter == 2 ? get_color(RED) : get_color(BLUE);
	fillRect(0,0,1,16,pitching_color);
	fillRect(0,31,1,16,batting_color);
	
	//Mound and box.
	fillRect(5,3,2,6,get_color(WHITE));
	fillRect(6,25,5,4,get_color(YELLOW));
	fillRect(7,26,3,2,get_color(CLEAR));
	
	//Bar
	fillRect(2,6,1,12,get_color(RED));
	fillRect(5,6,1,6,get_color(YELLOW));
	fillRect(7,6,1,2,get_color(GREEN));
}

void draw_result(result_t * current_result) {
	// first clear all the LEDs on the board
	fillRect(0,0,32,16,get_color(CLEAR));
	if (current_result->strike==1) {
		// draw an 'S'
		fillRect(6,0,1,5, get_color(WHITE));
		fillRect(10,0,4,1, get_color(WHITE));
		fillRect(6,3,1,5, get_color(WHITE));
		fillRect(6,3,4,1, get_color(WHITE));
		fillRect(6,6,1,5, get_color(WHITE));
		// draw a 'T'
		fillRect(6,11,1,5,get_color(WHITE));
		fillRect(8,11,6,1,get_color(WHITE));
		// draw an 'R'
		fillRect(6,19,1,5,get_color(WHITE));
		drawPixel(6,20,get_color(WHITE));
		fillRect(10,19,7,1, get_color(WHITE));
		fillRect(6,21,1,5,get_color(WHITE));
		drawPixel(9,22,get_color(WHITE));
		drawPixel(8,23,get_color(WHITE));
		drawPixel(7,24,get_color(WHITE));
		drawPixel(6,25,get_color(WHITE));
	}
	else if (current_result->strike==2) {
		// draw an 'F'
		fillRect(6,0,1,4,get_color(WHITE));
		fillRect(10,0,7,1,get_color(WHITE));
		fillRect(8,3,1,2,get_color(WHITE));
		// draw an 'O'
		fillRect(7,9,1,3,get_color(WHITE));
		fillRect(6,10,5,1,get_color(WHITE));
		fillRect(10,10,5,1,get_color(WHITE));
		fillRect(7,15,1,3,get_color(WHITE));
		// draw a 'U'
		fillRect(10,18,5,1,get_color(WHITE));
		fillRect(6,18,5,1,get_color(WHITE));
		fillRect(7,23,1,3,get_color(WHITE));
		// draw an 'L'
		fillRect(10,25,6,1,get_color(WHITE));
		fillRect(6,30,1,4,get_color(WHITE));
	}
	else if (current_result->ball) {
		// draw a 'B'
		fillRect(7,0,1,4,get_color(WHITE));
		fillRect(6,1,2,1,get_color(WHITE));
		fillRect(10,0,7,1,get_color(WHITE));
		fillRect(7,3,1,4,get_color(WHITE));
		fillRect(6,4,2,1,get_color(WHITE));
		fillRect(7,6,1,4,get_color(WHITE));
		// draw an 'A'
		fillRect(7,8,1,3,get_color(WHITE));
		fillRect(10,9,6,1,get_color(WHITE));
		fillRect(6,9,6,1,get_color(WHITE));
	  fillRect(6,11,1,5,get_color(WHITE));
		// draw one 'L'
		fillRect(10,16,6,1,get_color(WHITE));
		fillRect(6,21,1,5,get_color(WHITE));
		// draw another 'L'
		fillRect(10,23,6,1,get_color(WHITE));
		fillRect(6,28,1,5,get_color(WHITE));
	}
	else if (current_result->out) {
		// draw an 'O'
		fillRect(7,1,1,3,get_color(WHITE));
		fillRect(10,2,5,1,get_color(WHITE));
		fillRect(6,2,5,1,get_color(WHITE));
		fillRect(7,7,1,3,get_color(WHITE));
		// draw a 'U'
		fillRect(10,10,6,1,get_color(WHITE));
		fillRect(6,10,6,1,get_color(WHITE));
		fillRect(7,16,1,3,get_color(WHITE));
		// draw a 'T'
		fillRect(6,19,1,5,get_color(WHITE));
		fillRect(8,19,6,1,get_color(WHITE));
	}
  else {
		if(current_result->hit==4) {
			// first clear all the LEDs on the board
			fillRect(0,0,32,16,get_color(CLEAR));
			// draw an 'H'
			fillRect(6,1,7,1,get_color(WHITE));
			fillRect(7,4,1,3,get_color(WHITE));
			fillRect(10,1,7,1,get_color(WHITE));
			// draw an 'O'
			fillRect(7,9,1,3,get_color(WHITE));
			fillRect(6,10,5,1,get_color(WHITE));
			fillRect(10,10,5,1,get_color(WHITE));
			fillRect(7,15,1,3,get_color(WHITE));
			// draw an 'M'
			fillRect(10,17,6,1,get_color(WHITE));
			fillRect(6,17,6,1,get_color(WHITE));
			drawPixel(9,18,get_color(WHITE));
			drawPixel(8,19,get_color(WHITE));
			drawPixel(7,18,get_color(WHITE));
			// draw an 'E'
			fillRect(10,24,7,1,get_color(WHITE));
			fillRect(6,24,1,4,get_color(WHITE));
			fillRect(7,27,1,3,get_color(WHITE));
			fillRect(6,30,1,4,get_color(WHITE));
			delay(100000);
		
			// first clear all the LEDs on the board
			fillRect(0,0,32,16,get_color(CLEAR));
			// draw an 'R'
			fillRect(7,1,1,3,get_color(WHITE));
			fillRect(6,2,2,1,get_color(WHITE));
			fillRect(10,1,7,1,get_color(WHITE));
			fillRect(7,4,1,3,get_color(WHITE));
			drawPixel(8,5,get_color(WHITE));
			drawPixel(7,6,get_color(WHITE));
			drawPixel(6,7,get_color(WHITE));
			// draw a 'U'
			fillRect(6,10,6,1,get_color(WHITE));
			fillRect(7,16,1,3,get_color(WHITE));
			fillRect(10,10,6,1,get_color(WHITE));
			// draw an 'N'
			fillRect(6,19,7,1,get_color(WHITE));
			fillRect(10,19,7,1,get_color(WHITE));
			drawPixel(9,20,get_color(WHITE));
			drawPixel(8,21,get_color(WHITE));
			drawPixel(7,22,get_color(WHITE));
		}
		else draw_num(current_result->hit,6,13,get_color(WHITE));	
	}
	
	delay(100000);

}

/*
do_action() performs the main game simulation. It waits until the pitcher presses SW2
*/
void do_action() {
		while(pitcher_pressed == 0){} //Wait for pitcher to press while he does his switches.
			// pitcher may be adjusting switches to choose their pitch
		sw1 = (PTC->PDIR & (1<<10)) >> 10;
		sw2 = (PTC->PDIR & (1<<11)) >> 11; // get input values from sw1 an sw2 to determine which pitch to throw
		pitcher_pressed = 0;
			
		//DO pitcher reaction game.
		ball_loc = 3;
		int count = 0;
		pressed_pitch_loc = -1;
		
		// Go through this while loop 8 times, waiting for 
		// the pitcher to react. If they don't react in time,
		// throw the pitch anyways
		while(count < 16 && pressed_pitch_loc < 0) {
			// while the ball is still traveling left
			while(ball_loc < 13 && pressed_pitch_loc < 0) {
				drawPixel(ball_loc, 7, get_color(WHITE));
				delay(3500);
				drawPixel(ball_loc, 7, get_color(CLEAR));
				ball_loc++;
			}
			// while the ball is traveling right
			while(ball_loc > 2 && pressed_pitch_loc < 0) {
				drawPixel(ball_loc, 7, get_color(WHITE));
				delay(3500);
				drawPixel(ball_loc, 7, get_color(CLEAR));
				ball_loc--;
			}
			count++;
		}
		int meter_speed = determine_speed(pressed_pitch_loc);
		drawPixel(pressed_pitch_loc, 7, get_color(WHITE));
		delay(10000);
		// clear the pitcher's bar
		fillRect(0,5,3,16,get_color(CLEAR));
		for(int i = 0; i < 3; i++) {
			drawPixel(7,5,get_color(WHITE));
			delay(10000);
			drawPixel(7,5,get_color(CLEAR));
			delay(10000);
		}
		//Would normally create a more interesting pitch based on pitcher_pressed_loc and 2 switches.
		//For now will just do a fastball.
		create_pitch(sw1,sw2);
		int chosenPitch = (sw2 << 1) + sw1;
		ball_row = 7;
		ball_col = 5;
		batted_col = -1; // TODO: use for balls
		batted_row = -1;
		
		// animate the ball travelling towards the batter's box
		int wasBall = 1;
		int i = 0;
		while(ball_col < 31 && batted_row < 0) {
			drawPixel(ball_row, ball_col, get_color(WHITE));
			if(chosenPitch==3) {
				if(ball_col < 20) delay(meter_speed);
				else delay(1000);
			}
			else delay(meter_speed);
			drawPixel(ball_row, ball_col, get_color(CLEAR));
			fillRect(6,25,5,4,get_color(YELLOW));
			fillRect(7,26,3,2,get_color(CLEAR));
			ball_col += pitch[i][1];
			if(ball_col < 0) ball_col = 0;
			if(ball_col > 31) ball_row = 31;
			
			ball_row += pitch[i][0];
			if(ball_row < 0) ball_row = 0;
			if(ball_row > 15) ball_row = 15;
			if (inBox(ball_row,ball_col) > 0) wasBall = 0;
			i++;
		}
		
		// If the batter did swing, draw the pixel corresponding to where the ball was when they swung
		if(batted_row > -1) {
			drawPixel(batted_row, batted_col, get_color(WHITE));
		}
		else {
			drawPixel(ball_row, ball_col, get_color(WHITE));
		}
		
		delay(100000);
		display_state = 2; // display result state
		result_t* result = produce_result(batted_row, batted_col, wasBall);
		draw_result(current_result);
		updateGameState(game, current_result);
		draw_state(game);
}
