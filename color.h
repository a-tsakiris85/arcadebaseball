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

typedef struct RGB {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	
} color_t;

color_t * get_color(int c);
void create_colors(void);