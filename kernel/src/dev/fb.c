#include "dev/fb.h"

#include <limine.h>

#include "kdef.h"
#include "kutil.h"

#define FB_WEIGHT_WIDTH 1
#define FB_WEIGHT_HEIGHT 1

static struct limine_framebuffer_request volatile fb_req = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0,
};

static struct fb_info fbs[16];
static size_t fb_cnt;

void
fb_init(void)
{
	fb_cnt = 0;
	size_t resp_fb_cnt = fb_req.response->framebuffer_count;
	
	for (size_t i = 0; i < resp_fb_cnt; ++i) {
		struct limine_framebuffer *lmn_fb = fb_req.response->framebuffers[i];
		fbs[fb_cnt++] = (struct fb_info){
			.addr = lmn_fb->address,
			.width = lmn_fb->width,
			.height = lmn_fb->height,
			.mem_info = {
				.pitch = lmn_fb->pitch,
				.depth = lmn_fb->bpp,
				.shift_r = lmn_fb->red_mask_shift,
				.shift_g = lmn_fb->green_mask_shift,
				.shift_b = lmn_fb->blue_mask_shift,
				.size_r = lmn_fb->red_mask_size,
				.size_g = lmn_fb->green_mask_size,
				.size_b = lmn_fb->blue_mask_size,
			},
		};
		
		if (lmn_fb->memory_model != LIMINE_FRAMEBUFFER_RGB
		    || lmn_fb->bpp > 64) {
			--fb_cnt;
		}
	}
	
	if (!fb_cnt)
		ku_log_err("found no usable framebuffers!");
}

struct fb_info const *
fb_get_best(void)
{
	size_t best_ind = 0;
	uint64_t best_score = 0;
	
	for (size_t i = 0; i < fb_cnt; ++i) {
		uint64_t score = 0;
		
		score += fbs[i].width * FB_WEIGHT_WIDTH;
		score += fbs[i].height * FB_WEIGHT_HEIGHT;
		
		if (score > best_score) {
			best_ind = i;
			best_score = score;
		}
	}
	
	return &fbs[best_ind];
}

struct fb_info const *
fb_get_all(size_t *out_cnt)
{
	if (out_cnt)
		*out_cnt = fb_cnt;
	
	return fbs;
}

fb_id_t
fb_get_id(struct fb_info const *info)
{
	return ((uintptr_t)info - (uintptr_t)fbs) / sizeof(struct fb_info);
}

int
fb_get_pixel(uint8_t *out_r, uint8_t *out_g, uint8_t *out_b, fb_id_t fb,
             uint64_t x, uint64_t y)
{
	return GRC_OK;
}

int
fb_put_pixel(fb_id_t fb, uint64_t x, uint64_t y, uint8_t r, uint8_t g,
             uint8_t b)
{
	return GRC_OK;
}
