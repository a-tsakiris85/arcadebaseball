
#include <stdint.h>
#include <stdlib.h>
#include "matrix.h"


int a = 0;
int m = 0 ;
matrix_t * matrix;

int que = 0;
void drawPixel(uint8_t row, uint8_t col, const color_t * color) {
	que = 8;
	if(row < 0 || row > 15 || col < 0 || col > 31){
		que = 0;
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
	que = 0;
	
	
}

void fillRect(uint8_t row, uint8_t col, uint8_t width, uint8_t height, const color_t * color) {
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			drawPixel(row + r, col + c, color);
		}
	}
}

//I'm not sure if I need to malloc, etc.
void init_matrix(matrix_t * m) {
	matrix = m;
	matrix->currentRowPair = 0;
	matrix->cycle_index = 0;
	fillRect(0,0,NUM_COLS, NUM_ROWS, get_color(CLEAR));
}

void updateDisplay() {
	digital_write(LAT, 0);
	
	//Which row group we are working with.
	uint8_t row_select = matrix->currentRowPair;
	//need to make ABC represent this in binary.
	digital_write(A, row_select & 0x01);
	digital_write(B, (row_select >> 1) & 0x01);
	digital_write(C, (row_select >> 2) & 0x01);
	digital_write(OE, 0);
	
	uint8_t color_threshold = matrix->cycle_index / (NUM_ROWS / 2);
	//send in the 192 bits by iterating over the columns.
	for(int col = 0; col < NUM_COLS; col++) {
	
		digital_write(R1, 0);
		digital_write(R2, 0);
		digital_write(G1, 0);
		digital_write(G2, 0);
		digital_write(B1, 0);
		digital_write(B2, 0);
		
		color_t top_color = matrix->buffer[row_select][col];
		color_t bot_color = matrix->buffer[row_select + (NUM_ROWS /2)][col];
		
		digital_write(R1, top_color.r / (RESOLUTION - 1) > color_threshold);
		digital_write(G1, top_color.g / (RESOLUTION - 1) > color_threshold);
		digital_write(B1, top_color.b / (RESOLUTION - 1) > color_threshold);
		
		digital_write(R2, bot_color.r / (RESOLUTION - 1) > color_threshold);
		digital_write(G2, bot_color.g / (RESOLUTION - 1) > color_threshold);
		digital_write(B2, bot_color.b / (RESOLUTION - 1) > color_threshold);
		
		digital_write(CLK, 1);
		digital_write(CLK, 0);
		
	}
	
	
	digital_write(OE, 1);
	digital_write(LAT, 1);
	
	matrix->currentRowPair = (matrix->currentRowPair + 1) % (NUM_ROWS / 2);
	matrix->cycle_index = (matrix->cycle_index + 1) % THRESHOLD_RESET;

}

void setup_matrix() {
	config_pins_as_output();
	matrix_t mat;
	init_matrix(&mat);
}



void start_matrix_refresh() {
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; 
	PIT->MCR &= ~(1<<1);
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK / 700; //port c interrupts don't work if faster.
	NVIC_EnableIRQ(PIT0_IRQn);
	
	NVIC_SetPriority(PIT0_IRQn, 1); 
	NVIC_SetPriority(PORTC_IRQn, 0);
	
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK; // clear timer interrupt flag
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK; // enable timer interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; // enable timer
}
void PIT0_IRQHandler(void) {
	PIT->CHANNEL[0].TFLG = 1;
	
	
	//Need PIT code to disable interrupts
	//PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK; // clear timer interrupt flag
	//PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK; // disable timer interrupts
	updateDisplay();
	//Refresh and restart timer
	//PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;  // re-enable timer interrupts
}

void disable_interrupt(void) {
	PIT->CHANNEL[0].TCTRL = 1;
}
void enable_interrupt(void) {
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK / 750; //port c interrupts don't work if faster.
	NVIC_EnableIRQ(PIT0_IRQn);
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK; // clear timer interrupt flag
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK; // enable timer interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; // enable timer
}

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

