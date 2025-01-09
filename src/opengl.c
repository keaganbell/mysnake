#pragma once

static GLuint create_shader(GLenum type, const char *shader_source) {
    switch (type) {
        case GL_VERTEX_SHADER: {
            linfo("Loaded vertex shader");
            break;
        }
        case GL_FRAGMENT_SHADER: {
            linfo("Loaded fragment shader");
            break;
        }
    }
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char logbuffer[512];
        glGetShaderInfoLog(shader, 512, NULL, logbuffer);
        lerror("Shader compilation failed: %s", logbuffer);
    }
    return shader;
}

static GLuint make_shader_program(GLuint vertex_shader, GLuint fragment_shader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char logbuffer[512];
        glGetProgramInfoLog(program, 512, NULL, logbuffer);
        lerror("Program link failed: %s", logbuffer);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

static GLuint load_shaders() {
    entire_file_t vert_file = read_entire_file("ass/shaders/vertex.glsl");
    if (vert_file.data == NULL) {
        lerror("Failed to load vertex shader.");
        return false;
    }
    entire_file_t frag_file = read_entire_file("ass/shaders/fragment.glsl");
    if (frag_file.data == NULL) {
        lerror("Failed to load fragment shader.");
        return false;
    }
    GLuint vert_shader = create_shader(GL_VERTEX_SHADER, (char *)vert_file.data);
    GLuint frag_shader = create_shader(GL_FRAGMENT_SHADER, (char *)frag_file.data);
    GLuint program = make_shader_program(vert_shader, frag_shader);
    check_glerror();
    free_file(vert_file);
    free_file(frag_file);
    return program;
}

static void initialize_opengl_state(opengl_state_t *gl) {
    gl->shader_program = load_shaders();
    glUseProgram(gl->shader_program);
    glGenVertexArrays(1, &gl->vao);
    glBindVertexArray(gl->vao);

    // vertex buffer object
    glGenBuffers(1, &gl->vbo); // generate 1 vbo
    glBindBuffer(GL_ARRAY_BUFFER, gl->vbo); // bind the vbo
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES_COUNT*sizeof(vertex_t), NULL, GL_DYNAMIC_DRAW);

    // index buffer object
    // TODO: get these off the stack and use pools so that the size of the array
    // can be variable.
    u32 indices[MAX_INDICES_COUNT] = {};
    u32 offset = 0;
    for (size_t i = 0; i < MAX_INDICES_COUNT; i += 6) {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;
        indices[i + 3] = 3 + offset;
        indices[i + 4] = 2 + offset;
        indices[i + 5] = 0 + offset;
        offset += 4;
    }
    glGenBuffers(1, &gl->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // uniform buffer
    glGenBuffers(1, &gl->ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, gl->ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, gl->ubo);
    gl->proj = glGetUniformLocation(gl->shader_program, "proj"); // FIXME
    check_glerror();

    // vertex attributes
    GLint position_index = glGetAttribLocation(gl->shader_program, "_position"); // get the loc in shader
    glVertexAttribPointer(position_index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid *)offsetof(vertex_t, position));
    GLint color_index = glGetAttribLocation(gl->shader_program, "_color"); // get the loc in shader
    glVertexAttribPointer(color_index, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid *)offsetof(vertex_t, color));
    GLint uv_index = glGetAttribLocation(gl->shader_program, "_uv"); // get the loc in shader
    glVertexAttribPointer(uv_index, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid *)offsetof(vertex_t, uv));
    GLint texid_index = glGetAttribLocation(gl->shader_program, "_texid"); // get the loc in shader
    glVertexAttribPointer(texid_index, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(vertex_t), (GLvoid *)offsetof(vertex_t, texid));

    glEnableVertexAttribArray(position_index);
    glEnableVertexAttribArray(color_index);
    glEnableVertexAttribArray(uv_index);
    check_glerror();
}

static inline void initialize_vertices(arena_t *scratch, vertices_t *verts) {
    verts->base = push_array(scratch, vertex_t, MAX_VERTICES_COUNT);
    verts->count = 0;
    verts->max_count = MAX_VERTICES_COUNT;
}

static inline vertex_t *push_vertex(vertices_t *verts) {
    assert(verts->count < verts->max_count);
    vertex_t *result = verts->base + verts->count;
    ++verts->count;
    return result;
}

static inline void render_rectangle(vertices_t *verts, render_command_rectangle_t *rectangle) {
    // bottom left
    vertex_t *vertex2 = push_vertex(verts);
    vertex2->position = rectangle->position;
    vertex2->color = rectangle->color;
    vertex2->uv.u = 0.0f;
    vertex2->uv.v = 0.0f;

    // top left
    vertex_t *vertex0 = push_vertex(verts);
    vertex0->position = add_vec3f(rectangle->position, (vec3f){ .y = rectangle->extent.height });
    vertex0->color = rectangle->color;
    vertex0->uv.u = 0.0f;
    vertex0->uv.v = 1.0f;

    // bottom right
    vertex_t *vertex3 = push_vertex(verts);
    vertex3->position = add_vec3f(rectangle->position, (vec3f){ .x = rectangle->extent.width, .y = rectangle->extent.height });
    vertex3->color = rectangle->color;
    vertex3->uv.u = 1.0f;
    vertex3->uv.v = 1.0f;

    // top right
    vertex_t *vertex1 = push_vertex(verts);
    vertex1->position = add_vec3f(rectangle->position, (vec3f){ .x = rectangle->extent.width });
    vertex1->color = rectangle->color;
    vertex1->uv.u = 1.0f;
    vertex1->uv.v = 0.0f;
}

// OVERVIEW
// STEP 0: initialize the buffers for the frame
// STEP 1: process the commands and populate the buffers
// STEP 2: upload buffers to the GPU
// STEP 3: make draw calls
// REPEAT steps 2 and 3 until everything is drawn
SNAPI void opengl_process_render_commands(render_command_group_t *render_command_group) {
    temp_arena_t scratch = begin_temp_arena(render_command_group->scratch);
    opengl_state_t *gl = (opengl_state_t *)render_command_group->renderer;
    initialize_vertices(scratch.arena, &gl->vertices);

    i32 target_width = 800;
    i32 target_height = 800;
    i32 screenwidth = render_command_group->screenwidth;
    i32 screenheight = render_command_group->screenheight;
    r32 screen_aspect_ratio = (r32)screenwidth/screenheight;
    r32 aspect_ratio = (r32)target_width/target_height;
    r32 scale;
    if (screen_aspect_ratio >= aspect_ratio) {
        // Screen is wider than the game aspect ratio
        scale = (r32)screenheight/target_height;
        r32 scaled_width = target_width*scale;
        glViewport((screenwidth - scaled_width)/2, 0, scaled_width, screenheight);
    } else {
        // Screen is taller than the game aspect ratio
        scale = (r32)screenwidth/target_width;
        r32 scaled_height = target_height*scale;
        glViewport(0, (screenheight - scaled_height) / 2, screenwidth, scaled_height);
    }

    // STEP 1: process the commands and populate the buffers
    for (size_t command_offset = 0; command_offset < render_command_group->used; ) {
        render_command_header_t *typeless = (render_command_header_t *)(render_command_group->base + command_offset);
        switch (typeless->type) {
            case RENDER_TYPE_render_command_clear_t: {
                render_command_clear_t *command = (render_command_clear_t *)typeless;
                glClearColor(command->r, command->g, command->b, command->a);
                command_offset += sizeof(*command);
                break;
            }
            case RENDER_TYPE_render_command_rectangle_t: {
                render_command_rectangle_t *command = (render_command_rectangle_t *)typeless;
                command_offset += sizeof(*command);
                render_rectangle(&gl->vertices, command);
                break;
            }
            case RENDER_TYPE_render_command_texture_t: {
                render_command_texture_t *command = (render_command_texture_t *)typeless;
                command_offset += sizeof(*command);
                // TODO: push on some vertices
                break;
            }

            // more cases:
            //  - text?
            //  - particles?
            //  - complicated meshes? snake?

            INVALID_DEFAULT_CASE
        }
        check_glerror();
    }

    // STEP 2: upload buffers to the GPU
    r32 a = (r32)2.0f*render_command_group->meters_to_pixels/target_width;
    r32 b = (r32)2.0f*render_command_group->meters_to_pixels/target_height;
    mat4f proj = {
            a,  0.0f, 0.0f, 0.0f,
         0.0f,     b, 0.0f, 0.0f,
         0.0f,  0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f
    };
    glUniformMatrix4fv(gl->proj, 1, GL_FALSE, proj.elements);
    check_glerror();

    glBufferSubData(GL_ARRAY_BUFFER, 0, gl->vertices.count*sizeof(vertex_t), gl->vertices.base);

    // STEP 3: make draw calls
    if (gl->vertices.count % 2 != 0) {
        lwarn("number of vertices is not divisible by 2. this means the number of indices to draw is incorrect");
    }
    // TODO: draw something other than batched quads
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, gl->vertices.count*6/4, GL_UNSIGNED_INT, 0);
    check_glerror();

    end_temp_arena(scratch);
}
