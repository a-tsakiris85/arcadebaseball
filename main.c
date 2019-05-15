#include "matrix.h"
#include "game.h"
#include "time.h"

#define pitch_size 100


int sw1= 8; //dummy value. Will later be used for pitch selection.
int sw2 = 5; //dummy value. Later used for pitch selection.

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


int pitch[100][2];

result_t * current_result;
game_t * game; // used to keep track of the current game state


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




void delay(int x) {
	disable_interrupt();
	for(int i = 0; i < x/100; i++) {
		for(int j = 0; j < 100; j++);
		updateDisplay();
	}
	enable_interrupt();
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
random(high) returns a random number between 1 and high, inclusive
*/
int random(int high) {
	return (rand() % high) + 1;
}

void draw_result(void) {
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
	display_state=0;

}

/*
produce_result(row,col,wasBall) updates the current result probabilistically
based on the position at which the batter swung (denoted by (row,col)) and whether
or not the pitch thrown was a ball.
*/
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
		else if(a == 1) { //edge
			if(r <30) current_result->strike = 2; // 30% chance of foul ball
			else if (r < 70) current_result->out = 1; // 40% chance of an out
			else if (r < 87) current_result->hit = 1; // 17% chance of a single
			else if (r < 95) current_result->hit = 2; // 8% chance of a double
			else if (r < 99) current_result->hit = 3; // 4% chance of a triple
			else current_result->hit = 1;
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
	draw_result();
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

/*
config() sets up all of the necessary configurations to allow user input.
It also initializes the random module so that we may use the random number generator.
*/
void config() {
	SIM->SCGC5    |= (1 << 11 | 1 << 9); //Enable clock to ports C and A
	PORTC->PCR[10] = (1 << 8); //make input pin GPIO switch 1
	PORTC->PCR[11] = (1 << 8); //make input pin GPIO switch 2
	PORTC->PCR[6] = (1 << 8); //make on-board button GPIO (SW2)
	PORTA->PCR[4] = (1 << 8); //make on-board button GPIO (SW3)
	
	PTC->PDDR &= !(1 << 10); //enable switch 1 as input
	PTC->PDDR &= !(1 << 11); //enable swith 2 as input
	PTC->PDDR &= !(1 << 6); //enable button SW2 as input
	PTC->PDDR &= !(1 << 4); // enable button SW3 as input
	
	PORTC->PCR[6] |= (10 << 16); //enable rising edge interrupt for SW2
	NVIC_EnableIRQ(PORTC_IRQn); // enable interrupt handler for SW2
	PORTA->PCR[4] |= (10 << 16); // enable rising edge interrupt for SW3
	NVIC_EnableIRQ(PORTA_IRQn); // enable interrupt handler for SW2

	/* enable clock to RNGA module */
	SIM->SCGC6 |= SIM_SCGC6_RNGA_MASK;
	
  /* turn on RNGA module */
  SIM->SCGC3 |= SIM_SCGC3_RNGA_MASK;

  /* set SLP bit to 0 - "RNGA is not in sleep mode" */
  RNG->CR &= ~RNG_CR_SLP_MASK; 

  /* set HA bit to 1 - "security violations masked" */
  RNG->CR |= RNG_CR_HA_MASK;
 
  /* set GO bit to 1 - "output register loaded with data" */
  RNG->CR |= RNG_CR_GO_MASK;
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
			for(int i = 20; i < 50; i++) {
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

//Actually carries out the pitching simulation.
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
		while(count < 8 && pressed_pitch_loc < 0) {
			// while the ball is still traveling left
			while(ball_loc < 12 && pressed_pitch_loc < 0) {
				drawPixel(ball_loc, 7, get_color(WHITE));
				delay(10000);
				drawPixel(ball_loc, 7, get_color(CLEAR));
				ball_loc++;
			}
			// while the ball is traveling right
			while(ball_loc > 3 && pressed_pitch_loc < 0) {
				drawPixel(ball_loc, 7, get_color(WHITE));
				delay(10000);
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
		ball_row = 7;
		ball_col = 5;
		batted_col = -1; // TODO: use for balls
		batted_row = -1;
		
		// animate the ball travelling towards the batter's box
		int wasBall = 1;
		int i = 0;
		while(ball_col < 31 && batted_row < 0) {
			drawPixel(ball_row, ball_col, get_color(WHITE));
			delay(meter_speed);
			drawPixel(ball_row, ball_col, (ball_col == 29 || ball_col == 25)&&
					inBox(ball_row,ball_col)>0 ? get_color(YELLOW) : get_color(CLEAR));
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
		produce_result(batted_row, batted_col, wasBall);
		draw_result();
		updateGameState(game, current_result);
		draw_state(game);
}

//main game loop
int main (void) {
	create_colors();
	game = init_game_state();
	current_result = malloc(sizeof(result_t));
	if(!current_result) return -1;
	config();
	//SUPER WEIRD bug. If this isn't directly in main, other lights start flashing...
	//Will just keep it here.
	config_pins_as_output();
	matrix_t mat;
	init_matrix(&mat);
	//Lines above this MUST BE IN MAIN//
	
	draw_state(game);
	start_matrix_refresh();
	
	//start of loop.
	while(1) {
		while(display_state == 0); // Wait for batter to press SW2 to indicate the beginning of their turn
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
		
//Push Button 2 (Batter)
void PORTC_IRQHandler(void) {
	PORTC->PCR[6] |= (1 << 24); // clear the interrupt status flag
	if(display_state == 0) {
		display_state = 1;
	}
	if(display_state == 1) {
		batted_col = ball_col;
		batted_row = ball_row;
	}
}

//Push Button 3 (Pitcher)
void PORTA_IRQHandler(void) {
	PORTA->PCR[4] |= (1 << 24); // clear the interrupt status flag
	pressed_pitch_loc = ball_loc;
	pitcher_pressed = 1;
}


	


