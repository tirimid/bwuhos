#include "dev/fb.h"

#include <limine.h>

#include "kdef.h"
#include "kutil.h"

#define MAX_FB_CNT 16
#define FB_WEIGHT_WIDTH 1
#define FB_WEIGHT_HEIGHT 1

static struct limine_framebuffer_request volatile fb_req = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0,
};

static uint64_t gen_mask(uint8_t size);

static struct fb_info fbs[MAX_FB_CNT];
static size_t fb_cnt;

int
fb_init(void)
{
	ku_println(LT_INFO, "initializing framebuffer");
	
	fb_cnt = 0;
	size_t resp_fb_cnt = fb_req.response->framebuffer_count;
	
	for (size_t i = 0; i < resp_fb_cnt && fb_cnt < MAX_FB_CNT; ++i) {
		struct limine_framebuffer *lmn_fb = fb_req.response->framebuffers[i];
		struct fb_info *fb = &fbs[fb_cnt];
		*fb = (struct fb_info){
			.addr = lmn_fb->address,
			.width = lmn_fb->width,
			.height = lmn_fb->height,
			.mem_info = {
				.pitch = lmn_fb->pitch,
				.depth = lmn_fb->bpp,
				.shift_r = lmn_fb->red_mask_shift,
				.shift_g = lmn_fb->green_mask_shift,
				.shift_b = lmn_fb->blue_mask_shift,
				.mask_r = gen_mask(lmn_fb->red_mask_size),
				.mask_g = gen_mask(lmn_fb->green_mask_size),
				.mask_b = gen_mask(lmn_fb->blue_mask_size),
			},
		};
		fb->mem_info.mask_all |= fb->mem_info.mask_r << fb->mem_info.shift_r;
		fb->mem_info.mask_all |= fb->mem_info.mask_g << fb->mem_info.shift_g;
		fb->mem_info.mask_all |= fb->mem_info.mask_b << fb->mem_info.shift_b;
		++fb_cnt;
		
		if (lmn_fb->memory_model != LIMINE_FRAMEBUFFER_RGB
		    || lmn_fb->bpp > 64) {
			--fb_cnt;
		}
	}
	
	if (!fb_cnt) {
		ku_println(LT_ERR, "found no usable framebuffers!");
		return 1;
	}
	
	return 0;
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
	// TODO: implement get pixel.
	return 1;
}

int
fb_put_pixel(fb_id_t fb, uint64_t x, uint64_t y, uint8_t r, uint8_t g,
             uint8_t b)
{
	struct fb_info *info = &fbs[fb];
	if (x >= info->width || y >= info->height)
		return 1;
	
	size_t off = info->mem_info.pitch * y + info->mem_info.depth / 8 * x;
	uint64_t *px = (uint64_t *)(info->addr + off);
	
	*px &= ~info->mem_info.mask_all;
	*px |= (r & info->mem_info.mask_r) << info->mem_info.shift_r;
	*px |= (g & info->mem_info.mask_g) << info->mem_info.shift_g;
	*px |= (b & info->mem_info.mask_b) << info->mem_info.shift_b;
	
	return 0;
}

static uint64_t
gen_mask(uint8_t size)
{
	uint64_t mask = 0;
	for (uint8_t i = 0; i < size; ++i)
		mask |= 1 << i;
	return mask;
}
