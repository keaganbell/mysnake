#pragma once

static inline u32 rand_custom(u32 *seed) {
    *seed = (1664525 * *seed + 1013904223) % (1<<31);
    return *seed;
}

static inline u32 rand_range(i32 min, i32 max, u32 *seed) {
    return min + (rand_custom(seed) % (max - min + 1));
}

typedef struct game_memory {
    b32 initialized;

    size_t permanent_size;
    void *permanent;

    size_t renderer_size;
    void *renderer;

    size_t scratch_size;
    void *scratch;
} game_memory_t;

typedef enum {
    FACING_LEFT,
    FACING_UP,
    FACING_RIGHT,
    FACING_DOWN
} facing_direction_t;

typedef struct coordinates {
    u32 screenwidth; // pixel space
    u32 screenheight; // pixel space
    u32 gridx; // grid space
    u32 gridy; // grid space
    r32 boundsx; // world space (camera width/height)
    r32 boundsy; // world space
    i32 meters_to_pixels;
} coordinates_t;

typedef struct snake { 
    vec2i position;
    vec2i direction;
    r32 movetimer;
    r32 time_per_move;
    u32 length;
    b32 alive;
} snake_t;

typedef enum {
    CELL_UNOCCUPIED,
    CELL_OCCUPANT_FOOD,
    CELL_OCCUPANT_SNAKE
} occupant_type_t;

typedef struct cell {
    occupant_type_t occupant;
    u32 currenttime; // in ticks
} cell_t;

#define DEFAULT_GRIDSIZE 30
#define DEFAULT_CELLS_PER_SCREEN DEFAULT_GRIDSIZE*DEFAULT_GRIDSIZE
typedef struct world {
    b32 is_tick;
    b32 ate;
    r32 spawntimer;
    r32 seconds_per_spawn;
    snake_t snake;
    coordinates_t coords;
    cell_t cells[DEFAULT_CELLS_PER_SCREEN];
} world_t;

#define DEFAULT_METERS_TO_PIXELS 100
#define DEFAULT_WORLD_SIZE_METERS 800.0f/DEFAULT_METERS_TO_PIXELS
typedef struct gamestate {
    arena_t permanent_arena;
    arena_t scratch_arena;

    // world stuff
    b32 paused;
    r32 gametime;
    u32 randnum;
    world_t world;

    void *renderer; // TODO: make this a descriminated union

    // assets_t *assets;
    //
    // player_controller_t controllers[];
    //
    // arena_t audio_arena;
    // audio_state_t audio;
    //
    // game_mode_t game_mode;
    // union {
    //     game_mode_title_screen_t *title_screen;
    //     game_mode_cutscene_t *cut_scene;
    //     game_mode_playing_t *play_mode;
    // };
} gamestate_t;

typedef struct input_state input_state_t;
typedef struct gameapi {
    void (*update_and_render)(game_memory_t *memory, render_command_group_t *commands, input_state_t *input);
} gameapi_t;
