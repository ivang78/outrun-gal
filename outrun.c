#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include "galaksija.h"
char my_x_pos, my_y_pos, my_speed, road_curve, road_curve_old, curve_x_delta;
char oth_position, oth_x_pos, oth_y_pos, oth_speed, oth_x_delta;
unsigned int oth_virtual_y_pos;
char collision, crash;
unsigned int road_pos;
unsigned int next_curve_pos, next_car_pos;
char c;
char sprite[48] = {
  0xc0, 0xd8, 0xcc, 0xcc, 0xcc, 0xcc, 0xe4, 0xc0, 0xc0, 0xd8, 0xc3, 0xcb, 0xc7, 0xc3, 0xe4, 0xc0,
  0xfa, 0xf3, 0xf7, 0xf3, 0xf3, 0xfb, 0xf3, 0xf5, 0xd6, 0xfb, 0xff, 0xcf, 0xcf, 0xff, 0xf7, 0xe9,
  0xcb, 0xfd, 0xcd, 0xcf, 0xcf, 0xce, 0xfe, 0xc7, 0xcb, 0xff, 0xcf, 0xcf, 0xcf, 0xcf, 0xff, 0xc7,	
  //0xc0, 0xd8, 0xcc, 0xcc, 0xcc, 0xcc, 0xe4, 0xc0, 0xe0, 0xc6, 0xc3, 0xcb, 0xc7, 0xc3, 0xc9, 0xd0,
  //0xfa, 0xf3, 0xf7, 0xf3, 0xf3, 0xfb, 0xf3, 0xf5, 0xff, 0xc3, 0xff, 0xcf, 0xcf, 0xff, 0xc3, 0xff,
  //0xcb, 0xfd, 0xcd, 0xcf, 0xcf, 0xce, 0xfe, 0xc7, 0xcb, 0xff, 0xcf, 0xcf, 0xcf, 0xcf, 0xff, 0xc7,
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
#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT 3
#define SPRITE_N_MY 0
#define SPRITE_N_OTHER 1

#define ROAD_Y_START 13
#define ROAD_Y_SIZE 10
#define ROAD_Y_CURVE 4
#define ROAD_HALF 13
#define ROAD_CENTER 15

#define MY_START_X_POS 12
#define MY_START_Y_POS 11

#define OTH_START_Y_POS 5

#define MAX_COLLISION 10

/*
	draw sprite
	xp: x position
	yp: y position
	n: number of sprite
	d: draw mode
*/
void draw_sprite(char xp, char yp, char n, char p) {
	int addr0 = SCREEN_ADDR + (yp << 5) + xp;
	for (char j = 0; j < SPRITE_HEIGHT; j++) {
		for (char i = 0; i < SPRITE_WIDTH; i++) {
			if (p == SPRITE_DRAW) {
				z80_bpoke(addr0 + (j << 5) + i, sprite[(j << 4)  + (n << 3) + i]);
			} else {
				z80_bpoke(addr0 + (j << 5) + i, 32);
			}
		}
	}
}

/*
	Welcome frame
*/
void frame() {
	char j;
	gal_gotoxy(10 ,3); gal_puts("OUTRUN GAME");
	gal_gotoxy(2,6);  gal_puts("ARROW LEFT, RIGHT - STEERING");
	gal_gotoxy(2,7);  gal_puts("ARROW UP - ACCELERATE");
	gal_gotoxy(2,8);  gal_puts("ARROW DOWN - BRAKE");
	gal_gotoxy(2,9);  gal_puts("DEL - QUIT");
	gal_gotoxy(2,10); gal_puts("PRESS ENTER");	
	gal_gotoxy(2,13); gal_puts("BY IVAN ILYICHEV 2024"); 
	do {
	} while (getk() != 10);
}

/*
	draw background
*/
void draw_bg() {
	for (char i = 0; i < 96; i++) {
		z80_bpoke(SCREEN_ADDR + 32 + i, bg[i]);
	}
	gal_gotoxy(1, 15); gal_puts("SPEED");
	gal_gotoxy(19, 15); gal_puts("SCORE 0");
	gal_gotoxy(14, 0); gal_puts("RUN!");
}

/*
	scroll background
*/
void scroll_bg() {
	char bb0;
	int base_addr = SCREEN_ADDR + 32;	
	for (int j = 0; j < 3; j++) {
		bb0 = z80_bpeek(base_addr + (j << 5));
		for (char i = 1; i < 32; i++) {
			z80_bpoke(base_addr + (j << 5) + i - 1, z80_bpeek(base_addr + (j << 5) + i));
		}
		z80_bpoke(base_addr + (j << 5) + 31, bb0);
	}
}

/*
	draw road
*/
void draw_road() {
	char yp0;
	char road_char, road_half_persp, rc, rcd, road_curv_persp;
	int addr0 = SCREEN_ADDR, addr1;
	char nstr[10];
	
	road_half_persp = ROAD_HALF;
	rc = 0;
	rcd = 0;
	if (road_curve != 0) {
		rcd = abs((road_curve << 6) / ROAD_Y_SIZE);
	}
	for (char j = 0; j < ROAD_Y_SIZE; j++) {		
		if (j%2 == road_pos%2) {
			road_char = 196; // 204;
		} else {
			road_char = 209; // 243;
		}
		addr1 = addr0 + ((ROAD_Y_START - j) << 5);
		road_curv_persp = rc >> 4;
		if (road_curve < 0) {
			road_curv_persp = -road_curv_persp;
		}
		// draw left ground 
		for (char i = 0; i < ROAD_CENTER - road_half_persp + road_curv_persp; i++) {
			z80_bpoke(addr1 + i, road_char);
		}
		// draw right ground 
		for (char i = ROAD_CENTER + 2 + road_half_persp + road_curv_persp; i < 32; i++) {
			z80_bpoke(addr1 + i, road_char);
		}
		// calculate other car position
		if (oth_position > 0 && ROAD_Y_START - j == oth_y_pos) {
			if (oth_position == 1) {
				oth_x_pos = ROAD_CENTER - road_half_persp + road_curv_persp + 1 + oth_x_delta;
			} else {
				oth_x_pos = ROAD_CENTER + road_half_persp + road_curv_persp - SPRITE_WIDTH + 1 - oth_x_delta;
			}
			draw_sprite(oth_x_pos, oth_y_pos, SPRITE_N_OTHER, SPRITE_DRAW);
		}
			
		road_half_persp--;
		rc = rc + rcd;
	}
	// draw score
	if (road_pos%8 == 0) {
		itoa(road_pos, nstr, 10);
		gal_gotoxy(25, 15); gal_puts(nstr);
	}
	road_pos = road_pos + my_speed;
}

/*
	draw speed
*/
void draw_speed() {
	gal_gotoxy(7, 15);
	gal_putc(214);
	if (my_speed == 0) {
		gal_putc(231); gal_putc(195);
	} else {
		gal_putc(227); gal_putc(199);
	}
	gal_putc(212);
}

/*
	calc road
 */
void calc_road() {
	char oth_y_p2, curve_dir, curve_next;
	int z;
	// calc next road condition
	if (road_pos >= next_curve_pos && oth_position == 0) {
		next_curve_pos = road_pos + (rand() / (RAND_MAX / 35)) + 35;		
		// dir may be -1, 0 or 1
		curve_dir = (rand() / (RAND_MAX / 3)) - 1;
		curve_next = road_curve + curve_dir;
		if (curve_next > -3 && curve_next < 3) { 
			road_curve = curve_next;
		}
		curve_x_delta = road_curve;
		if (curve_x_delta < -1) {
			curve_x_delta = -1;
		} else if (curve_x_delta > 1) {
			curve_x_delta = 1;
		} 
	} 
	// clear road if curve change
	if (road_curve != road_curve_old) {
		road_curve_old = road_curve;
		for (z = 0; z < 320; z++) {
			z80_bpoke(SCREEN_ADDR + 128 + z, 32);
		}
		draw_sprite(my_x_pos, my_y_pos, SPRITE_N_MY, SPRITE_DRAW);
	}
	// draw other car? 
	if (oth_position > 0) {
		if (my_speed == 0) {
			oth_speed = -1;
		} else {
			oth_speed = 1;
		}
		oth_virtual_y_pos = oth_virtual_y_pos + oth_speed;
		oth_y_p2 = oth_virtual_y_pos >> 2;
		if (oth_y_pos != oth_y_p2) {
			draw_sprite(oth_x_pos, oth_y_pos, SPRITE_N_OTHER, SPRITE_CLEAR);
			oth_y_pos = oth_y_p2;
		}
		if (oth_y_pos > MY_START_Y_POS || oth_y_pos < OTH_START_Y_POS) {
			oth_position = 0;
			oth_y_pos = 0;
		}
	}
	if (oth_position == 0 && road_pos >= next_car_pos) {
		next_car_pos = road_pos + (rand() / (RAND_MAX / 35)) + 35;
		oth_x_delta = rand() / (RAND_MAX / 3);
		if (rand() > (RAND_MAX / 2)) {
			oth_position = 1;
		} else {
			oth_position = 2;
		}
		oth_y_pos = OTH_START_Y_POS;
		oth_virtual_y_pos = oth_y_pos << 2;
		oth_speed = 1;
	}
}

/*
 * check collision
 */
void check_collision () {
	unsigned char is_crash = 0;	
	// crash with ground
	if (my_x_pos < ROAD_CENTER - ROAD_HALF + 1 || my_x_pos > ROAD_CENTER + ROAD_HALF + 1 - SPRITE_WIDTH)  {
		is_crash = 1;
	}
	// crash with other car
	if (oth_y_pos == my_y_pos - 2 && !(oth_x_pos + SPRITE_WIDTH - 1 < my_x_pos || oth_x_pos > my_x_pos + SPRITE_WIDTH - 1)) {
		is_crash = 1;
	}
	if (oth_y_pos > my_y_pos - 2 && oth_y_pos < my_y_pos + 3 && !(oth_x_pos + SPRITE_WIDTH < my_x_pos || oth_x_pos > my_x_pos + SPRITE_WIDTH)) {
		is_crash = 1;
	}
	if (is_crash == 1 && crash == 0) {
		gal_gotoxy(13, 0); gal_puts("CRASH!");
		draw_speed();
		my_speed = 0;
		crash = 1;
		collision--;
		if (collision == 0) {
			gal_gotoxy(11, 0); gal_puts("YOU LOOSE!");
			do {
				c = fgetc_cons();
			} while (c != 10 && c != 0);
			c = 255;
		}
	}
}

/*
	main function
*/
int main() {	
	rnd = z80_wpeek(RND_ADDR);
	srand(rnd);
	gal_cls();
	frame();
	do {
		gal_cls();
		draw_bg();
		
		my_x_pos = MY_START_X_POS;
		my_y_pos = MY_START_Y_POS;
		road_pos = 0;
		next_curve_pos = 60;
		next_car_pos = 40;
		road_curve = 0;
		road_curve_old = 0;
		curve_x_delta = 0;
		crash = 0;
		collision = MAX_COLLISION;
		my_speed = 0;
		oth_position = 0;
		oth_y_pos = 0;
		oth_virtual_y_pos = 0;
		oth_speed = 0;
		oth_x_delta = 0;
		
		draw_speed();
		draw_sprite(my_x_pos, my_y_pos, SPRITE_N_MY, SPRITE_DRAW);		
		do {
			calc_road();
			draw_road();
			if (road_curve != 0 && road_pos%4 == 0) {
				scroll_bg(road_curve);
			}
			c = getk();
			switch (c) {
				case 45: // left 
				case 46: // right
					crash = 0;
					draw_sprite(my_x_pos, my_y_pos, SPRITE_N_MY, SPRITE_CLEAR);
					if (c == 45) {
						my_x_pos--;
					} else {
						my_x_pos++;
					}
					draw_sprite(my_x_pos, my_y_pos, SPRITE_N_MY, SPRITE_DRAW);
					break;
				case 43: // up
					crash = 0;
					gal_gotoxy(13, 0); gal_puts("      ");
					my_speed = 1;
					draw_speed();
					break;
				case 44: // down
					crash = 0;
					my_speed = 0;
					draw_speed();
					break;
				case 67: // del
					cont = 0;
					c = 255;
					break;
				default:
					break;
			}
			if (road_curve != 0 && my_speed > 0 && road_pos%2 == 0 && c != 45 && c != 46) {
				draw_sprite(my_x_pos, my_y_pos, SPRITE_N_MY, SPRITE_CLEAR);
				my_x_pos = my_x_pos - curve_x_delta;
				draw_sprite(my_x_pos, my_y_pos, SPRITE_N_MY, SPRITE_DRAW);
			}
			check_collision();
		} while (c != 255);
	} while (cont == 1);
	return 0;
}
