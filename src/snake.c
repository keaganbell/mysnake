/*
 * TODO: Decide how much more work I want to put into this game.
 *
 * FONTS & TEXT:
 *
 * AUDIO:
 *  - BG Music
 *  - Sound effects
 *
 *
 * WORLD GEN:
 *
 * 
 *
 */
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "defines.h"
#include "log.h"
#include "memory.h"
#include "render.h"
#include "platform.h"
#include "input.h"
#include "snake.h"
#include "opengl.h"
#include "opengl.c"

// just hacking this together since i don't care about this project anymore
#if defined(_WIN64)
#include <windows.h>
void win32_init_opengl_functions() {
    glCreateShader = (gl_create_shader *)wglGetProcAddress("glCreateShader");
    glDeleteShader = (gl_delete_shader *)wglGetProcAddress("glDeleteShader");
    glShaderSource = (gl_shader_source *)wglGetProcAddress("glShaderSource");
    glCompileShader = (gl_compile_shader *)wglGetProcAddress("glCompileShader");
    glGetShaderInfoLog = (gl_get_shader_info_log *)wglGetProcAddress("glGetShaderInfoLog");
    glGetShaderiv = (gl_get_shaderiv *)wglGetProcAddress("glGetShaderiv");

    glCreateProgram = (gl_create_program *)wglGetProcAddress("glCreateProgram");
    glDeleteProgram = (gl_delete_program *)wglGetProcAddress("glDeleteProgram");
    glAttachShader = (gl_attach_shader *)wglGetProcAddress("glAttachShader");
    glLinkProgram = (gl_link_program *)wglGetProcAddress("glLinkProgram");
    glUseProgram = (gl_use_program *)wglGetProcAddress("glUseProgram");
    glValidateProgram = (gl_validate_program *)wglGetProcAddress("glValidateProgram");
    glGetProgramInfoLog = (gl_get_program_info_log *)wglGetProcAddress("glGetProgramInfoLog");
    glGetProgramiv = (gl_get_programiv *)wglGetProcAddress("glGetProgramiv");

    glGetUniformLocation = (gl_get_uniform_location *)wglGetProcAddress("glGetUniformLocation");
    glUniformMatrix4fv = (gl_uniform_matrix4fv *)wglGetProcAddress("glUniformMatrix4fv");
    glUniform1i = (gl_uniform1i *)wglGetProcAddress("glUniform1i");
    glUniform1f = (gl_uniform1f *)wglGetProcAddress("glUniform1f");
    glUniform2fv = (gl_uniform2fv *)wglGetProcAddress("glUniform2fv");
    glUniform3fv = (gl_uniform3fv *)wglGetProcAddress("glUniform3fv");
    glUniform4fv = (gl_uniform4fv *)wglGetProcAddress("glUniform4fv");

    glGenVertexArrays = (gl_gen_vertex_arrays *)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (gl_bind_vertex_array *)wglGetProcAddress("glBindVertexArray");
    glGenBuffers = (gl_gen_buffers *)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (gl_bind_buffer *)wglGetProcAddress("glBindBuffer");
    glBufferData = (gl_buffer_data *)wglGetProcAddress("glBufferData");
    glBufferSubData = (gl_buffer_sub_data *)wglGetProcAddress("glBufferSubData");
    glBindBufferBase = (gl_bind_buffer_base *)wglGetProcAddress("glBindBufferBase");

    glDrawElementsBaseVertex = (gl_draw_elements_base_vertex *)wglGetProcAddress("glDrawElementsBaseVertex");

    glGetAttribLocation = (gl_get_attrib_location *)wglGetProcAddress("glGetAttribLocation");
    glBindAttribLocation = (gl_bind_attrib_location *)wglGetProcAddress("glBindAttribLocation");
    glEnableVertexAttribArray = (gl_enable_vertex_attrib_array *)wglGetProcAddress("glEnableVertexAttribArray");
    glDisableVertexAttribArray = (gl_disable_vertex_attrib_array *)wglGetProcAddress("glDisableVertexAttribArray");
    glVertexAttribPointer = (gl_vertex_attrib_pointer *)wglGetProcAddress("glVertexAttribPointer");
    glVertexAttribIPointer = (gl_vertex_attribi_pointer *)wglGetProcAddress("glVertexAttribIPointer");

    glGenFramebuffers = (gl_gen_framebuffers *)wglGetProcAddress("glGenFramebuffers");
    glBindFramebuffer = (gl_bind_framebuffer *)wglGetProcAddress("glBindFramebuffer");
    glFramebufferTexture2D = (gl_framebuffer_texture_2d *)wglGetProcAddress("glFramebufferTexture2D");
    glCheckFramebufferStatus = (gl_check_framebuffer_status *)wglGetProcAddress("glCheckFramebufferStatus");
    glTexImage2DMultisample = (gl_tex_image_2d_multisample *)wglGetProcAddress("glTexImage2DMultisample");
    glBlitFramebuffer = (gl_blit_framebuffer *)wglGetProcAddress("glBlitFramebuffer");

    glGenRenderbuffers = (gl_gen_render_buffers *)wglGetProcAddress("glGenRenderbuffers");
    glBindRenderbuffer = (gl_bind_renderbuffer *)wglGetProcAddress("glBindRenderbuffer");
    glRenderbufferStorageMultisample = (gl_render_buffer_storage_multisample *)wglGetProcAddress("glRenderbufferStorageMultisample");
    glFramebufferRenderbuffer = (gl_framebuffer_renderbuffer *)wglGetProcAddress("glFramebufferRenderbuffer");

    //glDebugMessageCallback = (gl_debug_message_callback *)wglGetProcAddress("glDebugMessageCallback");
}
#endif

void *initialize_renderer(arena_t *arena) {
    opengl_state_t *gl = push_struct(arena, opengl_state_t);
    win32_init_opengl_functions();
    initialize_opengl_state(gl);
    return gl;
}

entire_file_t read_entire_file(const char *filename) {
    entire_file_t result = {};
    FILE *file = fopen(filename, "rb");
    if (!file) {
        lerror("Failed to locate file: %s", filename);
        return result;
    }
    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);
    result.data = malloc(filesize + 1);
    fread(result.data, 1, filesize, file);
    fclose(file);
    result.data[filesize] = '\0';
    return result;
}

void free_file(entire_file_t file) {
    assert(file.data);
    free(file.data);
}

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
        game->renderer = initialize_renderer(&game->permanent_arena);

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

    opengl_process_render_commands(render_commands);
}
