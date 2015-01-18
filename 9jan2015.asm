section .text
global start
msg	 db 'Hello World!', 0x0A
msglen	 equ $ - msg

start:
mov ecx, msg
mov edx, msglen
mov eax, 4
mov ebx, 1
int 0x80
mov eax, 1
mov ebx, 0
int 0x80
