#include "color.h"
#include <stdint.h>
#include <stdlib.h>

int colors = 0;
color_t * init_color(uint8_t r, uint8_t g, uint8_t b) {
	colors = 6;
	color_t * c = malloc (sizeof(color_t));
	if(!c) {
		colors = 8;
		return c;
	}
	c->r = r;
	c->g = g;
	c->b = b;
	colors = 0;
	return c;
}

color_t * c_blue = NULL;
color_t * c_clear = NULL;
color_t * c_green = NULL;
color_t * c_red = NULL;
color_t * c_white = NULL;
color_t * c_yellow = NULL;
color_t * c_darkblue = NULL;

void create_colors() {
c_blue = (init_color(0,0,255));
c_clear = (init_color(0,0,0));
c_green = (init_color(0,255,0));
c_red = (init_color(255,0,0));
c_white = (init_color(255,255,255));
c_yellow = (init_color(255,255,0));
c_darkblue = (init_color(25,25,112));
}
color_t * get_color(int c) {
	colors = 5;
	if(c == CLEAR) {
		colors = 0;
		return c_clear;
	}
	if(c == RED) {
		colors = 0;
		return c_red;
	}
	if(c == BLUE) {
		colors = 0;
		return c_blue;
	}
	if(c == GREEN) {
		colors = 0;
		return c_green;
	}
	if(c == WHITE) {
		colors = 0;
		return c_white;
	}
	if(c== YELLOW) {
		colors = 0;
		return c_yellow;
	}
	if(c==DARK_BLUE) {
		colors = 0;
		return c_darkblue;
	}
	
}
