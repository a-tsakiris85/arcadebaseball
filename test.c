/* 
This file includes all of the test cases we wrote to determine that the LED matrix was working properly and that 
our game logic was implemented correctly. Each of these test cases was added to the test.h file and called in
main (in main.c) to test all of our game features independently. 
*/
#include "draw.h"

extern result_t * current_result;
extern game_t * game; 
extern int sw2;
extern int sw1;


// tests that a pixel is correctly drawn at (0,0) on the matrix display
void pixelTest(void) {
	drawPixel(0,0,get_color(RED));
	while(1);
}

// tests that a rectangle of width 10 and height 5 is drawn correctly on the matrix display
void drawRectTest(void) {
	fillRect(0,0,10,5,get_color(WHITE));
	while(1);
}

/* tests that a rectangle of width 10 and height 2 is drawn correctly on the matrix display
   and is correctly animated as it travels across the screen*/
void animateRectTest(void) {
	int row = 0; int col = 0;
	int width = 10; int height = 2;
	int delta = 1;
	while(1) {
		fillRect(row,col,width,height,get_color(BLUE));
		delay(1200);
		fillRect(row,col,width,height,get_color(CLEAR));
		if(row > 14) delta = -delta;
		row += delta; col += delta;
		fillRect(row,col,width,height,get_color(BLUE));
	}
}

/* tests that all of the LED colors are displayed properly */
void colorsTest(void) {
	drawPixel(0,0,get_color(WHITE));
	drawPixel(1,1,get_color(RED));
	drawPixel(2,2,get_color(BLUE));
	drawPixel(3,3,get_color(GREEN));
	drawPixel(4,4,get_color(YELLOW));
	while(1);
}

// tests that a random state gets drawn correctly on the matrix display
void stateTest(void) {
	game_t* testGame;
	testGame->balls = 2;
	testGame->batter = 1;
	testGame->first = 1;
	testGame->third = 1;
	testGame->inning = 4;
	testGame->score1 = 5;
	testGame->score2 = 8;
	testGame->second = 0;
	testGame->outs = 2;
	testGame->strikes = 1;
	draw_state(testGame);
	while(1);
}

 
//tests that the drawNum function correctly draws numbers 1-9
void drawNumber(void) {
	int row = 7; int col = 15;
	int num = 0;
	while(num < 10) {
		draw_num(num,row,col,get_color(WHITE));
		fillRect(0,0,32,16,get_color(CLEAR));
		delay(8000);
		num++;
	}
}
	

/* 
tests that all of the possible results from an at-bat are displayed 
on the LED matrix correctly
*/
void drawResult(void) {
	// draw ball
	result_t * result;
	result->ball = 1;
	result->hit = 0; result->out = 0; result->strike = 0;
	draw_result(result);
	fillRect(0,0,32,16,get_color(CLEAR));
	
	// draw strike
	result->strike = 1;
	result->hit = 0; result->out = 0; result->ball = 0;
	draw_result(result);
	fillRect(0,0,32,16,get_color(CLEAR));
	
	// draw foul
	result->strike = 2;
	result->hit = 0; result->out = 0; result->ball = 0;
	draw_result(result);
	fillRect(0,0,32,16,get_color(CLEAR));
	
	
	// draw out
	result->out = 1;
	result->hit = 0; result->strike = 0; result->ball = 0;
	draw_result(result);
	fillRect(0,0,32,16,get_color(CLEAR));
	
	// draw single
	result->hit = 1;
	result->out = 0; result->strike = 0; result->ball = 0;
	draw_result(result);
	fillRect(0,0,32,16,get_color(CLEAR));
	
	// draw double
	result->hit = 2;
	result->out = 0; result->strike = 0; result->ball = 0;
	draw_result(result);
	fillRect(0,0,32,16,get_color(CLEAR));
	
	// draw triple
	result->hit = 3;
	result->out = 0; result->strike = 0; result->ball = 0;
	draw_result(result);
	fillRect(0,0,32,16,get_color(CLEAR));
	
	// draw home run
	result->hit = 4;
	result->out = 0; result->strike = 0; result->ball = 0;
	draw_result(result);
	fillRect(0,0,32,16,get_color(CLEAR));
}

/*
reset_defaults(result) resets all of the fields of `result` to 0.
*/
void reset_defaults(result_t * result) {
	result->ball = 0;
	result->hit = 0;
	result->out = 0;
	result->strike = 0;
}

/*
reset_game_defaults(game) resets all of the fields of `game` to 0.
*/
void reset_game_defaults(game_t * game) {
	game->balls = 0;
	game->batter = 1;
	game->first = 0;
}
	


// tests that the mound, batter's box, and various other displays
// are drawn correctly, and that the white LED travels correctly across the screen.
void testDrawAction(void) {
	game_t * init;
	init->first = 1;
	init->third = 3;
	draw_action(init);
	sw2 = 1;
	delay(5000);
}

void updateGame(void) {
	// test walk
	result_t * result = init_result();
	result->ball = 1;
	game_t * game = init_game_state();
	game->balls = 3;
	updateGameState(game,result);
	draw_state(game);
	delay(5000);
	
	// test runners
	reset_defaults(result);
	game->first = 1;
	game->second = 1;
	game->third = 1;
	result->hit = 1;
	updateGameState(game,result);
	draw_state(game);
	delay(5000);
	
	// test strike -> out
	reset_defaults(result);
	game->strikes = 2;
	result->strike = 1;
	updateGameState(game,result);
	draw_state(game);
	delay(5000);

	// test end of inning, players switch roles
	reset_defaults(result);
	game->strikes = 2;
	game->outs = 2;
	result->strike = 1;
	updateGameState(game,result);
	draw_state(game);
	delay(5000);
	
	// test update score correctly
	reset_defaults(result);
	result->hit = 4;
	updateGameState(game,result);
	draw_state(game);
	delay(5000);
	
	// test foul ball
	reset_defaults(result);
	result->strike = 2;
	updateGameState(game,result);
	draw_state(game);
	delay(5000);
	
}	
	




