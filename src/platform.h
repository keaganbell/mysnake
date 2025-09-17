#pragma once
#include <stdlib.h>

#define DEFAULT_SCREENSIZE 800

typedef struct entire_file {
    u8 *data;
    size_t filesize;
} entire_file_t;

extern void *platform_initialize_renderer(arena_t *arena);
extern entire_file_t read_entire_file(const char *filename);
extern void free_file(entire_file_t file);
