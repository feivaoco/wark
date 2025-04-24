@echo off

if "%1" == "main" (
    cl ^
    /O2 /Iinclude /Isrc /D _WINDOWS /MT ^
    src\main.c ^
    /link /LIBPATH:lib/msvc ^
    raylib.lib user32.lib gdi32.lib winmm.lib msvcrt.lib shell32.lib ^
    /NODEFAULTLIB:libcmt ^
    /out:wark.exe

    del *.obj
)

if "%1" == "wark" (
    cl ^
    /LD /O2 /Iinclude /Isrc /D _WINDOWS /MT ^
    src\wark.c ^
    /link /LIBPATH:lib/msvc ^
    raylib.lib user32.lib gdi32.lib winmm.lib msvcrt.lib shell32.lib ^
    /NODEFAULTLIB:libcmt ^
    /out:wark_module.dll

    del wark.exp
    del wark.lib
    del *.obj
)
