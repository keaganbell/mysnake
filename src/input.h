#pragma once

// alternatively could do bit mask
typedef struct button_state {
    b32 pressed;
    b32 released;
    b32 up;
    b32 down;
} button_state_t;

typedef enum {
    KEY_ESCAPE,
    KEY_SPACE,
    KEY_ENTER,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,

    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,

    BUTTON_MAX_COUNT
} buttonkeys_t;

typedef struct input_state {
   b32 resized;
   u32 screenwidth; 
   u32 screenheight; 
   r32 frametime; // in seconds?
   r64 mousex;
   r64 mousey;
   button_state_t buttons[BUTTON_MAX_COUNT];
   b32 shift, alt, control;
} input_state_t;
