#pragma once
#include <GLES3/gl3.h>

typedef struct vertex {
    vec3f position;
    vec4f color;
    vec2f uv;
    u32 texid;
} vertex_t;

typedef struct vertices {
    vertex_t *base;
    u32 count;
    u32 max_count;
} vertices_t;

#define MAX_QUAD_COUNT 1000
#define MAX_VERTICES_COUNT MAX_QUAD_COUNT*4
#define MAX_INDICES_COUNT MAX_QUAD_COUNT*6
typedef struct opengl_state {
    GLuint vao;
    GLuint vbo;
    GLuint ebo; // just 1 buffer with all the indices for quads
    GLuint ubo;

    GLuint proj;

    GLuint shader_program;

    vertices_t vertices;
} opengl_state_t;

#define check_glerror() {\
    GLenum err = glGetError();\
    if (err != GL_NO_ERROR) {\
        lerror("GL_ERROR: 0x%x", err);\
    }\
}

