section .text
global _start
msg db 'Hello', 0x0A
msglen equ $ - msg
_start:
mov ecx, msg ;文字列のアドレス
mov edx, msglen ;文字列長
mov eax, 4 ;出力のシステムコール
mov ebx, 1 ;標準出力を指定
int 0x80 ;システムコール実行
mov eax, 1 ;終了のシステムコール
mov ebx, 0 ;正常終了
int 0x80 ;システムコール実行



