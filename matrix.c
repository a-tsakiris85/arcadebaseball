
#include <stdint.h>
#include <stdlib.h>
#include "matrix.h"


/* matrix is a global variable corresponding to the current state of
the LED matrix. */
matrix_t * matrix;

/*
drawPixel(row,col,color) sets the global matrix object's color to [color]
at the position (row,col) in the matrix's buffer attribute. This buffer 
attribute is then used to actually update the matrix display with the colors
at each LED position.
*/
void drawPixel(uint8_t row, uint8_t col, const color_t * color) {
	if(row > 15 || col > 31){
		return;
	}
	if(row == 0) {
		matrix->buffer[7][col] =  * color;
	}
	else if(row == 8) {
		matrix->buffer[15][col] = * color;
	}
	else {
		matrix->buffer[row - 1][col] = * color;
	}
}

/*
fillRect(row,col,width,height,color) starts at position (row,col) on the LED matrix, where
(0,0) is the top right pixel on the LED board, and draws a rectangle of color [color] with
height [height] and width [width]. 

- Precondition: [row]: Possible row values range from 0-15 and correspond to rows in the 
  16-LED direction of the matrix display. 
- Precondition: [col]: Possible col values range from 0-31 and correspond to columns in 
  the 32-LED direction.
- Precondition: [width]: Width corresponds to the number of pixels to draw in the
  long (32-LED) direction. This inequality must hold: 0 < col+width < 31
- Precondition: [height]: Height corresponds to the number of pixels to draw in the short
  (16-LED) direction. This inequality must hold: 0 < row+height < 15
- Precondition: [color]: must be a valid and properly initialized instance of color_t
*/
void fillRect(uint8_t row, uint8_t col, uint8_t width, uint8_t height, const color_t * color) {
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			drawPixel(row + r, col + c, color);
		}
	}
}

/*
init_matrix(m) initializes the global variable `matrix` with the matrix
object `m` and its fields with default values
*/
void init_matrix(matrix_t * m) {
	matrix = m;
	matrix->currentRowPair = 0;
	fillRect(0,0,NUM_COLS, NUM_ROWS, get_color(CLEAR));
}

/*
updateDisplay() writes to the output pins on the LED matrix display 
to draw a pair of rows on the display. It draws the row pair corresponding
to the current row pair stored in the global variable `matrix`.
*/
void updateDisplay() {
	// Used to signal the beginning of a row of data
	digital_write(LAT, 0);
	
	// row_select 
	uint8_t row_select = matrix->currentRowPair;
	
	// Write to the row select pins on the matrix display
	digital_write(A, row_select & 0x01);
	digital_write(B, (row_select >> 1) & 0x01);
	digital_write(C, (row_select >> 2) & 0x01);
	// Disable output on the LED board until 
	digital_write(OE, 0);
	// Now that the address is set, clock out 192 bits of data by iterating over the columns.
	for(int col = 0; col < NUM_COLS; col++) {
		digital_write(R1, 0);
		digital_write(R2, 0);
		digital_write(G1, 0);
		digital_write(G2, 0);
		digital_write(B1, 0);
		digital_write(B2, 0);
		
		color_t top_color = matrix->buffer[row_select][col];
		color_t bot_color = matrix->buffer[row_select + (NUM_ROWS /2)][col];
		
		digital_write(R1, top_color.r>0 ? 1: 0);
		digital_write(G1, top_color.g>0 ? 1: 0);
		digital_write(B1, top_color.b>0 ? 1: 0);
		
		digital_write(R2, bot_color.r>0 ? 1: 0);
		digital_write(G2, bot_color.g>0 ? 1: 0);
		digital_write(B2, bot_color.b>0 ? 1: 0);
		
		digital_write(CLK, 1);
		digital_write(CLK, 0);
		
	}
	
	// Now that we have iterated over all of the columns, latch the result
	// and write it to the matrix display
	digital_write(OE, 1);
	digital_write(LAT, 1);
	
	// Can only write to one row pair at a time, so setup the next row pair
	// that will be written to the matrix display when updateDisplay() is called again.
	matrix->currentRowPair = (matrix->currentRowPair + 1) % (NUM_ROWS / 2);
}


/*
start_matrix_refresh() is used to configure the PIT timer so that interrupts may
be triggered 700 times per second in order to constantly redraw the current matrix 
display (which is necessary in order to make it appear that the LEDs remain continuously on)
*/
void start_matrix_refresh() {
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; // enable clock to PIT module
	PIT->MCR &= ~(1<<1); // enable clock for standard PIT timers
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK / 700; // trigger interrupts ~700 times per second
	NVIC_EnableIRQ(PIT0_IRQn); // enable interrupts from PIT0 timer.
	
	NVIC_SetPriority(PIT0_IRQn, 1); 
	NVIC_SetPriority(PORTC_IRQn, 0);
	
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK; // clear timer interrupt flag
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK; // enable timer interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; // enable timer
}

/*
PIT0 interrupt handler. Clears the interrupt flag and updates the LED Matrix display.
*/
void PIT0_IRQHandler(void) {
	PIT->CHANNEL[0].TFLG = 1;
	updateDisplay();
}

void disable_interrupt(void) {
	PIT->CHANNEL[0].TCTRL = 1;
}
void enable_interrupt(void) {
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK / 1000;
	NVIC_EnableIRQ(PIT0_IRQn);
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK; // clear timer interrupt flag
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK; // enable timer interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; // enable timer
}

/*
draw_num(n,row,col,color) draws the number `n` at position (row,col) with
color `color`. If `n`>9, only the ones digit is drawn.
*/
void draw_num(int n, uint8_t row, uint8_t col, color_t * color) {
	n = n % 10;
	if (n != 1 && n !=4) fillRect(row,col,1,4,color);
	if (n!=5 && n!=6) fillRect(row,col,4,1,color);
	if (n!=2) fillRect(row,col+3,4,1,color);
	if (n!=1 && n!=4 && n!=7 && n!=9) fillRect(row,col+6,1,4,color);
	if (n==0 || n==2 || n==6 || n==8) fillRect(row+3,col+3,4,1,color);
	if (n==0 || n==4 || n==5 || n==6 || n==8 || n==9) fillRect(row+3,col,4,1,color);
	if(n!= 0 && n!=1 && n!= 7) fillRect(row,col+3,1,4,color);
}

