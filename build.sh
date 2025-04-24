#!/bin/bash

if [ "$(uname)" = "Linux" ]; then
    [ "$1" = main ] && gcc \
        -o wark src/main.c \
        -D _LINUX \
        -O2 -Iinclude -Isrc -Llib/linux \
        -ldl -lraylib -lm \
        -g -fPIC -std=c11 \
        -Wall -Wpedantic -Werror -Wl,-z,origin,--no-undefined

    [ "$1" = wark ] && gcc \
        -shared -o wark_module.so src/wark.c \
        -D _LINUX \
        -O2 -Iinclude -Isrc -Llib/linux \
        -ldl -lraylib -lm \
        -g -fPIC -std=c11 \
        -Wall -Wpedantic -Werror -Wl,-z,origin,--no-undefined
elif [ "$(uname)" = "Windows_NT" ]; then
    [ "$1" = main ] && gcc \
        -o wark.exe src/main.c \
        -D _WINDOWS \
        -O2 -Iinclude -Isrc -Llib/mingw \
        -lraylib -lm -lopengl32 -lgdi32 -lwinmm -lshell32 -luser32 \
        -g -fPIC -std=c11 \
        -Wall -Wpedantic -Werror -Wl,--no-undefined

    [ "$1" = wark ] && gcc \
        -shared -o wark_module.dll src/wark.c \
        -D _WINDOWS \
        -O2 -Iinclude -Isrc -Llib/mingw \
        -lraylib -lm -lopengl32 -lgdi32 -lwinmm -lshell32 -luser32 \
        -g -fPIC -std=c11 \
        -Wall -Wpedantic -Werror -Wl,--no-undefined
else
    echo "Sistema desconocido: $(uname)"
fi
