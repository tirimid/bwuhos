#include "dev/fb.h"
#include "kutil.h"

void
_start(void)
{
	fb_init();
	fb_id_t fb = fb_get_id(fb_get_best());
	fb_put_pixel(fb, 100, 100, 255, 255, 255);
	ku_hang();
}
