#!/bin/bash

# builds and runs the test application at test/main.c that decodes cats.gif into cats.ppm

set -xe

gcc -o target/test/main -lm src/gifmethat.c test/main.c
./target/test/main
