#pragma once

#define array_length(array) sizeof(array)/sizeof(*array)

typedef struct arena {
    u8 *base;
    size_t used;
    size_t cap;
    u32 stack_count;
} arena_t;

typedef struct temp_arena {
    arena_t *arena;
    size_t used;
} temp_arena_t;

static void initialize_arena(arena_t *arena, u8 *base, size_t cap) {
    arena->base = base;
    arena->used = 0;
    arena->cap = cap;
    arena->stack_count = 0;
}

#define push_size(arena, size) _push_arena(arena, size)
#define push_array(arena, type, count) (type *)_push_arena(arena, sizeof(type)*count)
#define push_struct(arena, type) (type *)_push_arena(arena, sizeof(type))
static inline void *_push_arena(arena_t *arena, size_t size) {
    assert(arena->used + size < arena->cap);
    void *result = arena->base + arena->used;
    arena->used += size;
    return result;
}

static inline temp_arena_t begin_temp_arena(arena_t *arena) {
    ++arena->stack_count;
    return (temp_arena_t) {
        .arena = arena,
        .used = arena->used
    };
}

static inline void end_temp_arena(temp_arena_t temp) {
    assert(temp.arena->stack_count > 0);
    assert(temp.used <= temp.arena->used);
    --temp.arena->stack_count;
    temp.arena->used = temp.used;
}
