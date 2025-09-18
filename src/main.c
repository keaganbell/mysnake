#include <GLFW/glfw3.h>
#include <stdlib.h> // malloc (loading files)

#include "defines.h"
#include "log.h"
#include "memory.h"
#include "render.h"
//#include "opengl.h"
#include "snake.h"
#include "platform.h"
#include "input.h"

static i32 g_write_time_hack_value;

typedef struct platform_state {
    GLFWwindow *window;
    i32 screenwidth;
    i32 screenheight;
    i32 file_descriptor;
    i32 watch_descriptor;
    input_state_t input;
    void *gamelib;
    void (*game_update_and_render)(game_memory_t *memory, render_command_group_t *render_commands, input_state_t *input);
} platform_state_t;
static platform_state_t g_platform_state;
#include "glfw_input.c"
//#include "opengl.c"

#if defined(_WIN64)
#include "windows.c"
#else
#include "linux.c"
#endif

static void initialize_window_and_opengl() {
    b32 glfw_initialized = glfwInit();
    assertmsg(glfw_initialized, "Failed to initialize GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(DEFAULT_SCREENSIZE, DEFAULT_SCREENSIZE, "ssSSssSSnake", NULL, NULL);
    assertmsg(window, "Failed to create window");
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    g_platform_state.window = window;
    g_platform_state.screenwidth = DEFAULT_SCREENSIZE;
    g_platform_state.screenheight = DEFAULT_SCREENSIZE;
}

static game_memory_t create_game_memory() {
    game_memory_t result = {};
    result.permanent_size = (size_t)(1<<16); // 64 KiB
    result.permanent = platform_alloc(result.permanent_size);
    assertmsg(result.permanent, "Failed to allocate permanent memory of %zu bytes", result.permanent_size);
    result.renderer_size = (size_t)(1<<20); // 1 MiB
    result.renderer = platform_alloc(result.renderer_size);
    assertmsg(result.renderer, "Failed to allocate renderer memory of %zu bytes", result.renderer_size);
    result.scratch_size = (size_t)(1<<20); // 1 MiB
    result.scratch = platform_alloc(result.scratch_size);
    assertmsg(result.scratch, "Failed to allocate scratch memory of %zu bytes", result.scratch_size);
    return result;
}

int main(void) {
    initialize_window_and_opengl();
    game_memory_t memory = create_game_memory();

    initialize_watch_directory(&g_platform_state);
    initialize_gamelib(&g_platform_state);
    r32 time_per_checkfile = 2.f;
    r32 checkfile_timer = 0.f;

    // BEGIN MAIN LOOP
    r32 target_frametime = 0.01666667f;
    r32 last_frame_time = glfwGetTime();
    while (!glfwWindowShouldClose(g_platform_state.window)) {
        r32 frametime = glfwGetTime() - last_frame_time;
        glfwPollEvents();

        // CHECK HOTRELOAD
        checkfile_timer += frametime;
        if (checkfile_timer > time_per_checkfile) {
            checkfile_timer = 0.f;
            if (watchfile_modified(g_platform_state.file_descriptor)) {
                reload_gamelib(&g_platform_state);
            }
        }

        // FIXED UPDATE LOOP
        if (frametime >= target_frametime) {
            g_platform_state.input.frametime = frametime;

            render_command_group_t commands = {};
            if (g_platform_state.game_update_and_render) {
                g_platform_state.game_update_and_render(&memory, &commands, &g_platform_state.input/*, audio_stuff */);
            }

            glfwSwapBuffers(g_platform_state.window);

            for (i32 i = 0; i < BUTTON_MAX_COUNT; ++i) {
                g_platform_state.input.buttons[i].pressed = false;
                g_platform_state.input.buttons[i].released = false;
            }
            g_platform_state.input.resized = false;

            last_frame_time = glfwGetTime();
        }
        else {
            r32 sleeptime = target_frametime - frametime;
            if (sleeptime > 0) {
                glfwWaitEventsTimeout(sleeptime);
            }
        }
    } // END MAIN LOOP
    return 0;
}
