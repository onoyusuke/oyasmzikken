%include "syscall.inc" 

;------------------------------------
Exit:
               mov    eax, 1         ; sys_exit
               mov    ebx, 0         ; exit with code 0
               int    0x80
;------------------------------------
; exit with ebx
ExitN:
               mov    ebx, eax       ; exit with code ebx
               mov    eax, 1         ; sys_exit
               int    0x80
;------------------------------------
; print string to stdout
; eax : top address
; edx : no of put char
OutString:
               pusha
               mov    ecx, eax
               mov    eax, SYS_write
               mov    ebx, 1         ; to stdout
               int    0x80
               popa
               ret
;------------------------------------
; get length of asciiz string
; eax : top address
; eax : return length
StrLen:
               push   ecx
               push   edi
               mov    edi, eax
               push   eax
               xor    eax, eax
               mov    ecx, 0xFFFF  ; no more than 65k chars.
         repne scasb
               pop    ecx
               sub    edi, ecx
               mov    eax, edi
               pop    edi
               pop    ecx
               ret
;------------------------------------
; print asciiz string
; eax : pointer to string 
OutAsciiZ:
               push   edx
               push   eax
               call   StrLen
               mov    edx, eax
               pop    eax
               call   OutString
               pop    edx
               ret
; パスカルタイプの文字列を表示するサブルーチンも用意しておきます． 文字列の先頭に 1 バイトを文字数として持ち，その後ろに文字列が続きます． 文字数を 1 バイトで持つため最大 255 文字に制限されます．
;------------------------------------
; print pascal string to stdout
; ebx : top address
OutPString:
                push   eax
                push   ebx
                push   edx
                xor    edx, edx
                mov    dl, [ebx]
                inc    ebx
                mov    eax, ebx
                call   OutString
                pop    edx
                pop    ebx
                pop    eax
                ret

; 次に１文字(１バイト)を標準出力に書き出すサブルーチンです．write システム は文字列の格納されたバッファアドレスを渡す必要があるため，スタック上に バッファを確保(4バイト)しています．実行前後でレジスタは保存されます．

;------------------------------------
; print 1 character to stdout
; eax : put char
OutChar:
               pusha
               push   eax            ; work buffer on stack
               mov    eax, SYS_write
               mov    ebx, 1         ; to stdout
               mov    edx, 1         ; 1 char
               mov    ecx, esp
               int    0x80
               pop    eax
               popa
               ret

; EAX の4バイトを文字列として出力します．下位が先に表示されます． また 8ビット目は常に0に設定します．メモリダンプ用です．

;------------------------------------
; print 4 characters in eax to stdout
;   destroyed : eax
OutChar4:
               push   ecx
               mov    ecx, 0x0408
.loop:
               and    al, 0x7F       ; 7bit only
               cmp    al, 0x7F
               jz     .dot
               cmp    al, 0x20
               jae    .output
.dot           mov    al, '.'
.output        call   OutChar
               shr    eax, cl
               dec    ch
               jnz    .loop
               pop    ecx
               ret

; OutChar で改行コード (0x0A) を出力すれば改行できますが，頻繁に使うため 改行を出力する専用のサブルーチンも用意しておきます． レジスタの値は変化しません．

;------------------------------------
; new line
;   all registers are preserved.
NewLine:
               push   eax
               mov    al, 0AH
               call   OutChar
               pop    eax
               ret

; カーソル直前の文字を消す必要もあるでしょう．これも専用のサブルーチンを 用意します．バックスペースは1文字左に移動してスペースを書き出し， もう一度 1文字左に移動する必要があります．

;------------------------------------
; Backspace
;   destroyed : al
BackSpace:
               mov    al, 0x08
               call   OutChar
               mov    al, ' '
               call   OutChar
               mov    al, 0x08
               call   OutChar
               ret

; 次は数値の出力です．まず簡単な 16進数の出力です． EAX の内容を16進数で標準出力に書き出します．表示桁数は EAXの下位優先で PrintHex2 は 2桁,PrintHex4 は 4桁, PrintHex8 は 8桁で表示します．

;------------------------------------
; print 2 digit hex number (lower 8 bit of eax)
;   eax : number
;   destroyed : edx
PrintHex2:
               mov    edx, 2
               jmp    short PrintHex

;------------------------------------
; print 4 digit hex number (lower 16 bit of eax)
;   eax : number
;   destroyed : edx
;
PrintHex4:
               mov    edx, 4
               jmp    short PrintHex

;------------------------------------
; print 8 digit hex number (eax)
;   eax : number
;   destroyed : edx
;
PrintHex8:
               mov    edx, 8

;------------------------------------
; print hex number
;   eax : number     edx : digit
;
PrintHex:
               push   eax
               push   ecx
               push   ebx
               mov    ecx, edx
  .loop1:      mov    bl, al
               and    bl, 0x0F
               shr    eax, 4
               or     bl, 0x30
               cmp    bl, 0x3A
               jb     .skip
               add    bl, 0x41 - 0x3A   ; A-F
  .skip:
               push   ebx
               loop   .loop1
               mov    ecx, edx
  .loop2:      pop    eax
               call   OutChar
               loop   .loop2
               pop    ebx
               pop    ecx
               pop    eax
               ret

; 10進数の出力では，数値を数字に変換する必要があります．ここでは 上位の桁から順に表示します． PrintLeftは EAX の内容を符号付十進数値として左詰めで標準出力に書き出します． PrintLeftU は符号なし十進数値として左詰めで出力します．

;------------------------------------
; Output Unsigned Number to stdout
; eax : number
PrintLeftU:
                pusha
                xor     ecx, ecx            ; 桁数カウンタ
                xor     edi, edi            ; 正を仮定
                jmp     short PrintLeft.positive

;------------------------------------
; Output Number to stdout
; eax : number
PrintLeft:
                pusha
                xor     ecx, ecx            ; 桁数カウンタ
                xor     edi, edi            ; 正を仮定
                test    eax, eax
                jns     .positive
                inc     edi                 ; 負に設定
                neg     eax
    .positive:  mov     ebx, 10
    .PL1:       xor     edx, edx            ; 上位桁を 0 に
                div     ebx                 ; 10 で除算
                push    edx                 ; 剰余(下位桁)をPUSH
                inc     ecx                 ; 桁数更新
                test    eax, eax            ; 終了か?
                jnz     .PL1
    .PL2:       test    edi, edi
                je      .pos
                mov     al, '-'             ; 文字コードに変更
                call    OutChar             ; 出力
    .pos:       pop     eax                 ; 上位桁から POP
                add     al, '0'             ; 文字コードに変更
                call    OutChar             ; 出力
                loop    .pos
                popa
                ret

; PrintRight は EAX の内容を符号つき十進数値として空白を補って右詰めで標準出力に 書き出します．ECX に桁を指定します．PrintRightU は符号なし数値を出力， PrintRight0 は符号なしで前に0を補って数値を出力します．

;------------------------------------
; Output Number to stdout
; ecx:column
; eax:number
PrintRight0:
                pusha
                mov     ebp, '0'
                jmp     short PrintRightU.pr0

;------------------------------------
; Output Unsigned Number to stdout
; ecx:column
; eax:number
PrintRightU:
                pusha
                mov     ebp, ' '
    .pr0:       mov     esi, ecx            ; 表示桁数を esi に
                xor     ecx, ecx            ; 桁数カウンタ
                xor     edi, edi            ; 正を仮定
                jmp     short PrintRight.positive

;------------------------------------
; Output Number to stdout
; ecx:column
; eax:number
PrintRight:
                pusha
                mov     ebp, ' '
    .pr0:       mov     esi, ecx            ; 表示桁数を esi に
                xor     ecx, ecx            ; 桁数カウンタ
                xor     edi, edi            ; 正を仮定
                test    eax, eax
                jns     .positive
                dec     esi
                inc     edi                 ; 負を設定
                neg     eax
    .positive:
                mov     ebx, 10
    .pr1:       xor     edx, edx            ; 上位桁を 0 に
                div     ebx                 ; 10 で除算
                push    edx                 ; 剰余(下位桁)をPUSH
                inc     ecx                 ; 桁数更新
                test    eax, eax            ; 終了か?
                jnz     .pr1
                sub     esi, ecx            ; esi にスペース数
                jbe     .done               ; 表示桁数を超える
                xchg    esi, ecx            ; ecx にスペース数
    .space:     mov     eax, ebp            ; スペースか 0
                call    OutChar             ; スペース出力
                loop    .space
                xchg    esi, ecx            ; ecx に表示桁数
    .done:
                jmp     short PrintLeft.PL2

; 1文字を標準入力から読みこみます．読んだ文字は EAX レジスタに格納されます． 入力バッファはスタック上に4バイト確保 (push eax) して, 結果を EAX に格納 (pop eax) しています．当然 AL の１バイトのみが有効です．

;------------------------------------
; input 1 character from stdin
; eax : get char
InChar:
               push   ebx
               push   ecx
               push   edx
               push   eax            ; work buffer on stack
               mov    eax, SYS_read
               mov    ebx, 0         ; from stdin
               mov    ecx, esp       ; into Input Buffer
               mov    edx, 1         ; 1 char
               int    0x80           ; call kernel
               pop    eax
               pop    edx
               pop    ecx
               pop    ebx
               ret

; EAX に指定した文字数(バイト数)を標準入力から読みこみます． キーボードからの入力も標準入力となり，編集機能が全く無いと 実用的ではありません．ここでは１文字消去 (バックスペース) の の機能のみを実装しておきます．任意の位置への挿入などは後で 作成することにします．

;------------------------------------
; Input Line
; eax : BufferSize
; ebx : Buffer Address
; return       eax : no. of char
;
InputLine0:
               push   edi
               push   ecx
               push   edx
               mov    edx, eax
               mov    edi, ebx       ; Input Buffer
               xor    ecx, ecx
   .in_char:
               call   InChar
               cmp    al, 0x08       ; BS ?
               jnz    .in_char2
               test   ecx, ecx
               jz     .in_char2
               call   BackSpace      ; backspace
               dec    ecx
               jmp    short .in_char
   .in_char2:
               cmp    al, 0x0A       ; enter ?
               jz     .in_exit

   .in_printable:
               call   OutChar
               mov    [edi + ecx], al
               inc    ecx
               cmp    ecx, edx       ;
               jae    .in_toolong
               jmp    short .in_char
   .in_toolong:
               dec    ecx
               call   BackSpace
               jmp    short .in_char

   .in_exit:
               mov    dword [edi + ecx], 0
               inc    ecx
               call   NewLine
               mov    eax, ecx
               pop    ecx
               pop    edi
               ret

