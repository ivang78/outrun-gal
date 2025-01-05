#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include "galaksija.h"
char x_pos, y_pos, road_half, road_center, road_curve, road_curve_old;
unsigned int road_pos;
char c;
char sprite[96] = {
  0xc0, 0xd8, 0xcc, 0xcc, 0xcc, 0xcc, 0xe4, 0xc0, 0xe0, 0xc6, 0xc3, 0xcb, 0xc7, 0xc3, 0xc9, 0xd0, 0xc0, 0xe0, 0xf0, 0xf0, 0xf0, 0xd0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  0xfa, 0xf3, 0xf7, 0xf3, 0xf3, 0xfb, 0xf3, 0xf5, 0xff, 0xc3, 0xff, 0xcf, 0xcf, 0xff, 0xc3, 0xff, 0xfc, 0xcd, 0xfc, 0xff, 0xfc, 0xce, 0xfc, 0xc0, 0xf8, 0xf3, 0xf3, 0xf3, 0xf3, 0xf4, 0xc0, 0xc0,
  0xcb, 0xfd, 0xcd, 0xcf, 0xcf, 0xce, 0xfe, 0xc7, 0xcb, 0xff, 0xcf, 0xcf, 0xcf, 0xcf, 0xff, 0xc7, 0xcb, 0xfe, 0xcd, 0xcc, 0xce, 0xfd, 0xc7, 0xc0, 0xed, 0xde, 0xcd, 0xce, 0xed, 0xde, 0xc0, 0xc0
};
char bg[96] = {
  0xc0, 0xc0, 0xe0, 0xf0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc8, 0xda, 0xff, 0xdc, 0xf0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xf0, 0xfc, 0xf4, 0xc0, 0xc0, 0xc0, 0xc0,
  0xe0, 0xe6, 0xe6, 0xee, 0xf7, 0xe4, 0xd0, 0xc0, 0xf8, 0xe6, 0xe6, 0xf6, 0xff, 0xff, 0xfd, 0xfd, 0xd0, 0xe0, 0xec, 0xef, 0xfd, 0xf0, 0xc0, 0xd8, 0xda, 0xd9, 0xff, 0xff, 0xfd, 0xf4, 0xd0, 0xc0,
  0xfd, 0xf9, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
unsigned int rnd;
char cont = 1;

#define SPRITE_DRAW 0
#define SPRITE_CLEAR 1
#define ROAD_Y_START 15
#define ROAD_Y_SIZE 10
#define ROAD_Y_CURVE 4

/*
	draw background
*/
void draw_bg () {
	for (char i = 0; i < 96; i++) {
		z80_bpoke(SCREEN_ADDR + 96 + i, bg[i]);
	}
}

void draw_road () {
	char yp0 = ROAD_Y_START;
	char road_char, road_half_persp, rc, rcd, road_curv_persp;
	int addr0 = SCREEN_ADDR, addr1;
	road_half_persp = road_half;
	rc = 0;
	rcd = 0;
	if (road_curve != 0) {
		rcd = abs((road_curve << 6) / ROAD_Y_SIZE);
	}
	for (char j = 0; j < ROAD_Y_SIZE; j++) {		
		if (j%2 == road_pos%2) {
			road_char = 132; // 204;
		} else {
			road_char = 145; // 243;
		}
		addr1 = addr0 + ((yp0 - j) << 5);
		road_curv_persp = rc >> 4;
		if (road_curve < 0) {
			road_curv_persp = -road_curv_persp;
		}
		for (char i = 0; i < road_center - road_half_persp + road_curv_persp; i++) {
			z80_bpoke(addr1 + i, road_char);
		}
		for (char i = road_center + 2 + road_half_persp + road_curv_persp; i < 32; i++) {
			z80_bpoke(addr1 + i, road_char);
		}
		road_half_persp--;
		rc = rc + rcd;
	}
	road_pos++;
}

/*
	draw sprite
	xp: x position
*/
void draw_sprite (char xp, char yp, char p) {
	int addr0 = SCREEN_ADDR + (yp << 5) + xp;
	for (char j = 0; j < 3; j++) {
		for (char i = 0; i < 8; i++) {
			if (p == SPRITE_DRAW) {
				z80_bpoke(addr0 + (j << 5) + i, sprite[(j << 5) + i]);
			} else {
				z80_bpoke(addr0 + (j << 5) + i, 32);
			}
		}
	}
}

/*
	calc road
 */
void calc_road () {
	int z;
	/*if (road_pos >= 30 && road_pos < 60) {
		road_curve = 1;
	} else if (road_pos >= 60 && road_pos < 90) {
		road_curve = 2;
	} else if (road_pos >= 90 && road_pos < 120) {
		road_curve = 1;
	} else if (road_pos >= 120 && road_pos < 150) {
		road_curve = 0;
	} else if (road_pos >= 150 && road_pos < 180) {
		road_curve = -1;
	} else if (road_pos >= 180 && road_pos < 210) {
		road_curve = -2;
	} else if (road_pos >= 210 && road_pos < 240) {
		road_curve = -1;
	} else if (road_pos >= 240) {
		road_curve = 0;
	}*/
	// clear road if curve change
	if (road_curve != road_curve_old) {
		road_curve_old = road_curve;
		for (z = 0; z <288; z++) {
			z80_bpoke(SCREEN_ADDR + 192 + z, 32);
		}
		draw_sprite(x_pos, y_pos, SPRITE_DRAW);
	}
}

/*
	main function
*/
int main() {	
	rnd = z80_wpeek(RND_ADDR);
	srand(rnd);
	gal_cls();
	do {
		gal_cls();
		draw_bg ();
		x_pos = 11;
		y_pos = 12;
		road_half = 15;
		road_center = 15;
		road_pos = 0;
		road_curve = 0;
		road_curve_old = 0;
		draw_sprite(x_pos, y_pos, 0);
		do {
			calc_road();
			draw_road();
			c = getk();
			switch (c) {
				case 45: // left 
					if (x_pos > 0) {
						draw_sprite(x_pos, y_pos, SPRITE_CLEAR);
						x_pos--;
					}
					draw_sprite(x_pos, y_pos, SPRITE_DRAW);
					break;
				case 46: // right
					if (x_pos < 32) {
						draw_sprite(x_pos, y_pos, SPRITE_CLEAR);
						x_pos++;
					}
					draw_sprite(x_pos, y_pos, SPRITE_DRAW);
					break;
				case 43: // up
					break;
				case 44: // down
					break;
				case 67: // del
					cont = 0;
					c = 255;
					break;
				default:
					break;
			}
			if (road_curve != 0 && road_pos%3 == 0) {
				draw_sprite(x_pos, y_pos, SPRITE_CLEAR);
				x_pos = x_pos - road_curve;
				draw_sprite(x_pos, y_pos, SPRITE_DRAW);
			}
		} while (c != 255);
	} while (cont == 1);
	return 0;
}
