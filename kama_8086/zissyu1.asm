%include "debug.inc"

section .bss
msg: resb 1; 領域を確保

section .text
global _start


_start:

mov al, 0xff

REGS; debug

mov [msg], al

MEM1 [msg]; debug

mov eax,1
mov ebx,0
int 0x80


