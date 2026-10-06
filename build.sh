#!/usr/bin/bash

set -x

time clang++ main.cpp\
 -Wall -Wextra -pedantic\
 -o test.o \
 -include ./headers/pch.hpp\
 -lsfml-system -lsfml-window -lsfml-graphics\
 -lsfml-audio


