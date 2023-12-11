#!/bin/sh

set -xe

time cc d01.c && ./a.out
time cc d02.c && ./a.out
time cc d03.c && ./a.out
time cc d04.c && ./a.out
time cc d05.c && ./a.out
time cc d06.c -lm && ./a.out
time cc d07.c && ./a.out
time cc d08.c && ./a.out
time cc d09.c && ./a.out
time cc d10.c && ./a.out