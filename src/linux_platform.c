#ifdef PLATFORM_LINUX
#include <sys/inotify.h> // inotify
#include <sys/mman.h> // mmap
#include <unistd.h> // usleep, read
#include <dlfcn.h> // dlopen, dlclose, dlsym
#include <fcntl.h>

#include <GLFW/glfw3.h>
#include <stdlib.h> // malloc (loading files)

#include "defines.h"
#include "log.h"
#include "memory.h"
#include "render.h"
#include "opengl.h"
#include "snake.h"
#include "platform.h"
#include "input.h"

typedef struct linux_state {
    GLFWwindow *window;
    i32 screenwidth;
    i32 screenheight;
    i32 file_descriptor;
    i32 watch_descriptor;
    input_state_t input;
    void *gamelib;
    void (*game_update_and_render)(game_memory_t *memory, render_command_group_t *render_commands, input_state_t *input);
} linux_state_t;
static linux_state_t g_linux_state;
#include "linux_input.c"
#include "linux_hotreload.c"
#include "opengl.c"

void *platform_initialize_renderer(arena_t *arena) {
    opengl_state_t *gl = push_struct(arena, opengl_state_t);
    initialize_opengl_state(gl);
    return gl;
}
static void initialize_window_and_opengl() {
    b32 glfw_initialized = glfwInit();
    assertmsg(glfw_initialized, "Failed to initialize GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(DEFAULT_SCREENSIZE, DEFAULT_SCREENSIZE, "ssSSssSSnake", NULL, NULL);
    assertmsg(window, "Failed to create window");
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    g_linux_state.window = window;
    g_linux_state.screenwidth = DEFAULT_SCREENSIZE;
    g_linux_state.screenheight = DEFAULT_SCREENSIZE;
}

extern  entire_file_t read_entire_file(const char *filename) {
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

extern void free_file(entire_file_t file) {
    assert(file.data);
    free(file.data);
}

static game_memory_t create_game_memory() {
    game_memory_t result = {};
    result.permanent_size = (size_t)(1<<16); // 64 KiB
    result.permanent = mmap(NULL, result.permanent_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    assertmsg(result.permanent, "Failed to allocate permanent memory of %lu bytes", result.permanent_size);
    result.renderer_size = (size_t)(1<<20); // 1 MiB
    result.renderer = mmap(NULL, result.renderer_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    assertmsg(result.renderer, "Failed to allocate renderer memory of %lu bytes", result.renderer_size);
    result.scratch_size = (size_t)(1<<20); // 1 MiB
    result.scratch = mmap(NULL, result.scratch_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    assertmsg(result.scratch, "Failed to allocate scratch memory of %lu bytes", result.scratch_size);
    return result;
}

int main(void) {
    initialize_window_and_opengl();
    game_memory_t memory = create_game_memory();

    initialize_watch_directory(&g_linux_state);
    initialize_gamelib(&g_linux_state);
    r32 time_per_checkfile = 2.f;
    r32 checkfile_timer = 0.f;

    // BEGIN MAIN LOOP
    r32 target_frametime = 0.01666667f;
    r32 last_frame_time = glfwGetTime();
    while (!glfwWindowShouldClose(g_linux_state.window)) {
        r32 frametime = glfwGetTime() - last_frame_time;
        glfwPollEvents();

        // CHECK HOTRELOAD
        checkfile_timer += frametime;
        if (checkfile_timer > time_per_checkfile) {
            checkfile_timer = 0.f;
            if (watchfile_modified(g_linux_state.file_descriptor)) {
                reload_gamelib(&g_linux_state);
            }
        }

        // FIXED UPDATE LOOP
        if (frametime >= target_frametime) {
            g_linux_state.input.frametime = frametime;

            render_command_group_t commands = {};
            if (g_linux_state.game_update_and_render) {
                g_linux_state.game_update_and_render(&memory, &commands, &g_linux_state.input/*, audio_stuff */);
            }
            if (memory.initialized) {
                opengl_process_render_commands(&commands);
            }

            glfwSwapBuffers(g_linux_state.window);

            for (i32 i = 0; i < BUTTON_MAX_COUNT; ++i) {
                g_linux_state.input.buttons[i].pressed = false;
                g_linux_state.input.buttons[i].released = false;
            }
            g_linux_state.input.resized = false;

            last_frame_time = glfwGetTime();
        }
        else {
            r32 sleeptime = target_frametime - frametime;
            if (sleeptime > 0) {
                usleep((useconds_t)(sleeptime*1e6));
            }
        }
    } // END MAIN LOOP
    return 0;
}
#endif
