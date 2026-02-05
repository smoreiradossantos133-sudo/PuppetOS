/* PuppetOS Input Driver - Mouse & Keyboard Support */
#include <stdint.h>

typedef struct {
    int32_t x;
    int32_t y;
    uint8_t buttons;
} mouse_state_t;

typedef struct {
    uint8_t last_key;
    uint8_t key_down[256];
} keyboard_state_t;

static mouse_state_t mouse = {640, 360, 0};
static keyboard_state_t keyboard = {0, {0}};

void input_mouse_update(int32_t dx, int32_t dy, uint8_t buttons) {
    mouse.x += dx;
    mouse.y += dy;
    
    if (mouse.x < 0) mouse.x = 0;
    if (mouse.y < 0) mouse.y = 0;
    if (mouse.x > 1024) mouse.x = 1024;
    if (mouse.y > 768) mouse.y = 768;
    
    mouse.buttons = buttons;
}

void input_keyboard_press(uint8_t key) {
    keyboard.key_down[key] = 1;
    keyboard.last_key = key;
}

void input_keyboard_release(uint8_t key) {
    keyboard.key_down[key] = 0;
}

int32_t input_mouse_x(void) {
    return mouse.x;
}

int32_t input_mouse_y(void) {
    return mouse.y;
}

uint8_t input_mouse_buttons(void) {
    return mouse.buttons;
}
