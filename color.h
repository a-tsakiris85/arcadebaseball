#include <stdint.h>
#include <stdlib.h>

#define COLOR_SIZE 256

#define CLEAR 0
#define RED 1
#define BLUE 2
#define GREEN 3
#define WHITE 4
#define YELLOW 5
#define DARK_BLUE 6



/* 
 A structure used to describe a color in RGB. 
 RGB values are treated as binary values. If the value of a color component is 0,
 the color is treated as 0 in RGB. If the value is 1, the color is treated as 255 in RGB. 
*/
typedef struct rgb {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	
} color_t;

color_t * get_color(int c);
void create_colors(void);
