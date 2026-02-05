/*
 * Input System
 * Handles keyboard, mouse, and other input devices
 */

#ifndef INPUT_H
#define INPUT_H

#include <kernel/kernel.h>

/* ===== KEY CODES ===== */
typedef enum {
    KEY_ESCAPE = 0x01,
    KEY_1 = 0x02, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
    KEY_MINUS, KEY_EQUAL,
    KEY_BACKSPACE = 0x0E,
    KEY_TAB = 0x0F,
    KEY_Q = 0x10, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
    KEY_LBRACKET, KEY_RBRACKET,
    KEY_ENTER = 0x1C,
    KEY_LCTRL = 0x1D,
    KEY_A = 0x1E, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L,
    KEY_SEMICOLON, KEY_QUOTE, KEY_BACKTICK,
    KEY_LSHIFT = 0x2A,
    KEY_BACKSLASH = 0x2B,
    KEY_Z = 0x2C, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M,
    KEY_COMMA, KEY_PERIOD, KEY_SLASH,
    KEY_RSHIFT = 0x36,
    KEY_LALT = 0x38,
    KEY_SPACE = 0x39,
    KEY_CAPSLOCK = 0x3A,
    KEY_F1 = 0x3B,
    KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
} keycode_t;

/* ===== INPUT EVENTS ===== */
typedef enum {
    INPUT_EVENT_KEY_PRESS,
    INPUT_EVENT_KEY_RELEASE,
    INPUT_EVENT_MOUSE_MOVE,
    INPUT_EVENT_MOUSE_BUTTON_PRESS,
    INPUT_EVENT_MOUSE_BUTTON_RELEASE,
    INPUT_EVENT_SCROLL,
} input_event_type_t;

typedef struct {
    input_event_type_t type;
    uint64_t timestamp;
    
    union {
        struct {
            keycode_t keycode;
            char ascii;
            bool pressed;
        } key;
        
        struct {
            uint32_t x;
            uint32_t y;
            uint8_t buttons;  /* Bit mask: 0=left, 1=right, 2=middle */
        } mouse;
    } data;
} input_event_t;

/* ===== INPUT DEVICE INTERFACE ===== */
typedef struct {
    void (*init)(void);
    void (*enable)(void);
    void (*disable)(void);
    input_event_t (*get_event)(void);
    bool (*has_event)(void);
} input_device_t;

/* ===== INPUT MANAGER ===== */
void input_init(void);
input_event_t input_get_event(void);
bool input_has_event(void);
void input_register_device(input_device_t *device);
void input_process_events(void);

/* ===== EVENT HANDLERS ===== */
typedef void (*key_handler_t)(keycode_t key, bool pressed);
typedef void (*mouse_handler_t)(uint32_t x, uint32_t y, uint8_t buttons);

void input_register_key_handler(key_handler_t handler);
void input_register_mouse_handler(mouse_handler_t handler);

#endif /* INPUT_H */
