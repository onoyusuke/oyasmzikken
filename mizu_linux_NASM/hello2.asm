  ;------------------------------------
  ; hello2.asm
  ;------------------------------------

  %include "syscall.inc"
  %include "stdio.inc"

  section .text
  global _start

  msg             db   'hello, world', 0x0A, 0

  _start:
                 mov    eax, msg
                 call   OutAsciiZ
                 call   Exit
