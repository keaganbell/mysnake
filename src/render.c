#pragma once

static inline void initialize_render_commands(gamestate_t *game, render_command_group_t *commands) {
    // TODO: figure these numbers out
    commands->meters_to_pixels = game->world.coords.meters_to_pixels;
    commands->screenwidth = game->world.coords.screenwidth;
    commands->screenheight = game->world.coords.screenheight;
    commands->cap = (1<<12); // 4 kiB
    commands->base = push_size(&game->scratch_arena, commands->cap);
    commands->scratch = &game->scratch_arena;
    commands->renderer = game->renderer;
}

#define push_render_command(commands, type) (type *)_push_render_command(commands, sizeof(type), RENDER_TYPE_##type)
static inline render_command_header_t *_push_render_command(render_command_group_t *commands, size_t size, render_type_t type) {
    render_command_header_t *result = NULL;
    if (commands->used + size < commands->cap) {
        result = (render_command_header_t *)(commands->base + commands->used);
        result->type = type;
        commands->used += size;
    }
    else {
        lwarn("Render command buffer ran out of memory");
        //INVALID_CODEPATH
    }
    return result;
}

static inline void clear_background(render_command_group_t *commands, r32 r, r32 g, r32 b, r32 a) {
    render_command_clear_t *clear_command = push_render_command(commands, render_command_clear_t);
    if (clear_command) {
        clear_command->r = r;
        clear_command->g = g;
        clear_command->b = b;
        clear_command->a = a;
    }
}

static inline void draw_rectangle(render_command_group_t *commands, vec3f pos, vec2f extent, color_t color) {
    render_command_rectangle_t *rectangle = push_render_command(commands, render_command_rectangle_t);
    if (rectangle) {
        rectangle->position = pos;
        rectangle->extent = extent;
        rectangle->color = color;
    }
}
