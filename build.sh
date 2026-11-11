#!/usr/bin/bash

set -x

time clang++ main.cpp \
 -o test.o \
 -O0 \
 -include ./headers/pch.hpp \
 -lsfml-system -lsfml-window -lsfml-graphics \
 -lsfml-audio \
 -Wall -Wextra -pedantic \
 $@
