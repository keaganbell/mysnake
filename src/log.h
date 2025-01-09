#pragma once

#include <stdio.h> // printf
#include <string.h> 
#include <assert.h>

#define assertmsg(eval, msg, ...) if (!eval) { lfatal(msg, ##__VA_ARGS__); *(u8 *)NULL = 1; }

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define lfatal(message, ...) printf("FATAL: %s:%u: "message"\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
#define lerror(message, ...) printf("error: %s:%u: "message"\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
#define lwarn(message, ...)  printf("warn:  %s:%u: "message"\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
#define linfo(message, ...)  printf("info:  %s:%u: "message"\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
#define ldebug(message, ...) printf("debug: %s:%u: "message"\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
#define ltrace(message, ...) printf("trace: %s:%u: "message"\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
