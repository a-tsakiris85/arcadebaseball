
#include <stdint.h>
#include <stdlib.h>
#include "pins.h"
#include "color.h"


#define NUM_ROWS 16
#define NUM_COLS 32

/*
The structure representing the LED matrix.

	- buffer[16][32] is a 2D array of color objects. The color
	  object at a specify coordinate denotes which color to display at 
		the corresponding LED on the matrix display.
	- currentRowPair is an integer denoting which two rows are currently
	  being drawn. According to Adafruit's documentation, the matrix display
		is divided into 8 interleaved sections / strips. The zeroth section is
		the 0th and 8th rows of LEDs, the first section is the 1st 9th rows of LEDs, etc.
	- 
*/
typedef struct Matrix  {
	color_t buffer[NUM_ROWS][NUM_COLS];
	uint8_t currentRowPair; //0 -> 0,8, 1-> 1,9 etc. Which rows to draw on.
} matrix_t;

void setup_matrix(void);
void updateDisplay(void);
void fillRect(uint8_t row, uint8_t col, uint8_t width, uint8_t height, const color_t * color);
void drawPixel(uint8_t row, uint8_t col, const color_t * color);
void init_matrix(matrix_t * m);
void start_matrix_refresh(void);
void draw_num(int n, uint8_t row, uint8_t col, color_t * color);
void enable_interrupt(void); //not used
void disable_interrupt(void); //not used