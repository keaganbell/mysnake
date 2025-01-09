#pragma once
#include <math.h>

typedef struct vec2i {
    union {
        i32 elements[2];
        struct {
            union {
                i32 x;
                i32 width;
            };
            union {
                i32 y;
                i32 height;
            };
        };
    };
} vec2i;

typedef struct vec2f {
    union {
        r32 elements[2];
        struct {
            union {
                r32 x;
                r32 u;
                r32 width;
            };
            union {
                r32 y;
                r32 v;
                r32 height;
            };
        };
    };
} vec2f;

typedef struct vec3i {
    union {
        i32 elements[3];
        struct {
            union {
                i32 x;
                i32 r;
            };
            union {
                i32 y;
                i32 g;
            };
            union {
                i32 z;
                i32 b;
            };
        };
    };
} vec3i;

typedef struct vec3f {
    union {
        r32 elements[3];
        struct {
            union {
                r32 x;
                r32 r;
            };
            union {
                r32 y;
                r32 g;
            };
            union {
                r32 z;
                r32 b;
            };
        };
    };
} vec3f;

typedef struct vec4i {
    union {
        i32 elements[4];
        struct {
            union {
                i32 x;
                i32 r;
                i32 tl;
            };
            union {
                i32 y;
                i32 g;
                i32 tr;
            };
            union {
                i32 z;
                i32 b;
                i32 bl;
                i32 width;
            };
            union {
                i32 w;
                i32 a;
                i32 br;
                i32 height;
            };
        };
    };
} vec4i;

typedef struct vec4f {
    union {
        r32 elements[4];
        struct {
            union {
                r32 x;
                r32 r;
                r32 tl;
            };
            union {
                r32 y;
                r32 g;
                r32 tr;
            };
            union {
                r32 z;
                r32 b;
                r32 bl;
                r32 width;
            };
            union {
                r32 w;
                r32 a;
                r32 br;
                r32 height;
            };
        };
    };
} vec4f;

typedef union {
    r32 elements[16];
    struct {
        r32 a;
        r32 b;
        r32 c;
        r32 d;
        r32 e;
        r32 f;
        r32 g;
        r32 h;
        r32 i;
        r32 j;
        r32 k;
        r32 l;
        r32 m;
        r32 n;
        r32 o;
        r32 p;
    };
} mat4f;
#define MAT4F_IDENTITY (mat4f){ \
    1.0f, 0.0f, 0.0f, 0.0f, \
    0.0f, 1.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 1.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, 1.0f  \
}
#define diagmat4(x, y, z, w) (mat4f){\
    x, 0.0f. 0.0f, 0.0f,\
    0.0f. y, 0.0f, 0.0f,\
    0.0f, 0.0f, z, 0.0f,\
    0.0f, 0.0f, 0.0f, w\
}

static inline vec4f scale_vec4f(vec4f v, r32 a) {
    return (vec4f){ .x = v.x*a, .y = v.y*a, .z = v.z*a, .w = v.w*a };
}

static inline vec3f add_vec3f(vec3f a, vec3f b) {
    return (vec3f){ .x = a.x+b.x, .y = a.y+b.y, .z = a.z+b.z };
}

static inline vec2i add_vec2i(vec2i a, vec2i b) {
    return (vec2i){ a.x + b.x, a.y + b.y };
}

#define vec2i(x, y) (vec2i){x, y}
#define vec2f(x, y) (vec2f){x, y}
#define vec3i(x, y, z) (vec3i){x, y, z}
#define vec3f(x, y, z) (vec3f){x, y, z}
#define vec4f(x, y, z) (vec4f){x, y, z}

#ifdef APP_IMPL
#define MATH_IMPL
#endif
#ifdef MATH_IMPL

#endif
