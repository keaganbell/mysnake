/*
 * TODO: Decide how much more work I want to put into this game.
 *
 * FONTS & TEXT:
 *
 * AUDIO:
 *  - BG Music
 *  - Sound effects
 *
 * STATE MACHINE:
 *
 * WORLD GEN:
 *
 * 
 *
 */
#include "defines.h"
#include "log.h"
#include "memory.h"
#include "render.h"
#include "platform.h"
#include "input.h"
#include "snake.h"

#include "render.c"

#define SNAKE_COLOR (color_t){ 0.65f, 0.63f, 0.61f, 1.0f }
#define GRID_COLOR (color_t){ 0.12f, 0.2f, 0.31f, 1.0f }

static inline vec3f grid_to_world(coordinates_t coords, vec2i gridpoint) {
    return (vec3f) {
        .x = (r32)gridpoint.x*coords.boundsx/coords.gridx,
        .y = (r32)gridpoint.y*coords.boundsy/coords.gridy
    };
}

static inline vec2i world_to_grid(coordinates_t coords, vec3f worldpoint) {
    return (vec2i) {
        .x = (r32)worldpoint.x/coords.boundsx*coords.gridx,
        .y = (r32)worldpoint.y/coords.boundsy*coords.gridy
    };
}

static inline cell_t *get_cell_at_gridpoint(world_t *world, vec2i gridpoint) {
    cell_t *result = NULL;
    i32 index = gridpoint.y*world->coords.gridx + gridpoint.x;
    assert(index < DEFAULT_CELLS_PER_SCREEN && index > 0);
    result = world->cells + index;
    return result;
}

// NOTE: orthographic only
static inline vec3f screen_to_world(coordinates_t coords, vec2i screenpoint) {
    return (vec3f) {
        .x = (r32)screenpoint.x/coords.meters_to_pixels,
        .y = (r32)screenpoint.y/coords.meters_to_pixels
    };
}

static inline void render_grid_background(render_command_group_t *commands, coordinates_t coords) {
    r32 boundsx = coords.boundsx;
    r32 boundsy = coords.boundsy;
    r32 gridx = coords.gridx;
    r32 gridy = coords.gridy;
    r32 linewidth = 0.05f;
    for (u32 i = 0; i <= gridx; ++i) {
        vec3f pos = { i*boundsx/gridx - linewidth/2.0f, 0.0f, 0.9f };
        vec2f extent = { linewidth, boundsy };
        draw_rectangle(commands, pos, extent, GRID_COLOR);
    }
    for (u32 i = 0; i <= gridy; ++i) {
        vec3f pos = { 0.0f, i*boundsy/gridy - linewidth/2.0f, 0.9f };
        vec2f extent = { boundsx, linewidth };
        draw_rectangle(commands, pos, extent, GRID_COLOR);
    }
}

static inline void render_snake(render_command_group_t *commands, vec2i pos, coordinates_t coords) {
    vec3f renderpos = grid_to_world(coords, pos);
    vec2f extent = { 
        .width = (r32)coords.boundsx/coords.gridx,
        .height = (r32)coords.boundsy/coords.gridy
    };
    draw_rectangle(commands, renderpos, extent, SNAKE_COLOR);
}

static inline void render_food(render_command_group_t *commands, vec2i spawnpoint, coordinates_t coords) {
    vec3f renderpos = grid_to_world(coords, spawnpoint);
    vec2f extent = { 
        .width = (r32)coords.boundsx/coords.gridx,
        .height = (r32)coords.boundsy/coords.gridy
    };
    color_t color = { 1.f, 0.1f, 0.1f, 1.0f };
    draw_rectangle(commands, renderpos, extent, color);

}

static inline void render_gameplay_timer(render_command_group_t *commands, coordinates_t coords, r32 time) {
    // background
    {
        r32 paddingx = 0.1f;
        r32 paddingy = 0.1f;
        r32 background_width = 0.5f;
        vec3f renderpos = vec3f(paddingx, coords.boundsy - background_width - paddingy, 1.f);
        vec2f extent = { 
            .width = coords.boundsx - 2.f*paddingx,
            .height = background_width
        };
        color_t color = { 0.22f, 0.2f, 0.21f, 1.0f };
        draw_rectangle(commands, renderpos, extent, color);
    }

    // forground
    {
        r32 paddingx = 0.2f;
        r32 paddingy = 0.2f;
        r32 foreground_width = 0.3f;
        r32 foreground_length = coords.boundsx - 2.f*paddingx;
        if (time > 60.f) {
            time -= 60.f;
        }
        vec3f renderpos = vec3f(paddingx, coords.boundsy - foreground_width - paddingy, 0.9f);
        vec2f extent = { 
            .width = foreground_length*time/60.f,
            .height = foreground_width
        };
        color_t color = { 0.42f, 0.4f, 0.21f, 1.0f };
        draw_rectangle(commands, renderpos, extent, color);
    }
}

static inline void set_snake_direction(vec2i *dir, button_state_t *buttons) {
    if (buttons[KEY_LEFT].pressed && dir->x != 1) {
        *dir = vec2i(-1, 0);
    }
    else if (buttons[KEY_RIGHT].pressed && dir->x != -1) {
        *dir = vec2i(1, 0);
    }
    else if (buttons[KEY_DOWN].pressed && dir->y != 1) {
        *dir = vec2i(0, -1);
    }
    else if (buttons[KEY_UP].pressed && dir->y != -1) {
        *dir = vec2i(0, 1);
    }
}

static inline void move_snake(world_t *world) {
    snake_t *snake = &world->snake;
    if (snake->movetimer > snake->time_per_move) {
        snake->movetimer = 0.f;
        world->is_tick = true;

        vec2i nextpos = add_vec2i(snake->position, snake->direction);
        cell_t *cell = get_cell_at_gridpoint(world, nextpos);
        if (cell->occupant == CELL_UNOCCUPIED) {
            snake->position = nextpos;
            cell->currenttime = snake->length + 1;
            cell->occupant = CELL_OCCUPANT_SNAKE;
        }
        else if (cell->occupant == CELL_OCCUPANT_FOOD) {
            snake->position = nextpos;
            cell->currenttime = snake->length + 1;
            cell->occupant = CELL_OCCUPANT_SNAKE;
            ++snake->length;
            world->ate = true;
        }
        else if (cell->occupant == CELL_OCCUPANT_SNAKE) {
            world->snake.alive = false;
        }
    }
    else {
        world->is_tick = false;
    }
}

static inline void initialize_world(world_t *world) {
        world->coords.gridx = DEFAULT_GRIDSIZE;
        world->coords.gridy = DEFAULT_GRIDSIZE;
        world->coords.boundsx = DEFAULT_WORLD_SIZE_METERS; // in meters
        world->coords.boundsy = DEFAULT_WORLD_SIZE_METERS; // these could change with zoom
        world->coords.meters_to_pixels = DEFAULT_METERS_TO_PIXELS;
        world->coords.screenwidth = DEFAULT_SCREENSIZE;
        world->coords.screenheight = DEFAULT_SCREENSIZE;

        world->snake.position = vec2i(5, 5);
        world->snake.direction = vec2i(0, 0);
        world->snake.movetimer = 0;
        world->snake.time_per_move = 1.0f;
        world->snake.alive = true;
        world->snake.length = 1;

        world->seconds_per_spawn = 5.0f;
        world->spawntimer = 5.0f;
}

static void render_grid(render_command_group_t * commands, world_t *world) {
    i32 gridy = world->coords.gridy;
    i32 gridx = world->coords.gridx;
    i32 boundsx = world->coords.boundsx;
    i32 boundsy = world->coords.boundsy;
    for (i32 j = 0; j < gridy; ++j) {
        for(i32 i = 0; i < gridx; ++i) {
            i32 index = j*gridx + i;
            assert(index < DEFAULT_CELLS_PER_SCREEN); // FIXME: hardcoded gridsize
            cell_t *cell = world->cells + index;
            switch (cell->occupant) {
                case CELL_UNOCCUPIED: break;
                case CELL_OCCUPANT_FOOD: {
                    vec3f pos = grid_to_world(world->coords, vec2i(i, j));
                    vec2f extent = { 
                        .width = (r32)boundsx/gridx,
                        .height = (r32)boundsy/gridy
                    };
                    color_t color = { 0.62f, 0.1f, 0.11f, 1.0f };
                    draw_rectangle(commands, pos, extent, color);
                } break;
                case CELL_OCCUPANT_SNAKE: {
                    if (cell->currenttime > 0) {
                        if (world->is_tick && !world->ate) {
                            --cell->currenttime;
                        }
                        vec3f pos = grid_to_world(world->coords, vec2i(i, j));
                        vec2f extent = { 
                            .width = (r32)boundsx/gridx,
                            .height = (r32)boundsy/gridy
                        };
                        draw_rectangle(commands, pos, extent, SNAKE_COLOR);
                    }
                    else {
                        cell->occupant = CELL_UNOCCUPIED;
                    }
                } break;
            }
        }
    }
}

void game_update_and_render(game_memory_t *memory, render_command_group_t *render_commands, input_state_t *input/*, audio_samples*/) {
    // INITIALIZE THE FRAME
    // NOTE: the gamestate is assumed to be the first thing on the permanent memory store
    gamestate_t *game = (gamestate_t *)memory->permanent;
    if (!memory->initialized) {
        // called once
        initialize_arena(&game->permanent_arena, memory->permanent, memory->permanent_size);
        game->permanent_arena.used += sizeof(gamestate_t);

        // initialize renderer
        game->renderer = platform_initialize_renderer(&game->permanent_arena);

        game->paused = false;
        game->randnum = 1; // seed
        initialize_world(&game->world);

        memory->initialized = true;
    }
    // Scratch arena is reset and initialized every frame
    initialize_arena(&game->scratch_arena, memory->scratch, memory->scratch_size);

    // NOTE: because window resize events come from the OS we need to make
    // sure the gamestate is up to date with that info. This needs to happen before
    // render commands get initialized
    if (input->resized) {
        game->world.coords.screenwidth = input->screenwidth;
        game->world.coords.screenheight = input->screenheight;
    }
    initialize_render_commands(game, render_commands);

    // UPDATE AND RENDER
    clear_background(render_commands, 0.1f, 0.1f, 0.12f, 1.0f);

    render_grid_background(render_commands, game->world.coords);

    if (input->buttons[KEY_SPACE].pressed) {
        game->paused = !game->paused;
    }
    if (!game->paused) {
        game->gametime += input->frametime;
        set_snake_direction(&game->world.snake.direction, input->buttons);

        game->world.snake.movetimer += input->frametime;
        game->world.ate = false;
        move_snake(&game->world);

        game->world.spawntimer += input->frametime;
        if (game->world.spawntimer > game->world.seconds_per_spawn) {
            game->world.spawntimer = 0;
            i32 x = rand_range(0, game->world.coords.gridx - 1, &game->randnum);
            i32 y = rand_range(0, game->world.coords.gridy - 1, &game->randnum);
            vec2i spawnpoint = vec2i(x, y);
            cell_t *cell = get_cell_at_gridpoint(&game->world, spawnpoint);
            cell->occupant = CELL_OCCUPANT_FOOD;
        }
    }

    render_grid(render_commands, &game->world);

    render_snake(render_commands, game->world.snake.position, game->world.coords);

    render_gameplay_timer(render_commands, game->world.coords, game->gametime);
}
