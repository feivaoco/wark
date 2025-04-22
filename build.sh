#!/bin/bash

[ "$1" = main ] && gcc \
    -o wark main.c \
    -D LINUX_BUILD\
    -O2 -Iinclude -Llib  -ldl -lraylib -lm \
    -g -fPIC -std=c2x \
    -Wall -Wpedantic -Werror -Wl,-z,origin,--no-undefined

[ "$1" = wark ] && gcc \
    -shared -o wark_module.so wark.c \
    -O2 -Iinclude -Llib  -ldl -lraylib -lm \
    -g -fPIC -std=c2x \
    -Wall -Wpedantic -Werror -Wl,-z,origin,--no-undefined
