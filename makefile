projroot:=$(shell pwd)
inc:=-Isrc/
cflags:=-Wall -Wextra -Wno-missing-braces -Wno-unused-function
lib:=-lGL -lglfw

cflags +=-g -ggdb

.PHONY: all
all: engine game

.PHONY: engine
engine:
	gcc -o snake src/linux_platform.c $(cflags) -DPLATFORM_LINUX -DSNAKE_ENGINE $(inc) -rdynamic $(lib) -ldl

.PHONY: game
game:
	gcc $(cflags) $(inc) -shared -o snake.so src/snake.c
