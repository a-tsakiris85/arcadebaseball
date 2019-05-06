
//Try stopping the interrupts, doing them by hand during the loop, and then resuming them.

#include "matrix.h"
#include "game.h"

/*
	Draws the state of the game onto the matrix panel.
*/
void draw_state(game_t * state) {

	fillRect(0,0,32,16,get_color(CLEAR));
	//Bases
	fillRect(1,7,2,2,state->first ? get_color(YELLOW) : get_color(WHITE));
	fillRect(7,1,2,2,state->second ? get_color(YELLOW) : get_color(WHITE));
	fillRect(13,7,2,2,state->third ? get_color(YELLOW) : get_color(WHITE));
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
	
	//Base arrow
	//fillRect(4,28,1,3,get_color(YELLOW));
	//Batter
	fillRect(13 - state->inning + 1, 21, 1, state->inning, get_color(BLUE));
	fillRect(13 - state->inning + 2,22,1,state->inning-1,get_color(RED));
	if(state->batter == 2) {
		drawPixel(13 - state->inning + 1, 22, get_color(RED));
	}
	
	if(state->batter == 1) {
		fillRect(7,27,3,1,get_color(YELLOW));
		drawPixel(8,28,get_color(YELLOW));	
	}
	else {
		fillRect(8,27,3,1,get_color(YELLOW));
		drawPixel(7,28,get_color(YELLOW));
		
	}

	draw_num(state->score1, 11, 25, get_color(BLUE));
	draw_num(state->score2, 1, 25, get_color(RED));
}



int sw1= 8; //dummy value. Will later be used for pitch selection.
int sw2 = 5; //dummy value. Later used for pitch selection.
int c = 5; //debug
int b = 1; //debug

int display_state = 0; //0: normal display. 1: pitching state.
int pitcher_pressed =0;
int ball_loc = 0;
int pressed_pitch_loc = -1;

int ball_row = -1;
int ball_col = -1;

int batted_row = -1;
int batted_col = -1;

result_t * current_result;
game_t * game;

void delay(int x) {
	c = 1;
	disable_interrupt();
	for(int i = 0; i < x/100; i++) {
		for(int j = 0; j < 100; j++);
		updateDisplay();
	}
	enable_interrupt();
	c = 0;
}

//Draw the setup for a pitch
void draw_action(game_t * state) {
	fillRect(0,0,32,16,get_color(CLEAR));
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

//0 = not in box.
//1 = edge
//2 = in box
//3 = center.
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
//random number [1,high]
int random(int high) {
	return (rand() % high) + 1;
}

//produces result based on when the swing was.
void produce_result(int row, int col, int wasBall) {
	current_result->hit = 0;
	current_result->out = 0;
	current_result->strike = 0;
	current_result->ball = 0;
	int r = random(100);
	if(wasBall == 0) {
		if(batted_row == -1){ //called strike
			current_result->strike = 1;
			return;
		}
		int a = inBox(row,col);
		if(a == 0) {
			current_result->strike = 1;
		}
		if(a == 1) { //edge
			if(r <30) current_result->strike = 2;
			else if (r < 70) current_result->out = 1;
			else if (r < 87) current_result->hit = 1;
			else if (r < 95) current_result->hit = 2;
			else if (r < 99) current_result->hit = 4;
			else current_result->hit = 3;
		}
		else if (a == 2) { //center
			if(r <10) current_result->strike = 2;
			else if (r < 20) current_result->out = 1;
			else if (r < 60) current_result->hit = 1;
			else if (r < 90) current_result->hit = 2;
			else if (r < 99) current_result->hit = 4;
			else current_result->hit = 3;
		}
		else if(a == 3) { //sweetspot
			if(r <5) current_result->strike = 2;
			else if (r < 10) current_result->out = 1;
			else if (r < 30) current_result->hit = 1;
			else if (r < 60) current_result->hit = 2;
			else if (r < 99) current_result->hit = 4;
			else current_result->hit = 3;
		}
	}
	else { //was a ball
		if(batted_row > -1) current_result->strike=1; //swing and miss
		else current_result->ball = 1; //successfully didn't swing.
	}

	
}
//meter speed (pitch speed)
int determine_speed(int a) {
	if(a < 5 || a > 10) return 5000;
	if(a < 7 || a > 8) return 3000;
	return 1400;
}
void config_buttons_switches() {
	SIM->SCGC5    |= (1 << 11 | 1 << 9);  //Enable to C, A
	PORTC->PCR[10] = (1 << 8); //make input pin GPIO switch 1
	PORTC->PCR[11] = (1 << 8); //make input pin GPIO switch 2
	PORTC->PCR[6] = (1 << 8); //make button input pin GPIO (SW2)
	PORTA->PCR[4] = (1 << 8);
	
	PTC->PDDR &= !(1 << 10); //enable switch 1 as input
	PTC->PDDR &= !(1 << 11); //enable swith 2 as input
	PTC->PDDR &= !(1 << 6); //enable as input button SW2
	PTC->PDDR &= !(1<<4);
	
	PORTC->PCR[6] |= (10 << 16); //enable risiing edge interrupt SW2
	NVIC_EnableIRQ(PORTC_IRQn);
	PORTA->PCR[4] |= (10 <<16);
	NVIC_EnableIRQ(PORTA_IRQn);
}
//Actually carries out the pitching simulation.
void do_action() {
		while(pitcher_pressed == 0){} //Wait for pitcher to press while he does his switches.
		sw1 = (PTC->PDIR & (1<<10)) >> 10;
		sw2 = (PTC->PDIR & (1<<11)) >> 11;
		pitcher_pressed = 0;
		//DO pitcher reaction game.
		ball_loc = 3;
		int count = 0;
		pressed_pitch_loc = -1;
		
		while(count < 8 && pressed_pitch_loc < 0) {
			while(ball_loc < 12 && pressed_pitch_loc < 0) {
				drawPixel(ball_loc, 7, get_color(WHITE));
				delay(10000);
				drawPixel(ball_loc, 7, get_color(CLEAR));
				ball_loc++;
			}
			while(ball_loc > 3 && pressed_pitch_loc < 0) {
				drawPixel(ball_loc, 7, get_color(WHITE));
				delay(10000);
				drawPixel(ball_loc, 7, get_color(CLEAR));
				ball_loc--;
			}
			count++;
		}
		
		drawPixel(ball_loc, 7, get_color(WHITE));
		delay(10000);
		fillRect(0,5,3,16,get_color(CLEAR));
		
		for(int i = 0; i < 3; i++) {
			drawPixel(7,5,get_color(WHITE));
			delay(10000);
			drawPixel(7,5,get_color(CLEAR));
			delay(10000);
		}
		
		//Would normally create a more interesting pitch based on pitcher_pressed_loc and 2 switches.
		//For now will just do a fastball.
		
		ball_row = 7;
		ball_col = 5;
		batted_col = -1;
		batted_row = -1;
		int meter_speed = determine_speed(pressed_pitch_loc);
		while(ball_col < 31 && batted_row < 0) {
			c = 2;
			drawPixel(ball_row, ball_col, get_color(WHITE));
			c = 0;
			delay(meter_speed);
			c = 3;
			drawPixel(ball_row, ball_col, ball_col == 29 || ball_col == 25 ? get_color(YELLOW) : get_color(CLEAR));
			c = 0;
			ball_col++;
		}
		if(batted_row > -1) {
			drawPixel(batted_row, batted_col, get_color(WHITE));
		}
		else {
			drawPixel(ball_row, ball_col, get_color(WHITE));
		}
		
		delay(10000);
		display_state = 0;
		produce_result(batted_row, batted_col, 0);
		updateGameState(game, current_result);
		draw_state(game);
}
//main game loop
int main (void) {
	create_colors();
	game = init_game_state();
	current_result = malloc(sizeof(result_t));
	if(!current_result) return -1;
	config_buttons_switches();
	//SUPER WEIRD bug. If this isn't directly in main, other lights start flashing...
	//Will just keep it here.
	config_pins_as_output();
	matrix_t mat;
	init_matrix(&mat);
	//Lines above this MUST BE IN MAIN//
	
	
	//make_random(game);
	draw_state(game);
	start_matrix_refresh();
	
	//start of loop.
	while(1) {
		while(display_state == 0); //Wait for batter to press.
		draw_action(game);
		do_action();
		display_state = 0;
		pitcher_pressed = 0;
		display_state = 0; //0: normal display. 1: pitching state.
		pitcher_pressed =0;
		ball_loc = 0;
		pressed_pitch_loc = -1;
		ball_row = -1;
		ball_col = -1;
		batted_col = -1;
		batted_row = -1;
		
	}
		
		
	while(1);
	
}
		


//Yellow bottom wire = PTC 10

//Push button 2 (Batter
int batter_int = 0;
void PORTC_IRQHandler(void) {
	PORTC->PCR[6] |= (1 << 24);
	batter_int++;
	if(display_state == 0) {
		display_state = 1;
	}
	if(display_state == 1) {
		batted_col = ball_col;
		batted_row = ball_row;
	}
	
	
}

int pitcher_int = 0;
//Push Button 3 (Pitcher)
void PORTA_IRQHandler(void) {
	PORTA->PCR[4] |= (1 << 24);
	pitcher_int++;
	pressed_pitch_loc = ball_loc;
	pitcher_pressed = 1;
}


	


