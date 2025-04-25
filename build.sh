#!/bin/bash

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
