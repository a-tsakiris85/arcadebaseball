#include "draw.h"


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
		// reset defaults for next pitching simulation
		display_state = 0;
		pitcher_pressed = 0;
		display_state = 0; //0: normal display. 1: pitching state.
		pitcher_pressed = 0;
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
