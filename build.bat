@echo off

setlocal

set name=snake
set incs=-I%~dp0ext
set libs=user32.lib gdi32.lib opengl32.lib glfw3.lib shell32.lib

if not exist build (mkdir build)
pushd build
if not exist ass (mkdir ass)
if not exist ass\shaders (mkdir ass\shaders)


set shaderpat=..\ass\shaders\*.glsl
for %%g in (%shaderpat%) do (
	copy %%g ass\shaders
)

copy ..\ext\glfw3.lib .

where /q cl (
    cl -FC -W4 -Zi -Od -MD -nologo -DSNAKE_ENGINE %incs% ..\src\main.c -Fd%name%.pdb -Fe%name%.exe -link -INCREMENTAL:NO %libs%
    cl -FC -W4 -Zi -Od -MT -nologo %incs% ..\src\snake.c -Fdcompiler_dll_%name%.pdb -LD -link -OUT:%name%.dll -INCREMENTAL:NO %libs% /EXPORT:game_update_and_render -PDB:linker_dll_%name%.pdb
)
popd

endlocal