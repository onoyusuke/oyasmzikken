	org 100h
section .text

start:
	mov ah,02h
	mov dl,41h
	int 21h
	mov ah,4ch
	mov al,00h
	int 21h

