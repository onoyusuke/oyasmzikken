	org 100h
section .text

start:
	mov bl,30h
loop:
	mov ah,02h
	mov dl,bl
	int 21h
	mov ah,06h
	mov dl,0ffh
	int 21h
	jnz end
	mov ah,02h
	mov dl,08h
	int 21h
	inc bl
	cmp bl,39h
	ja start
	jmp loop
end:
	int 20
