;=========================================================================
; Segmentation fault になる
;   file : debug01.asm
;   2000/12/10 Jun Mizutani
;=========================================================================
section .text
global _start

msg             db      'I could die.', 10
msglen          equ     $ - msg

_start:
                mov dword[count], msglen
                mov     ecx, msg
        .loop:  mov     edx, 1              ; 1文字づつ出力
                mov     eax, 4
                mov     ebx, 1              ; 標準出力
                int     0x80
                inc     ecx                 ; 文字位置更新
                dec     dword[count]
                jne .loop
                mov     eax, [esi]          ; !
                mov     eax, 1
                mov     ebx, 0
                int     0x80                ; 終了

section .data

count       dd  0

;=========================================================================
