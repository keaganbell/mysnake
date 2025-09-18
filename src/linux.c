#pragma once
#include <sys/inotify.h> // inotify
#include <sys/mman.h> // mmap
#include <unistd.h> // read
#include <dlfcn.h> // dlopen, dlclose, dlsym
#include <fcntl.h> // fcntl

typedef struct timespec Clock;
typedef struct inotify_event iNotifyEvent;

#define DLL_FILENAME "snake.so"

static void initialize_watch_directory(linux_state_t *state) {
    i32 fd = inotify_init();
    assertmsg(fd, "Failed to get file descriptor");
    state->file_descriptor = fd;
    i32 flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    i32 wd = inotify_add_watch(fd, "./", IN_CLOSE_WRITE);
    assertmsg(wd, "Failed to get watch descriptor");
    state->watch_descriptor = wd;
}

static void initialize_gamelib(linux_state_t *state) {
    state->gamelib = dlopen("./"DLL_FILENAME, RTLD_LAZY);
    assertmsg(state->gamelib, "Couldn't load application library");
    state->game_update_and_render = dlsym(state->gamelib, "game_update_and_render");
    assertmsg(state->game_update_and_render, "Couldn't load application update function");
}

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))
static b32 watchfile_modified(i32 fd) {
    char msgbuffer[BUF_LEN];
    i32 msglength = read(fd, msgbuffer, BUF_LEN);
    if (msglength > 0) {
        i32 msgindex = 0;
        while (msgindex < msglength) {
            iNotifyEvent *event = (iNotifyEvent *)&msgbuffer[msgindex];
            if (event->mask & IN_CLOSE_WRITE && strcmp(event->name, DLL_FILENAME) == 0) {
                linfo("%s IN_CLOSE_WRITE event", event->name);
                return true;
            }
            msgindex += EVENT_SIZE + event->len;
        }
    }
    return false;
}

static void reload_gamelib(linux_state_t *state) {
    dlclose(state->gamelib);
    state->gamelib = dlopen("./"DLL_FILENAME, RTLD_LAZY);
    state->game_update_and_render = dlsym(state->gamelib, "game_update_and_render");
}

void *platform_alloc(size_t size) {
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}