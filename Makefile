#!/bin/bash
#
#gcc -g -o $1_tst.exe $1.c ./funcs/*.c -I . -L. -lcfitsio -lm -lcurl
gcc -o $1.exe -O2 $1.c ./funcs/*.c -I . -L. -lcfitsio -lm -lcurl
