#pragma once
#include "math.h"

typedef vec4f color_t;
#define COLOR_WHITE      (color_t){ .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f }
#define COLOR_BLACK      (color_t){ .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f }
#define COLOR_RED        (color_t){ .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f }
#define COLOR_BLUE       (color_t){ .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f }
#define COLOR_GREEN      (color_t){ .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f }
#define COLOR_LIGHTGRAY  (color_t){ .r = 0.8f, .g = 0.8f, .b = 0.82f, .a = 1.0f }
#define COLOR_DARKGRAY   (color_t){ .r = 0.1f, .g = 0.1f, .b = 0.12f, .a = 1.0f }

// TODO: flush out this idea
// Should this correspond to a kind of draw call?
typedef enum {
    RENDER_TYPE_render_command_clear_t,
    RENDER_TYPE_render_command_rectangle_t,
    RENDER_TYPE_render_command_texture_t
} render_type_t;

// NOTE: In HH, render_commands are "discriminated unions"
// basically, the size of data is different among commands for rectangles, textures, particles, etc
// so the push_command function does some safe-type management
typedef struct render_command_header {
    render_type_t type;
} render_command_header_t;

typedef struct render_command_clear {
    render_command_header_t header;
    r32 r, g, b, a;
} render_command_clear_t;

typedef struct render_command_rectangle {
    render_command_header_t header;
    vec3f position; // TODO: vec2? z sorting?
    vec2f extent;
    color_t color;
} render_command_rectangle_t;

typedef struct render_command_texture {
    render_command_header_t header;
    color_t color; // optional?
    vec3f position; // TODO: vec2? z sorting?
    vec2f extent;
    r32 texid;
} render_command_texture_t;

// IDEA: other types of render commands
//typedef struct render_command_particle {
//    render_command_header_t header;
//    vec3f position; // TODO: vec2? z sorting?
//    vec3f direction;
//    r32 spread;
//    r32 speed;
//    vec3f acceleration;
//    texid_t id;
//    TODO: how to do emission rate? theres some persistent state here.
//} render_command_texture_t;

typedef struct render_command_group {
    i32 meters_to_pixels;
    i32 screenwidth;
    i32 screenheight;

    void *renderer; // TODO: make this a descrimated union
    arena_t *scratch;

    u8 *base;
    size_t used;
    size_t cap;
} render_command_group_t;
