#pragma once

// FIXME: This whole input system could use some tlc
static inline void set_button_state(buttonkeys_t key, int action) {
    switch (action) {
        case GLFW_PRESS: {
            g_linux_state.input.buttons[key].down = true;
            g_linux_state.input.buttons[key].pressed = true;
            g_linux_state.input.buttons[key].released = false;
            g_linux_state.input.buttons[key].up = false;
        } break;

        case GLFW_RELEASE: {
            g_linux_state.input.buttons[key].down = false;
            g_linux_state.input.buttons[key].pressed = false;
            g_linux_state.input.buttons[key].released = true;
            g_linux_state.input.buttons[key].up = true;
        } break;

        case GLFW_REPEAT: {
        } break;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window;
    (void)scancode;
    (void)mods;
    // FIXME: just doing what necessary first
    switch (key) {
        case GLFW_KEY_ESCAPE: {
            set_button_state(KEY_ESCAPE, action);
        } break;

        case GLFW_KEY_SPACE: {
            set_button_state(KEY_SPACE, action);
        } break;

        case GLFW_KEY_ENTER: {
            set_button_state(KEY_ENTER, action);
        } break;

        case GLFW_KEY_LEFT: {
            set_button_state(KEY_LEFT, action);
        } break;

        case GLFW_KEY_RIGHT: {
            set_button_state(KEY_RIGHT, action);
        } break;

        case GLFW_KEY_UP: {
            set_button_state(KEY_UP, action);
        } break;

        case GLFW_KEY_DOWN: {
            set_button_state(KEY_DOWN, action);
        } break;

        case GLFW_MOUSE_BUTTON_LEFT: {
            set_button_state(MOUSE_BUTTON_LEFT, action);
        } break;

        case GLFW_MOUSE_BUTTON_RIGHT: {
            set_button_state(MOUSE_BUTTON_RIGHT, action);
        } break;

        case GLFW_MOUSE_BUTTON_MIDDLE: {
            set_button_state(MOUSE_BUTTON_MIDDLE, action);
        } break;

        default:
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    g_linux_state.input.mousex = xpos;
    g_linux_state.input.mousey = ypos;
}

// glfw callback
void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height) {
    (void)window;
    g_linux_state.input.screenwidth = width;
    g_linux_state.input.screenheight = height;
    g_linux_state.input.resized = true;
}
