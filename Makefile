#!/bin/bash
#
gcc -o $1.exe -O2 $1.c ./funcs/*.c -I . -L. -lcfitsio -lm -lcurl
