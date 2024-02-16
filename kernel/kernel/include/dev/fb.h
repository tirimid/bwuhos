#ifndef DEV_FB_H
#define DEV_FB_H

#include <stdint.h>
#include <stddef.h>

typedef size_t fb_id_t;

struct fb_info {
	uint8_t *addr;
	uint64_t width, height;
	
	struct {
		uint64_t pitch;
		uint16_t depth;
		uint64_t mask_r, mask_g, mask_b, mask_all;
		uint8_t shift_r, shift_g, shift_b;
	} mem_info;
};

int fb_init(void);
struct fb_info const *fb_get_best(void);
struct fb_info const *fb_get_all(size_t *out_cnt);
fb_id_t fb_get_id(struct fb_info const *info);
int fb_get_pixel(uint8_t *out_r, uint8_t *out_g, uint8_t *out_b, fb_id_t fb, uint64_t x, uint64_t y);
int fb_put_pixel(fb_id_t fb, uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b);

#endif
