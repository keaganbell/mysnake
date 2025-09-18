#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DLL_FILENAME "snake.dll"

typedef void pfn_update_and_render(game_memory_t *memory, render_command_group_t *commands, input_state_t *input);

void *platform_alloc(size_t size) {
    return VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

void initialize_watch_directory(platform_state_t *state) {
    UNUSED(state);
}

void initialize_gamelib(platform_state_t *state) {
    HMODULE handle = LoadLibraryA(DLL_FILENAME);
    assertmsg(handle, "failed to load library");
    state->game_update_and_render = (pfn_update_and_render *)GetProcAddress(handle, "game_update_and_render");
}

b32 watchfile_modified(i32 last_write_time_low_bits) {
    b32 result = false;
    FILETIME write_time = {};
    WIN32_FIND_DATA FindData;
    HANDLE FindHandle = FindFirstFileA(DLL_FILENAME, &FindData);
    if (FindHandle != INVALID_HANDLE_VALUE) {
        write_time = FindData.ftLastWriteTime;
        result = write_time.dwLowDateTime != g_write_time_hack_value;
        g_write_time_hack_value = write_time.dwLowDateTime;
        FindClose(FindHandle);
    }
    return result;
}

void reload_gamelib(platform_state_t *state) {
    lerror("Not supported on windows.");
}