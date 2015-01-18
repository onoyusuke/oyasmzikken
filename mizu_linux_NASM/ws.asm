;---------------------------------------------------------------------
  ;   Display current window size
  ;   2000/01/16 Jun Mizutani
  ;     nasm -f elf ws.asm
  ;     ld -s -o ws ws.o
  ;     ndisasm -b 32 ws
  ;   usage : ws
  ;---------------------------------------------------------------------

  section .bss

  struc winsize
          .ws_row     resw 1
          .ws_col     resw 1
          .ws_xpixel  resw 1
          .ws_ypixel  resw 1
  endstruc

  wsize istruc winsize
          .ws_row     resw 1
          .ws_col     resw 1
          .ws_xpixel  resw 1
          .ws_ypixel  resw 1
        iend

  section .text
  global _start

  %assign TIOCGWINSZ      0x5413

  ;------------------------------------
  ; start here
  ;------------------------------------
  _start:
                 mov    eax, 54         ; sys_ioctl
                 mov    ebx, 0          ; to stdout
                 mov    ecx, TIOCGWINSZ ; get wondow size
                 mov    edx, wsize
                 int    0x80            ; call kernel
                 xor    eax, eax
                 mov    ax, [edx + winsize.ws_col]
                 call   PRINT_LEFT
                 mov    eax, "x"
                 call   OUTCHAR
                 xor    eax, eax
                 mov    ax, [edx + winsize.ws_row]
                 call   PRINT_LEFT
                 call   CRLF
                 mov    eax, 1          ; sys_call 1 : exit
                 mov    ebx, 0          ; exit with code 0
                 int    0x80
                 hlt

  ;------------------------------------
  ; Output Number to stdout
  ; eax : number
  ;------------------------------------
  PRINT_LEFT:
                 pusha
                 test   eax, eax
                 jns    .PL0
                 neg    eax            ; negative number
                 push   eax
                 mov    al, '-'
                 call   OUTCHAR
                 pop    eax
     .PL0:
                 mov    ecx, 8
                 mov    edi, 10
                 mov    esi, 0
                 mov    ebx, 100000000
     .PL1:
                 call   PLSUB
                 xchg   eax, ebx       ;  : save eax
                 xor    edx, edx       ; ebx = ebx / 10
                 div    edi            ;  :
                 xchg   eax, ebx       ;  : restore eax
                 loop   .PL1

                 add    al, '0'
                 call   OUTCHAR
                 popa
                 ret

     ;--------------------------------
     ; Print Largest 1 digit
     ;--------------------------------
     PLSUB:
                 xor    edx, edx       ; edx = 0
                 div    ebx            ; eax = edx:eax / ebx
                 push   edx            ; modulo
                 test   eax, eax
                 jz     .PS1           ; if eax = 0 then just return.
                 add    al, '0'
                 call   OUTCHAR        ; print 1 digit
                 inc    esi            ;
                 jmp    short  .PS2
     .PS1:
                 test   esi, esi       ; suppress leading 0
                 jz     .PS2
                 mov    al, '0'
                 call   OUTCHAR
     .PS2:
                 pop    eax            ; modulo
                 ret

  CRLF:
                 mov    al, 0AH
  OUTCHAR:
                 pusha
                 push   eax            ; work buffer on stack
                 mov    eax, 4         ; sys_write
                 mov    ebx, 1         ; to stdout
                 mov    edx, 1         ; 1 char
                 mov    ecx, esp
                 int    0x80
                 pop    eax
                 popa
                 ret

