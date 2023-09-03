#!/bin/bash

# Builds the library and archives it as target/static/libgifmethat.a

set -xe

GCC_FLAGS="--std=c99 -Wall -Wextra -Wpedantic"

mkdir -p target/static

gcc -c $GCC_FLAGS -o target/gifmethat.o src/gifmethat.c
ar rcs target/static/libgifmethat.a target/gifmethat.o
