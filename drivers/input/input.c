/*
 * Input System Implementation
 * Handles keyboard and mouse input
 */

#include <drivers/input.h>
#include <kernel/kernel.h>
#include <stddef.h>

/* ===== INPUT DEVICE MANAGEMENT ===== */
#define MAX_INPUT_DEVICES 16

static input_device_t *input_devices[MAX_INPUT_DEVICES];
static uint32_t num_devices = 0;

static key_handler_t key_handler = NULL;
static mouse_handler_t mouse_handler = NULL;

/* ===== PS/2 KEYBOARD DRIVER (Stub) ===== */
static input_event_t ps2_key_buffer[256];
static uint32_t ps2_key_buffer_pos = 0;

static void ps2_keyboard_init(void) {
    KINFO("PS/2 Keyboard initialized");
}

static void ps2_keyboard_enable(void) {
    // Enable PS/2 keyboard interrupt (IRQ 1)
}

static void ps2_keyboard_disable(void) {
    // Disable PS/2 keyboard interrupt
}

static input_event_t ps2_keyboard_get_event(void) {
    input_event_t event = {0};
    if (ps2_key_buffer_pos > 0) {
        event = ps2_key_buffer[0];
        for (int i = 1; i < ps2_key_buffer_pos; i++) {
            ps2_key_buffer[i - 1] = ps2_key_buffer[i];
        }
        ps2_key_buffer_pos--;
    }
    return event;
}

static bool ps2_keyboard_has_event(void) {
    return ps2_key_buffer_pos > 0;
}

static input_device_t ps2_keyboard_device = {
    .init = ps2_keyboard_init,
    .enable = ps2_keyboard_enable,
    .disable = ps2_keyboard_disable,
    .get_event = ps2_keyboard_get_event,
    .has_event = ps2_keyboard_has_event,
};

/* ===== MOUSE DRIVER (Stub) ===== */
static input_event_t mouse_buffer[256];
static uint32_t mouse_buffer_pos = 0;

static void mouse_init(void) {
    KINFO("PS/2 Mouse initialized");
}

static void mouse_enable(void) {
    // Enable PS/2 mouse interrupt (IRQ 12)
}

static void mouse_disable(void) {
    // Disable PS/2 mouse interrupt
}

static input_event_t mouse_get_event(void) {
    input_event_t event = {0};
    if (mouse_buffer_pos > 0) {
        event = mouse_buffer[0];
        for (int i = 1; i < mouse_buffer_pos; i++) {
            mouse_buffer[i - 1] = mouse_buffer[i];
        }
        mouse_buffer_pos--;
    }
    return event;
}

static bool mouse_has_event(void) {
    return mouse_buffer_pos > 0;
}

static input_device_t mouse_device = {
    .init = mouse_init,
    .enable = mouse_enable,
    .disable = mouse_disable,
    .get_event = mouse_get_event,
    .has_event = mouse_has_event,
};

/* ===== INPUT SYSTEM ===== */
void input_init(void) {
    KINFO("Input system initializing...");
    
    /* Register default devices */
    input_register_device(&ps2_keyboard_device);
    input_register_device(&mouse_device);
    
    /* Initialize all devices */
    for (uint32_t i = 0; i < num_devices; i++) {
        if (input_devices[i]->init) {
            input_devices[i]->init();
        }
        if (input_devices[i]->enable) {
            input_devices[i]->enable();
        }
    }
    
    KINFO("Input system ready (%d devices)", num_devices);
}

void input_register_device(input_device_t *device) {
    if (num_devices < MAX_INPUT_DEVICES) {
        input_devices[num_devices++] = device;
    }
}

input_event_t input_get_event(void) {
    for (uint32_t i = 0; i < num_devices; i++) {
        if (input_devices[i]->has_event && input_devices[i]->has_event()) {
            return input_devices[i]->get_event();
        }
    }
    
    input_event_t empty = {0};
    return empty;
}

bool input_has_event(void) {
    for (uint32_t i = 0; i < num_devices; i++) {
        if (input_devices[i]->has_event && input_devices[i]->has_event()) {
            return true;
        }
    }
    return false;
}

void input_process_events(void) {
    while (input_has_event()) {
        input_event_t event = input_get_event();
        
        if (event.type == INPUT_EVENT_KEY_PRESS || event.type == INPUT_EVENT_KEY_RELEASE) {
            if (key_handler) {
                key_handler(event.data.key.keycode, event.type == INPUT_EVENT_KEY_PRESS);
            }
        } else if (event.type == INPUT_EVENT_MOUSE_MOVE) {
            if (mouse_handler) {
                mouse_handler(event.data.mouse.x, event.data.mouse.y, 
                            event.data.mouse.buttons);
            }
        }
    }
}

void input_register_key_handler(key_handler_t handler) {
    key_handler = handler;
}

void input_register_mouse_handler(mouse_handler_t handler) {
    mouse_handler = handler;
}
