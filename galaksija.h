/* Header file with specific GALAKSIJA low-level functions */

#define z80_bpoke(a,b)  (*(unsigned char *)(a) = b)
#define z80_wpoke(a,b)  (*(unsigned int *)(a) = b)
#define z80_lpoke(a,b)  (*(unsigned long *)(a) = b)
#define z80_bpeek(a)    (*(unsigned char *)(a))
#define z80_wpeek(a)    (*(unsigned int *)(a))
#define z80_lpeek(a)    (*(unsigned long *)(a))

#define SCREEN_ADDR 0x2800
#define RND_ADDR 0x2AA7

unsigned char _scr_x, _scr_y;

// Low-level clear the screen and reset internal cursor position
void gal_cls() { 
	int z;
	for (z = 0; z <512; z++) {
		z80_bpoke(SCREEN_ADDR + z, 32);
	}
	_scr_x = 0;
	_scr_y = 0;
}

// Set internal position of cursor for low-level putc and call gotoxy
void gal_gotoxy(char x, char y) {
	_scr_x = x;
	_scr_y = y;
}

// Low-level write character to internal cursor position
void gal_putc(char ch) {
	z80_bpoke(SCREEN_ADDR + (_scr_y << 5) + _scr_x, ch);
	_scr_x++;
	if (_scr_x > 32) {
		_scr_x = 0;
		_scr_y++;
	}
}

// Low-level write string to internal cursor position
int gal_puts (char *str) {
	char ch;
	int len = 0;
	while ((ch = *str) != 0x0) {
		z80_bpoke(SCREEN_ADDR + (_scr_y << 5) + _scr_x, ch);
		str++;
		len++;
		_scr_x++;
		if (_scr_x > 32) {
			_scr_x = 0;
			_scr_y++;
		}
	}
	return len;
}
