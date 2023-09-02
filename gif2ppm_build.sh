#!/bin/bash

set -xe

gcc -o target/gif2ppm -lm src/gifmethat.c example/gif2ppm.c
