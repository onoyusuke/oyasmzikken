;=========================================================================
; DEBUG
; file : debug.inc
; 2000/12/09
;         Copyright (C) 2000 Jun Mizutani <mizutani.jun@nifty.ne.jp>
;=========================================================================
%ifndef __DEBUG_INC
%define __DEBUG_INC

%include "stdio.inc"

;=========================================================================
;-------------------------------------------------------------------------
; 文字列表示
;   例.  CHECK 'Waw!'  ; 4文字まで
;-------------------------------------------------------------------------
%macro CHECK 1
                push    eax
                mov     eax, %1
                call    check_point
                pop     eax
%endmacro

;-------------------------------------------------------------------------
; Register 表示
;   例.  REGS
;-------------------------------------------------------------------------
%macro REGS  0
                call    print_regs
%endmacro

;-------------------------------------------------------------------------
; 指定したメモリの内容を16進数と10進数で表示
;   例.  MEM1    [work]  ; work の1バイトの内容を表示
;        MEM2    [work]  ; work の2バイトの内容を表示
;        MEM4    [work]  ; work の4バイトの内容を表示
;-------------------------------------------------------------------------
%macro MEM1  1
                pushfd
                push    eax
                push    ebx
                xor     eax, eax
                mov     al, %1
                lea     ebx, %1
                call    print_mem1
                pop     ebx
                pop     eax
                popfd
%endmacro

%macro MEM2  1
                pushfd
                push    eax
                push    ebx
                xor     eax, eax
                mov     ax, %1
                lea     ebx, %1
                call    print_mem2
                pop     ebx
                pop     eax
                popfd
%endmacro

%macro MEM4  1
                pushfd
                push    eax
                push    ebx
                mov     eax, %1
                lea     ebx, %1
                call    print_mem4
                pop     ebx
                pop     eax
                popfd
%endmacro

;=========================================================================
section .text

;-------------------------------------------------------------------------
; 文字列表示
; ex.  check 'Waw!'  4文字まで
;-------------------------------------------------------------------------
check_point:
                pushfd
                push    edx
                push    eax
                mov     eax, CheckMsg
                call    OutAsciiZ
                pop     eax
                call    OutChar4
                mov     al, ')'
                call    OutChar
                call    NewLine
                pop     edx
                popfd
                ret

;-------------------------------------------------------------------------
; Register 表示
;-------------------------------------------------------------------------
print_regs:
                pushfd
                push    edx
                push    eax
                call    NewLine
                push    edx                     ; EDX を保存
                push    eax
                mov     eax, 'EAX:'             ; EAX レジスタ表示
                call    OutChar4
                pop     eax
                call    PrintHex8
                call    .PrintDecimal

                mov     eax, 'EBX:'             ; EBX レジスタ表示
                call    OutChar4
                mov     eax, ebx
                call    PrintHex8
                call    .PrintDecimal

                mov     eax, 'ECX:'             ; ECX レジスタ表示
                call    OutChar4
                mov     eax, ecx
                call    PrintHex8
                call    .PrintDecimal

                mov     eax, 'EDX:'             ; EDX レジスタ表示
                call    OutChar4
                pop     eax                     ; 保存していたEDX
                call    PrintHex8
                call    .PrintDecimal

                mov     eax, 'EDI:'             ; EDI レジスタ表示
                call    OutChar4
                mov     eax, edi
                call    PrintHex8
                call    .PrintDecimal

                mov     eax, 'ESI:'             ; ESI レジスタ表示
                call    OutChar4
                mov     eax, esi
                call    PrintHex8
                call    .PrintDecimal

                mov     eax, 'EBP:'             ; EBP レジスタ表示
                call    OutChar4
                mov     eax, ebp
                call    PrintHex8
                call    .PrintDecimal

                mov     eax, 'ESP:'             ; ESP レジスタ表示
                call    OutChar4
                mov     eax, esp
                call    PrintHex8
                call    .PrintDecimal
                pop     eax
                pop     edx
                popfd
                ret
.PrintDecimal:
                push    eax
                mov     eax, ' '
                call    OutChar
                pop     eax
                call    PrintLeft
                call    NewLine
                ret

;-------------------------------------------------------------------------
; 指定したメモリの内容を16進数と10進数で表示
;-------------------------------------------------------------------------
print_mem1:
                push    edx
                push    eax
                call    NewLine
                mov     eax, 'MEM:'
                call    OutChar4
                mov     eax, ebx
                call    PrintHex8
                mov     eax, ' '
                call    OutChar
                pop     eax
                call    PrintHex2
                call    print_regs.PrintDecimal
                pop     edx
                ret

print_mem2:
                push    edx
                push    eax
                call    NewLine
                mov     eax, 'MEM:'
                call    OutChar4
                mov     eax, ebx
                call    PrintHex8
                mov     eax, ' '
                call    OutChar
                pop     eax
                call    PrintHex4
                call    print_regs.PrintDecimal
                pop     edx
                ret

print_mem4:
                push    edx
                push    eax
                call    NewLine
                mov     eax, 'MEM:'
                call    OutChar4
                mov     eax, ebx
                call    PrintHex8
                mov     eax, ' '
                call    OutChar
                pop     eax
                call    PrintHex8
                call    print_regs.PrintDecimal
                pop     edx
                ret

;-------------------------------------------------------------------------
; EDI からの10バイト(80bit)のメモリ内容を16進数で表示
; メモリの後部が上位桁
;-------------------------------------------------------------------------
print_mem80:
                pushfd
                push    ecx
                mov     ecx, 10
          .loop:
                mov     al, [edi+ecx-1]
                push    edx
                call    PrintHex2
                pop     edx
                loop    .loop
                pop     ecx
                popfd
                ret

;-------------------------------------------------------------------------
; EDI からの10バイト(80bit)のメモリ内容を16進数で表示
; メモリの前部が上位桁
;-------------------------------------------------------------------------
print_10:
                pushfd
                push    edi
                push    ecx
                mov     ecx, 10
          .loop:
                mov     al, [edi]
                call    PrintHex2
                inc     edi
                loop    .loop
                pop     ecx
                pop     edi
                popfd
                ret

;-------------------------------------------------------------------------
; FPU のスタックトップを16進数で表示
;     1 - 99京 までの整数を表示可能，超えると FFFFC000000000000000 となる
;-------------------------------------------------------------------------
print_top:
                pushfd
                push    edx
                push    eax
                push    edi
                fld     st0   ; DUP
                fstp    tword [BCD0] ; stack topを指定メモリ10byteに格納
                mov     edi, BCD0
                call    print_mem80
                mov     eax, ':'
                call    OutChar
                fld     st0   ; DUP
                fbstp   tword [BCD0] ;stack topをBCD18桁でメモリ10byteに格納
                mov     edi, BCD0
                call    print_mem80
                call    NewLine
                pop     edi
                pop     eax
                pop     edx
                popfd
                ret

;-------------------------------------------------------------------------
; FPU のステイタスワードを４桁の16進数で表示
;-------------------------------------------------------------------------
print_fpu_status:
                pushfd
                push    eax
                fnstsw  ax
                push    edx
                shr     eax, 11
                and     eax, 7
                call    PrintHex4
                call    NewLine
                pop     edx
                pop     eax
                popfd
                ret

;-------------------------------------------------------------------------
CheckMsg           db 10, 'CHECK(', 0

;=========================================================================
section .bss

BCD0            rest    1       ; 仮数値 BCD 表現(10バイト)

%endif

