#!/bin/sh
nasm -f elf call.asm
ld --dynamic-linker /lib/ld-linux.so.2 -lc call.o

