#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdint.h>

void input_mouse_update(int32_t dx, int32_t dy, uint8_t buttons);
void input_keyboard_press(uint8_t key);
void input_keyboard_release(uint8_t key);

int32_t input_mouse_x(void);
int32_t input_mouse_y(void);
uint8_t input_mouse_buttons(void);

#endif
