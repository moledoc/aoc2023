#!/bin/sh

set -xe

cc d1.c && ./a.out
cc d2.c && ./a.out
cc d3.c && ./a.out
cc d4.c && ./a.out
cc d5.c && ./a.out
cc d6.c -lm && ./a.out
cc d7.c && ./a.out
cc d8.c && ./a.out
cc d9.c && ./a.out