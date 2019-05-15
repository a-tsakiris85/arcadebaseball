#include "color.h"
#include <stdint.h>
#include <stdlib.h>


/*
init_color(r,g,b) creates a new instance of a color structure with 
RGB values r, g, and b, respectively.
*/
color_t * init_color(uint8_t r, uint8_t g, uint8_t b) {
	color_t * c = malloc (sizeof(color_t));
	if(!c) {
		return c;
	}
	c->r = r;
	c->g = g;
	c->b = b;
	return c;
}

color_t * c_blue = NULL;
color_t * c_clear = NULL;
color_t * c_green = NULL;
color_t * c_red = NULL;
color_t * c_white = NULL;
color_t * c_yellow = NULL;
color_t * c_darkblue = NULL;

/*
create_colors() initializes all of the color values used on the LED matrix
and assigns them to the global variables c_blue to c_darkblue (declared above).
*/
void create_colors() {
c_blue = (init_color(0,0,1));
c_clear = (init_color(0,0,0));
c_green = (init_color(0,1,0));
c_red = (init_color(1,0,0));
c_white = (init_color(1,1,1));
c_yellow = (init_color(1,1,0));
c_darkblue = (init_color(1,1,1));
}

/*
get_color(c) returns the color of the specified color c.
*/

color_t * get_color(int c) {
	if(c == CLEAR) {
		return c_clear;
	}
	if(c == RED) {
		return c_red;
	}
	if(c == BLUE) {
		return c_blue;
	}
	if(c == GREEN) {
		return c_green;
	}
	if(c == WHITE) {
		return c_white;
	}
	if(c== YELLOW) {
		return c_yellow;
	}
	if(c==DARK_BLUE) {
		return c_darkblue;
	}
	
}
