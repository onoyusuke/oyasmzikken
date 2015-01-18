section .data
MOJIRETU db "hello!",0

section .text
extern puts
global _start
_start:

push MOJIRETU
call puts
add esp, 4
mov eax, 1
mov ebx, 0
int 0x80

