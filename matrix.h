
#include <stdint.h>
#include <stdlib.h>
#include "pins.h"
#include "color.h"


#define NUM_ROWS 16
#define NUM_COLS 32
#define RESOLUTION 16
#define THRESHOLD_RESET (256 / RESOLUTION * (NUM_ROWS/2))


typedef struct Matrix  {
	color_t buffer[NUM_ROWS][NUM_COLS];
	uint8_t currentRowPair; //0 -> 0,8, 1-> 1,9 etc. Which rows to draw on.
	uint8_t cycle_index; //this is used for doing all the cool color stuff.
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