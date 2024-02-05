#ifndef INT_PIC_H__
#define INT_PIC_H__

#include <stdint.h>

void pic_init(void);
void pic_get_mask(uint8_t *out_mask_1, uint8_t *out_mask_2);
void pic_set_mask(uint8_t mask_1, uint8_t mask_2);
void pic_eoi(uint8_t irq);

#endif
