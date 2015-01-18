section .data
msg: db "Hello World", 10
len: equ $-msg

section .text
global _main
_main:
push dword len
lea eax, [msg]
push dword eax
push dword 1

mov eax, 4
push dword eax
int 0x80

mov eax, 1
int 0x80
