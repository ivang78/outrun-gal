all:
	zcc +gal -create-app  -pragma-redirect:fputc_cons=fputc_cons_generic -o outrun outrun.c
