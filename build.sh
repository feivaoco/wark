#!/bin/bash

[ "$1" = main ] && gcc \
    -o wark main.c \
    -O2 -Iinclude -Llib  -ldl -lraylib -lm \
    -g -fPIC -std=c2x \
    -Wall -Wpedantic -Werror -Wl,-z,origin,--no-undefined

[ "$1" = wark ] && gcc \
    -shared -o wark_module.so wark.c \
    -O2 -Iinclude -Llib  -ldl -lraylib -lm \
    -g -fPIC -std=c2x \
    -Wall -Wpedantic -Werror -Wl,-z,origin,--no-undefined

#wark_dll
#gcc -o wark main.c -Iinclude -Llib  -ldl -g -fPIC -std=c2x -Wall -Wpedantic -Werror -Wl,-z,origin,--no-undefined

#gcc -o bol main.c -Iinclude -Llib/linux -lraylib -lm -std=c2x -Wall -Wpedantic -Werror -Wl,-z,origin,--no-undefined