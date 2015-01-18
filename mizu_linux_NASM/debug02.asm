;=========================================================================
; Segmentation fault になる
;   file : debug02.asm
;   2000/12/10 Jun Mizutani
;=========================================================================

%include "debug.inc"

section .text
global _start

msg             db      'I could die.', 10
msglen          equ     $ - msg

_start:
REGS
                mov dword[count], msglen
                mov     ecx, msg
CHECK '1'
        .loop:  mov     edx, 1              ; 1文字づつ出力
                mov     eax, 4
                mov     ebx, 1              ; 標準出力
                int     0x80
MEM4  [count]
                inc     ecx                 ; 文字位置更新
                dec     dword[count]
                jne .loop
CHECK '2'
REGS
                mov     eax, [esi]          ; !
CHECK '3'
                mov     eax, 1
                mov     ebx, 0
                int     0x80                ; 終了

section .data

count       dd  0

;=========================================================================
