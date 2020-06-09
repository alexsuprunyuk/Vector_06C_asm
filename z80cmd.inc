;include \masm32\include\debug.inc
;includelib \masm32\lib\debug.lib

;
;процедуры расшифровки комманд процессора Z80
;
SelM1RAMPage PROTO : WORD
SelSteckRAMPage PROTO :WORD,:BYTE

VideoMem MACRO vmem; макрос для помещения байта в видеопамять
LOCAL VideoMemEx
LOCAL VideoMemCom
        movzx eax, vmem
        test ah, 80h
        jz VideoMemEx
        test stack, 1
        jnz VideoMemEx
        test port2[8], 10h
        jz VideoMemCom
        call PixelToMem4
        jmp VideoMemEx
VideoMemCom:
        call PixelToMem16
VideoMemEx:
        ENDM
 
.code
ResetPC PROC; процедура в стадии разработки
    push esi
    xor ax, ax
    mov pc, ax
    mov iif, al
    mov imx, al
    lea esi, port80
    mov [esi],ax
    dec esi
    mov [esi],al;port10
    dec ax
    lea esi, port2
    mov [esi],ax
    inc esi
    inc esi
    mov [esi],ax
    inc esi
    inc esi
    mov [esi],ax
    inc esi
    inc esi
    mov [esi],ax
    mov port1, 0E0h
	mov p9cnt,0
	mov pAcnt,0
	mov pBcnt,0
	mov p9f,0
	mov pAf,0
	mov pBf,0
	mov port8,0
	mov port9,0
	mov portA,0
	mov portB,0
    pop esi
    ret
ResetPC ENDP

SelM1RAMPage PROC pcadr: WORD
; входной параметр адрес операнда, для которого вычисляется индекс источника данных
; входным параметром могут быть значения регистров pc, BC, DE, IX, IY
LOCAL indx: BYTE
LOCAL tmp: BYTE
  mov indx, 4 ;по умолчанию чтение данных из ОЗУ
  mov stack, 0
 .if blk==0
    mov ax, pcadr
    cmp ax, MaxRomAddr
    ja @F
    mov indx, 5 ;чтение из ПЗУ
 .else
@@: mov al, port10
    and al, 20h
    mov tmp, al
   .if tmp == 20h && pcadr > 9FFFh && pcadr < 0E000h
     mov stack, 1
     mov al, port10
     and al, 3
     mov indx, al ;чтение со страницы электронного диска
   .endif
     test port80, 4
     jz @F
     test port10, 10h
     jz @F
;включен режим обмена местами ОЗУ и электронного диска
      mov stack, 1
      shr al, 1
      shr al, 1
      and al, 3
      mov indx, al ; чтение со страницы электронного диска
     @@:
 .endif
  mov al, indx
  ret
; выходной параметр - номер источника данных
SelM1RAMPage ENDP

SelSteckRAMPage PROC radr:WORD, MorSP:BYTE
;входной параметр адрес операнда, для которого вычисляется индекс источника данных
; входным параметром могут быть значения регистров sp, HL
LOCAL indx: BYTE
    mov indx, 4 ;по умолчанию чтение данных из ОЗУ
    mov stack, 0
    mov ax, radr
    cmp ax, MaxRomAddr
    ja @F
    test blk, 1
    jnz @F
    mov indx, 5 ;чтение из ПЗУ
    jmp @SSExit
@@:
    ;---формирование сигнала стек---
    and ah, 0E0h
    cmp ah, 40h
    jne @SSRP1
    test port80, 20h ;блокировка сигнала стек при обращении
    jnz @SSExit ; к адресам 4000-5FFF при включенном бите 5 порта 80h
@SSRP1:
    test port80, 80h
    jnz @SwapRP1  ; если включена блокировка обращения к стеку
    mov al, MorSP
    and al, al; команды с регистром М?
    jnz @F; ;командам PUSH POP разрешены обращения к ЭД
    mov al, port80
    and al, 8 ;команды с регистром M могут обращаться к ЭД?
    shr al, 3
@@: mov stack, al
@SwapRP1:
    mov al, port80
    ror al, 2
    and al, 1
    xor stack, al ;инверсия сигнала стека если необходимо
        ; ---выбор источника данных---
    test stack, 1
    jz @F
    mov al, port10
    mov ah, al
    shr al, 4
    and al, 1
    mov stack, al
    jz @F
    mov al, ah
    ror al, 2
    and al, 3
    mov indx, al; чтение со страницы электронного диска
    jmp @SSExit
    ;port10 = 10h, 14h, 18h, 1Ch
    ;операции через сигнал STECK
@@:
    .if radr > 9FFFh && radr < 0E000h
        mov al, port10
        mov ah, al
        shr al, 5
        and al, 1
        mov stack, al
        jz @SSExit
        and ah, 3
        mov indx, ah; чтение со страницы электронного диска
        ;port10 = 20h, 21h, 22h, 23h
        ;замена экранного ОЗУ
    .endif
@SSExit:
    mov al, indx
    ret
; выходной параметр - номер источника данных
SelSteckRAMPage ENDP

CalcVirtualReg PROC padr:WORD
; al - код источника данных
; регистры не сохраняются
    mov ebx, -65536
    lea esi, rom
    cmp al, 5
    je @@0
    lea esi, ram0
    or al,al
    jz @@0
    mov cl, al
@@1:
    add esi, ebx
    dec cl
    jnz @@1
@@0:
    movzx ebx, padr
    add ebx, esi
    mov eax, ebx
    ret
CalcVirtualReg ENDP


GetPCCmd PROC
    push esi
    push ebx
    push ecx
    invoke SelM1RAMPage, pc
    ;теперь в аккумуляторе хранится код источника данных
    ;5 - ПЗУ
    ;4 - ОЗУ
    ;3..0 - ЭД
    invoke CalcVirtualReg, pc
    mov vPC, eax ;сохраняем указатель на код команды
    mov eax, [eax] ;считываем код операции в аккумулятор
    pop ecx
    pop ebx
    pop esi
    ret
GetPCCmd ENDP

vBCdetect PROC
    push esi
    push ebx
    push ecx
    invoke SelM1RAMPage, r.w.BC
    ;теперь в аккумуляторе хранится код источника данных
    invoke CalcVirtualReg, r.w.BC
    mov vBC, eax ;сохраняем указатель на [BC]
    mov al, [eax] ;считываем содержимое ячейки в аккумулятор
    pop ecx
    pop ebx
    pop esi
    ret
vBCdetect ENDP

vDEdetect PROC
    push esi
    push ebx
    push ecx
    invoke SelM1RAMPage, r.w.DE
    ;теперь в аккумуляторе хранится код источника данных
    invoke CalcVirtualReg, r.w.DE
    mov vDE, eax ;сохраняем указатель на [DE]
    mov al, [eax] ;считываем содержимое ячейки в аккумулятор
    pop ecx
    pop ebx
    pop esi
    ret
vDEdetect ENDP

vIXdetect PROC
    push esi
    push ebx
    push ecx
    invoke SelM1RAMPage, r.w.IX
    ;теперь в аккумуляторе хранится код источника данных
    invoke CalcVirtualReg, r.w.IX
    mov vIX, eax ;сохраняем указатель на [IX]
    mov al, [eax] ;считываем содержимое ячейки в аккумулятор
    pop ecx
    pop ebx
    pop esi
    ret
vIXdetect ENDP

vIYdetect PROC
    push esi
    push ebx
    push ecx
    invoke SelM1RAMPage, r.w.IY
    ;теперь в аккумуляторе хранится код источника данных
    invoke CalcVirtualReg, r.w.IY
    mov vIY, eax ;сохраняем указатель на [IY]
    mov al, [eax] ;считываем содержимое ячейки в аккумулятор
    pop ecx
    pop ebx
    pop esi
    ret
vIYdetect ENDP

vHLdetect PROC
    push esi
    push ebx
    push ecx
    invoke SelSteckRAMPage, r.w.HL, 0
    ;теперь в аккумуляторе хранится код источника данных
    invoke CalcVirtualReg, r.w.HL
    mov vHL, eax ;сохраняем указатель на [HL]
    mov al, [eax] ;считываем содержимое ячейки в аккумулятор
    pop ecx
    pop ebx
    pop esi
    ret
vHLdetect ENDP

vSPdetect PROC
    push esi
    push ebx
    push ecx
    invoke SelSteckRAMPage, spw, 1
    ;теперь в аккумуляторе хранится код источника данных
    invoke CalcVirtualReg, spw
    mov vSP, eax ;сохраняем указатель на [SP]
    mov ax, [eax] ;считываем содержимое ячейки в аккумулятор
    pop ecx
    pop ebx
    pop esi
    ret
vSPdetect ENDP

vImm16 PROC imm16:WORD
; вычисление указателя на ячейку памяти в командах (LD A,(XXXX), LD HL,(XXXX) и т.п.)
    push esi
    push ebx
    push ecx
    invoke SelM1RAMPage, imm16
    ;теперь в аккумуляторе хранится код источника данных
    invoke CalcVirtualReg, imm16
    ;получили указатель на ячейку
    pop ecx
    pop ebx
    pop esi
    ret
vImm16 ENDP

include io_ports.inc

Z80Processor PROC
        push ebx
        push ecx
        push edx
        push esi
        mov al, iif
        mov ah, al
        and al, 0FCh
        and ah, 0Ch
        shr ah, 2
        or al, ah
        mov iif, al
        ; далее следует расшифровка команд
        call GetPCCmd
        and al, al
        jnz @Next76
        ;NOP
        inc pc
        mov al, 1
        jmp @Complete
 
@Next76: cmp al, 76h
        jne @Next40
        ;HALT
        mov al, 1
        jmp @Complete
 
@Next40: test al, 40h
        jz @Next27; коды команд 00-3Fh, 80-0BFh
        test al, 80h
        jnz @NextC0; коды команд 0C0-0FFh
        ;MOV R,R
        mov ah, al
        and al, 7
        cmp al, 6
        jae @F
        xor al, 1
    @@: shr ah, 3
        and ah, 7
        cmp ah, 6
        jae @F
        xor ah, 1
    @@: push ax
        pop cx
        lea esi, r.w.BC
        push esi
        pop ebx
        movzx edx, al
        add esi, edx; источник данных
        movzx edx, ah
        add ebx, edx; приемник данных
        cmp cl, 6
        mov cl, 1
        jne @F
        mov cl, 2
        call vHLdetect
        mov esi, vHL
    @@: cmp ch, 6
        jne @F
        mov cl, 2
        call vHLdetect
        mov ebx, vHL
    @@: mov al, [esi]
        cmp ch, 6
        jne @Mov00
        cmp r.b.rH, 40h
        jae @Mov00
        test port80, 20h
        jnz @Mov01
        mov dx, r.w.HL
        cmp dx, MaxRomAddr
        ja @Mov00
        test blk, 1
        jnz @Mov00
        add ebx, 32768; данные будут помещены в ОЗУ
@Mov00: mov [ebx], al
        cmp ch, 6
        jne @Mov01
        VideoMem r.w.HL
@Mov01: mov al, cl
        inc pc
        jmp @Complete
 
@Next27: cmp al, 27h
        jne @Next2F
        ;DAA
        GetFlags
        pushf
        mov al, r.b.rA
        nf
        jz @F
        ;DAA для вычитания
        popf
        das
        jmp @Next27a
    @@: popf
        daa
@Next27a: mov r.b.rA, al
        lahf
    	mov al, r.b.rF
    	and al, 2
    	and ah, 11111101b
    	or al, ah
    	mov r.b.rF, al
        inc pc
        mov al, 1
        jmp @Complete
 
@Next2F: cmp al, 2Fh
        jne @Next37
        ;CMA
        mov al, r.b.rA
        not al
        mov r.b.rA, al
        mov ah, r.b.rF
        and ah, 11000111b
        and al, 28h; биты будут выставлены в регистре флагов
        or al, 12h
        or al, ah
        mov r.b.rF, al
        inc pc
        mov al, 1
        jmp @Complete
 
@Next37: cmp al, 37h
        jne @Next3F
        ;SCF
        mov ax, r.w.AF
        and al, 11000101b
        or al, 1
        and ah, 28h
        or al, ah
        mov r.b.rF, al
        inc pc
        mov al, 1
        jmp @Complete
 
@Next3F: cmp al, 3Fh
        jne @Next80
        ;CCF
        mov ax, r.w.AF
        and al, 11000101b
        cf
        jz @F
        or al, 10h
    @@: xor al, 1
        and ah, 28h
        or al, ah
        mov r.b.rF, al
        inc pc
        mov al, 1
        jmp @Complete
@Next80: test al, 80h
        jz @Next1
        ;ADD, ADC, SUB, SBB, ...
        mov ah, al
        and al, 7
        cmp al, 6
        jae @F
        xor al, 1
    @@: shr ah, 3
        and ah, 7
        cmp ah, 3
        ja @NextA0
        mov ch, ah
        lea esi, r.w.BC
        movzx edx, al
        add esi, edx; источник данных
        mov cl, 1
        cmp al, 6
        jne @F
        mov cl, 2
        call vHLdetect
        mov esi, vHL
    @@: GetFlags
        mov al, r.b.rA; приемник данных
        pushf
        and ch,ch
        jnz @Next88
        popf
        add al, [esi]
        jmp @F
@Next88:
        dec ch
        jnz @Next90
        popf
        adc al, [esi]
    @@: mov r.b.rA, al
        pushf
        pop ax
        and ax, 0000100011111001b
        shr ah, 1
        or al, ah
        mov r.b.rF, al
        mov al, cl
        inc pc
        jmp @Complete

@Next90:
        dec ch
        jnz @Next98
        popf
        sub al, [esi]
        jmp @F
@Next98:
        popf
        sbb al, [esi]
    @@: mov r.b.rA, al
        pushf
        pop ax
        and ax, 0000100011111011b
        shr ah, 1
        or al, ah
        mov r.b.rF, al
        mov al, cl
        inc pc
        jmp @Complete

@NextA0:;AND, OR, XOR, CMP
        and ah, 3
        mov ch, ah
        lea esi, r.w.BC
        movzx edx, al
        add esi, edx; источник данных
        mov cl, 1
        cmp al, 6
        jne @F
        mov cl, 2
        call vHLdetect
        mov esi, vHL
    @@: GetFlags
        lea ebx, r.b.rA; приемник данных
        mov al, [ebx]
        pushf
        and ch,ch
        jnz @NextA8
        ;AND R
        popf
        and al, [esi]
        mov [ebx], al
        lahf
        and ah, 11111100b
        or ah, 10h
        mov r.b.rF, ah
        mov al, cl
        inc pc
        jmp @Complete
@NextA8:
        dec ch
        jnz @NextB0
        ;XOR R
        popf
        xor al, [esi]
        jmp @F
@NextB0:
        dec ch
        jnz @NextB8
        ;OR R
        popf
        or al, [esi]
    @@: mov [ebx], al
        lahf
        and ah, 11101100b
        mov r.b.rF, ah
        mov al, cl
        inc pc
        jmp @Complete

@NextB8:;CMP R
        popf
        cmp al, [esi]
        pushf
        pop ax
        and ax, 0000100011010011b
        shr ah, 1
        or al, ah
        mov ah, [esi]
        and ah, 28h
        or al, ah
        mov r.b.rF, al
        mov al, cl
        inc pc
        jmp @Complete

@Next1: mov bl, al
        and bl, 7
        dec bl
        jnz @Next2
        test al, 8
        jnz @Next9
        ;LD BC nn, LD DE nn...
        lea ebx, r.w.BC
        and al, 30h
        shr al, 3
        cmp al, 6
        jb @F
        lea ebx, spw-6
@@:     movzx edx, al
        add ebx, edx
        ror eax, 8
        mov [ebx], ax
        mov al, 3
        inc pc
        inc pc
        inc pc
        jmp @Complete

@Next2: dec bl
        jnz @Next3
        ;LD (BC), A ...
        test al, 20h
        jnz @Next22
        test al, 8
        jnz @NextA
        test al, 10h
        jnz @Next12
        call vBCdetect
        mov ebx, vBC
        mov dx, r.w.BC
        jmp @F
@Next12:
        call vDEdetect
        mov dx, r.w.DE
        mov ebx, vDE
@@:     cmp dh, 40h
        jae @N12ram
        test port80, 20h
        jnz @N12rom
        cmp dx, MaxRomAddr
        ja @N12ram
        test blk, 1
        jnz @N12ram
        add ebx, 32768; данные будут помещены в ОЗУ
@N12ram:
        mov al, r.b.rA
        mov [ebx], al
        VideoMem dx
@N12rom:
        inc pc
        mov al, 2
        jmp @Complete
 
@NextA: test al, 10h
        jnz @Next1A
        call vBCdetect
        jmp @F
@Next1A:
        call vDEdetect
@@:     mov r.b.rA, al
        inc pc
        mov al, 2
        jmp @Complete
 
@Next22:
        mov bl, al
        shr eax, 8
        mov dx, ax
        invoke vImm16, ax
        test bl, 10h
        jnz @Next32
        ;LD HL, (xxxx)...
        test bl, 8
        jnz @Next2A
        mov bx, r.w.HL
        cmp dh, 40h
        jae @N22ram
        test port80, 20h
        jnz @N22rom
        cmp dx, MaxRomAddr
        ja @N22ram
        test blk, 1
        jnz @N22ram
        add eax, 32768; данные будут помещены в ОЗУ
@N22ram: mov [eax], bx
        VideoMem dx
        inc dx
        VideoMem dx
@N22rom: inc pc
        inc pc
        inc pc
        mov al, 5
        jmp @Complete

@Next2A:
        mov ax, [eax]
        mov r.w.HL, ax
        jmp @N22rom
 
@Next32:;LD A, (xxxx)...
        test bl, 8
        jnz @Next3A
        mov bl, r.b.rA
        cmp dh, 40h
        jae @N32ram
        test port80, 20h
        jnz @N32rom
        cmp dx, MaxRomAddr
        ja @N32ram
        test blk, 1
        jnz @N32ram
        add eax, 32768; данные будут помещены в ОЗУ
@N32ram: mov [eax], bl
        VideoMem dx
@N32rom: inc pc
        inc pc
        inc pc
        mov al, 4
        jmp @Complete
 
@Next3A:
        mov al, [eax]
        mov r.b.rA, al
        jmp @N32rom

@Next9: lea ebx, r.w.BC; ADD HL, RP
        and al, 30h
        shr al, 3
        cmp al, 6
        jb @F
        lea ebx, spw-6
@@:     movzx edx, al
        add ebx, edx
        lea esi, r.w.HL
        mov ax, [ebx]
        add ax, [esi]
        mov [esi], ax
        lahf
        and ah, 00111001b
        mov al, r.b.rF
        and al, 11000100b
        or al, ah
        mov r.b.rF, al
        mov al, 3
        inc pc
        jmp @Complete

@Next3: dec bl
        jnz @Next4
        ;INC RP, DEC RP
        mov cl, al
        lea ebx, r.w.BC
        and al, 30h
        shr al, 3
        cmp al, 6
        jb @F
        lea ebx, spw-6
@@:     movzx edx, al
        add ebx, edx
        test cl, 8
        jnz @DecRP
        inc word ptr [ebx]
        jmp @F
@DecRP: dec word ptr [ebx]
@@:     inc pc
        mov al, 2
        jmp @Complete

@Next4: cmp bl, 2
        ja @Next6
        ;INC R, DEC R
        mov ch, al
        and al, 38h
        shr al, 3
        cmp al, 6
        jae @F
        xor al, 1
    @@: lea esi, r.w.BC
        movzx edx, al
        add esi, edx; источник данных
        mov cl, 1
        cmp al, 6
        jne @F
        mov cl, 2
        call vHLdetect
        mov esi, vHL
        cmp r.b.rH, 40h
        jae @F
        test port80, 20h
        jnz @N4rom
        mov dx, r.w.HL
        cmp dx, MaxRomAddr
        ja @F
        test blk, 1
        jnz @F
        add esi, 32768; данные будут помещены в ОЗУ
@@:     test ch, 1
        jnz @DecR
        inc byte ptr [esi]
@N4ram: pushf
        pop ax
        and ax, 08F8h
        shr ah, 1
        or al, ah
        mov ah, r.b.rF
        and ah, 1
        or al, ah
        mov r.b.rF, al
        cmp cl, 2
        jne @F
        VideoMem r.w.HL
@@:     mov al, cl
        inc pc
        jmp @Complete

@N4rom: test ch, 1
        jnz @N5rom
        mov al, [esi]
        inc al
        jmp @N4ram
 
@DecR:  dec byte ptr [esi]
@N5ram: pushf
        pop ax
        and ax, 08FAh
        shr ah, 1
        or al, ah
        mov ah, r.b.rF
        and ah, 1
        or al, ah
        mov r.b.rF, al
        cmp cl, 2
        jne @F
        VideoMem r.w.HL
@@:     mov al, cl
        inc pc
        jmp @Complete
@N5rom: mov al, [esi]
        dec al
        jmp @N5ram
 
@Next6: cmp bl, 3
        jne @Next7
        ;MOV R, n
        mov ch, ah
        and al, 38h
        shr al, 3
        cmp al, 6
        jae @F
        xor al, 1
    @@: lea esi, r.w.BC
        movzx edx, al
        add esi, edx; источник данных
        mov cl, 2
        cmp al, 6
        jne @N6ram
        mov cl, 3
        call vHLdetect
        mov esi, vHL
        mov dx, r.w.HL
        cmp dh, 40h
        jae @N6ram
        test port80, 20h
        jnz @N6rom
        cmp dx, MaxRomAddr
        ja @N6ram
        test blk, 1
        jnz @N6ram
        add esi, 32768; данные будут помещены в ОЗУ
@N6ram: mov [esi], ch
        cmp cl, 3
        jne @N6rom
        VideoMem r.w.HL
@N6rom: mov al, cl
        inc pc
        inc pc
        jmp @Complete
 
@Next7: cmp bl, 4
        jne @Next8
        ; RLCA...
        mov ah, al
        lea ebx, r.b.rA
        mov al, [ebx]
        test ah, 10h
        jnz @Next17
        test ah, 8
        jnz @NextF
        rol al, 1
@RlcExit:
        mov [ebx], al
        lahf
        and ah, 1
        mov al, r.b.rF
        and al, 0C4h
        or al, ah
        shl ah, 3
        or al, ah
        shl ah, 2
        or al, ah
        mov r.b.rF, al
        inc pc
        mov al, 1
        jmp @Complete
 
 
@NextF: ror al, 1
        jmp @RlcExit
@Next17:
        test ah, 8
        jnz @Next1F
        push ebx
        GetFlags
        pop ebx
        mov al, [ebx]
        rcl al, 1
        jmp @RlcExit
@Next1F:
        push ebx
        GetFlags
        pop ebx
        mov al, [ebx]
        rcr al, 1
        jmp @RlcExit

@Next8: cmp al, 8
        jne @Next10
        ;EXAF
        lea ebx, r.w.AF
        lea esi, r.w.aAF
        mov ax, [esi]
        rol eax, 16
        mov ax, [ebx]
        mov [esi], ax
        ror eax, 16
        mov [ebx], ax
        mov al, 1
        inc pc
        jmp @Complete
 
@Next10:
        cmp al, 10h
        jne @Next18
        ;DJNZ
        mov bx, pc
        inc bx
        inc bx
        dec r.b.rB
        jz @F
        movsx ax, ah
        add ax, bx
        mov pc, ax
        mov al, 4
        jmp @Complete
@@:     mov pc, bx
        mov al, 3
        jmp @Complete
 
@Next18:
        cmp al, 18h
        jne @Next20
        ;JR
        mov bx, pc
        inc bx
        inc bx
        movsx ax, ah
        add ax, bx
        mov pc, ax
        mov al, 3
        jmp @Complete

@Next20:
        cmp al, 20h
        jne @Next28
        ;JRNZ
        mov bx, pc
        inc bx
        inc bx
        zf
        jnz @F
@Jrnz:  movsx ax, ah
        add ax, bx
        mov pc, ax
        mov al, 3
        jmp @Complete
@@:     mov pc, bx
        mov al, 2
        jmp @Complete
 
@Next28:
        cmp al, 28h
        jne @Next30
        ;JRZ
        mov bx, pc
        inc bx
        inc bx
        zf
        jz @B
        jmp @Jrnz

@Next30:
        cmp al, 30h
        jne @Next38
        ;JRNC
        mov bx, pc
        inc bx
        inc bx
        cf
        jnz @B
        jmp @Jrnz

@Next38:;JRC
        mov bx, pc
        inc bx
        inc bx
        cf
        jz @B
        jmp @Jrnz

@NextC0: mov bl, al
        and bl, 07h
        jnz @NextC1
        ;RET XX
        mov bl, al
        shr bl, 4
        and bl, 3
        jnz @NextD0
        ;RETNZ/Z
        and al, 8
        shl al, 3
        xor al, r.b.rF
        and al, 40h
        jz @C9Comm
        inc pc
        mov al, 2
        jmp @Complete
@NextD0: dec bl
        jnz @NextE0
        ;RETNC/C
        and al, 8
        shr al, 3
        xor al, r.b.rF
        and al, 1
        jz @C9Comm
        inc pc
        mov al, 2
        jmp @Complete
@NextE0: dec bl
        jnz @NextF0
        ;RETPO/PE
        and al, 8
        shr al, 1
        xor al, r.b.rF
        and al, 4
        jz @C9Comm
        inc pc
        mov al, 2
        jmp @Complete
@NextF0:;RETP/M
        and al, 8
        shl al, 4
        xor al, r.b.rF
        and al, 80h
        jz @C9Comm
        inc pc
        mov al, 2
        jmp @Complete
 
@NextC1: cmp bl, 1
        jne @NextC5
        test al, 8
        jnz @NextC9
        ;POP
        and al, 30h
        shr al, 3
        movzx eax, al
        lea ebx, r.w.BC
        add ebx, eax
        call vSPdetect
        mov [ebx], ax
        add spw, 02h
        mov al, 4
        inc pc
        jmp @Complete
 
@NextC5: cmp bl, 5
        jne @NextC2
        test al, 8
        jnz @NextCD
        ;PUSH
        and al, 30h
        shr al, 3
        movzx eax, al
        lea ebx, r.w.BC
        add ebx, eax
        sub spw, 02h
        call vSPdetect
        mov esi, vSP
        mov dx, spw
        cmp dh, 40h
        jae @F
        test port80, 20h
        jnz @NC5rom
        cmp dx, MaxRomAddr
        ja @F
        test blk, 1
        jnz @F
        add esi, 32768; данные будут помещены в ОЗУ
@@:     mov ax, [ebx]
        mov [esi], ax
@NC5rom:
        VideoMem dx
        inc dx
        VideoMem dx
        mov al, 4
        inc pc
        jmp @Complete

@NextC2: cmp bl, 2
        jne @NextC3
        ;JP XX
        mov bl, al
        shr bl, 4
        and bl, 3
        jnz @NextD2
        ;JP NZ/Z
        and al, 8
        shl al, 3
        xor al, r.b.rF
        and al, 40h
        jz @JPComm
        inc pc
        inc pc
        inc pc
        mov al, 3
        jmp @Complete
@NextD2: dec bl
        jnz @NextE2
        ;JP NC/C
        and al, 8
        shr al, 3
        xor al, r.b.rF
        and al, 1
        jz @JPComm
        inc pc
        inc pc
        inc pc
        mov al, 3
        jmp @Complete
@NextE2: dec bl
        jnz @NextF2
        ;JP PO/PE
        and al, 8
        shr al, 1
        xor al, r.b.rF
        and al, 4
        jz @JPComm
        inc pc
        inc pc
        inc pc
        mov al, 3
        jmp @Complete
@NextF2:;JP P/M
        and al, 8
        shl al, 4
        xor al, r.b.rF
        and al, 80h
        jz @JPComm
        inc pc
        inc pc
        inc pc
        mov al, 3
        jmp @Complete

@NextC3: cmp bl, 3
        jne @NextC4
        test al, 8
        jnz @NextCB
        cmp al, 0C3h
        jne @NextD3
        ;JMP
@JPComm: ror eax, 8
        mov pc, ax
        mov al, 3
        jmp @Complete

@NextD3: cmp al, 0D3h
        jne @NextE3
        ;OUT xx
        mov al, r.b.rA
        call OutPortProc
        inc pc
        inc pc
        mov al, 3
        jmp @Complete
 
@NextE3: cmp al, 0E3h
        jne @NextF3
        ;EX (SP), HL
        call vSPdetect
        push ax
        mov ebx, vSP
        mov ax, r.w.HL
        mov [ebx], ax
        pop ax
        mov r.w.HL, ax
	mov dx, spw
        VideoMem dx
        inc dx
        VideoMem dx
        inc pc
        mov al, 6
        jmp @Complete
 
@NextF3: ;DI
        mov al, iif
        and al, 0F3h
        mov iif, al
        inc pc
        mov al, 1
        jmp @Complete

@NextC4: cmp bl, 4
	jne @NextC6
	;CALL XX
        mov bl, al
        shr bl, 4
        and bl, 3
	jnz @NextD4
	;CALL NZ/Z
        and al, 8
        shl al, 3
        xor al, r.b.rF
        and al, 40h
        jz @CDComm
        inc pc
        inc pc
        inc pc
        mov al, 3
        jmp @Complete

@NextD4: dec bl
        jnz @NextE4
        ;CALL NC/C
        and al, 8
        shr al, 3
        xor al, r.b.rF
        and al, 1
        jz @CDComm
        inc pc
        inc pc
        inc pc
        mov al, 3
        jmp @Complete

@NextE4: dec bl
        jnz @NextF4
        ;CALL PO/PE
        and al, 8
        shr al, 1
        xor al, r.b.rF
        and al, 4
        jz @CDComm
        inc pc
        inc pc
        inc pc
        mov al, 3
        jmp @Complete

@NextF4:;CALL P/M
        and al, 8
        shl al, 4
        xor al, r.b.rF
        and al, 80h
        jz @CDComm
        inc pc
        inc pc
        inc pc
        mov al, 3
        jmp @Complete

@NextC6: cmp bl, 6
	    jne @NextC7
        ;ADD n, ADC n, SUB n, SBB n, ...
        shr al, 3
        and al, 7
        cmp al, 3
        ja @NextE6
        mov cx, ax
        GetFlags
        lea ebx, r.b.rA; приемник данных
        pushf
        and cl,cl
        jnz @NextCE
        popf
        mov al, [ebx]
        add al, ch
        jmp @F
@NextCE:
        dec cl
        jnz @NextD6
        popf
        mov al, [ebx]
        adc al, ch
    @@: mov [ebx], al
        pushf
        pop ax
        and ax, 08F9h
        shr ah, 1
        or al, ah
        mov r.b.rF, al
        mov al, 2
        inc pc
        inc pc
        jmp @Complete

@NextD6:
        dec cl
        jnz @NextDE
        popf
        mov al, [ebx]
        sub al, ch
        jmp @F
@NextDE:
        popf
        mov al, [ebx]
        sbb al, ch
    @@: mov [ebx], al
        pushf
        pop ax
        and ax, 08FBh
        shr ah, 1
        or al, ah
        mov r.b.rF, al
        mov al, 2
        inc pc
        inc pc
        jmp @Complete
 
@NextE6: ;AND n, OR n, XOR n, CMP n
        sub al, 4
        mov cx, ax
        GetFlags
        lea ebx, r.b.rA; приемник данных
        pushf
        and cl,cl
        jnz @NextEE
        ;AND n
        popf
        mov al, [ebx]
        and al, ch
        mov [ebx], al
        lahf
        and ah, 0ECh
        or ah, 10h
        mov r.b.rF, ah
        mov al, 2
        inc pc
        inc pc
        jmp @Complete
@NextEE:
        dec cl
        jnz @NextF6
        ;XOR n
        popf
        mov al, [ebx]
        xor al, ch
        jmp @F
@NextF6:
        dec cl
        jnz @NextFE
        ;OR n
        popf
        mov al, [ebx]
        or al, ch
    @@: mov [ebx], al
        lahf
        and ah, 0ECh
        mov r.b.rF, ah
        mov al, 2
        inc pc
        inc pc
        jmp @Complete

@NextFE:;CP n
        popf
        mov al, [ebx]
        cmp al, ch
        pushf
        pop ax
        and ax, 08D3h
        shr ah, 1
        or al, ah
        and ch, 28h
        or al, ch
        mov r.b.rF, al
        mov al, 2
        inc pc
        inc pc
        jmp @Complete
	

@NextC7: ;RST XX
	    and al, 38h
	    movzx eax, al
	    rol ax, 8
	    mov cl, 4
	    jmp @RSTComm

@NextC9: cmp al, 0C9h
         jne @NextD9
         ;RET
@C9Comm: call vSPdetect
         mov pc, ax
         add spw, 02h
         mov al, 3
         jmp @Complete

@NextD9: cmp al, 0D9h
        jne @NextE9
        ;EXX
        lea ebx, r.w.BC
        lea esi, r.w.aBC
        mov cl, 3
@@:     mov ax, [ebx]
        push ax
        mov ax, [esi]
        push ax
        inc ebx
        inc ebx
        inc esi
        inc esi
        dec cl
        jnz @B
        dec esi
        dec esi
        dec ebx
        dec ebx
        mov cl, 3
@@:     pop ax
        mov [ebx], ax
        pop ax
        mov [esi], ax
        dec ebx
        dec ebx
        dec esi
        dec esi
        dec cl
        jnz @B
        inc pc
        mov al, 1
        jmp @Complete
 
@NextE9: cmp al, 0E9h
        jne @NextF9
        ;PCHL
        mov ax, r.w.HL
        mov pc, ax
        mov al, 1
        jmp @Complete
 
@NextF9: ;SPHL
        mov ax, r.w.HL
        mov spw, ax
        inc pc
        mov al, 2
        jmp @Complete

@NextDB: cmp al, 0DBh
        jne @NextEB
        ;IN xx
        lea esi, r.b.rA
        mov al, [esi]
        call InPortProc
        inc pc
        inc pc
        mov al,3
        jmp @Complete
 
@NextEB: cmp al, 0EBh
        jne @NextFB
        ;EX DE,HL
        mov ax, r.w.HL
        rol eax, 16
        mov ax, r.w.DE
        mov r.w.HL, ax
        ror eax, 16
        mov r.w.DE, ax
        inc pc
        mov al, 1
        jmp @Complete
 
@NextFB: ;EI
        mov al, iif
        or al, 0Ch
        mov iif, al
        inc pc
        mov al, 1
        jmp @Complete


@NextCD: cmp al, 0CDh
         jne @NextDD
         ;CALL
@CDComm: mov cl, 5
	    inc pc
	    inc pc
@RSTComm:
	    inc pc
	    sub spw, 02h
	    push eax
	    call vSPdetect
	    mov ebx, vSP
        mov dx, spw
        cmp dh, 40h
        jae @F
        test port80, 20h
        jnz @NCDrom
        cmp dx, MaxRomAddr
        ja @F
        test blk, 1
        jnz @F
        add ebx, 32768; данные будут помещены в ОЗУ
@@:	    mov ax, pc
	    mov [ebx], ax
@NCDrom: pop eax
	    ror eax, 8
        mov pc, ax
	    VideoMem dx
	    inc dx
	    VideoMem dx
        mov al, cl
        jmp @Complete

@NextCB: cmp al, 0CBh
        jne @NextDB
        ;набор команд Z80
        mov al, ah
        and al, 0C0h
        jnz @NextCB_40
        ;RLC r, ...
        mov ch, ah
        and ah, 7h
        cmp ah, 6
        jae @F
        xor ah, 1
    @@: lea esi, r.w.BC
        movzx edx, ah
        add esi, edx; источник данных
        mov cl, 2
        cmp ah, 6
        jne @RlcR
        mov cl, 3
        call vHLdetect
        mov esi, vHL
        mov dx, r.w.HL
        cmp dh, 40h
        jae @RlcR
        test port80, 20h
        jnz @RlcRom
        cmp dx, MaxRomAddr
        ja @RlcR
        test blk, 1
        jnz @RlcR
        add esi, 32768; данные будут помещены в ОЗУ
@RlcR:  mov ah, ch
        and ah, 38h
        jnz @RrcR
        rol byte ptr [esi], 1
@RlcComm:
        lahf
        mov al, [esi]
        or al, al
        mov al, ah
        lahf
        and ax, 1110110011101101b
        or al, ah
        mov r.b.rF, al
        cmp cl, 3
        jne @RlcRom
        VideoMem r.w.HL
@RlcRom: mov al, cl
        inc pc
        inc pc
        jmp @Complete

@RrcR:  shr ah, 3
        dec ah
        jnz @Rl_R
        ror byte ptr [esi], 1
        jmp @RlcComm
@Rl_R:  dec ah
        jnz @Rr_R
        GetFlags
        rcl byte ptr [esi], 1
        jmp @RlcComm
@Rr_R:  dec ah
        jnz @SLA_R
        GetFlags
        rcr byte ptr [esi], 1
        jmp @RlcComm
@SLA_R: dec ah
        jnz @SRA_R
        GetFlags
        shl byte ptr [esi], 1
        jmp @RlcComm
@SRA_R: dec ah
        jnz @SLL_R
        GetFlags
        sar byte ptr [esi], 1
        jmp @RlcComm
@SLL_R: dec ah
        jnz @SRL_R
        GetFlags
        stc
        rcl byte ptr [esi], 1
        jmp @RlcComm
@SRL_R: GetFlags
        shr byte ptr [esi], 1
        jmp @RlcComm
 
@NextCB_40:
        mov al, ah
        and al, 38h
        shr al, 3
        mov cl, al
        mov al, 1
        shl al, cl
        mov ch, al
        ; ch - маска для оператора AND
        test ah, 80h
        jnz @NextCB_80
        ;BIT n, R
        and ah, 7h
        cmp ah, 6
        jae @F
        xor ah, 1
    @@: lea esi, r.w.BC
        movzx edx, ah
        add esi, edx; источник данных
        mov cl, 2
        cmp ah, 6
        jne @BitR
        mov cl, 3
        call vHLdetect
        mov esi, vHL
@BitR:  mov al, [esi]
        and al, ch
        lahf
        and ah, 01000000b
        or al, ah
        and al, 11111000b
        shr ah, 4
        or al, ah
        or al, 10h
        mov ah, r.b.rF
        and ah, 1
        or al, ah
        mov r.b.rF, al
        jmp @RlcRom
 
@NextCB_80:
        test ah, 40h
        pushf
        and ah, 7h
        cmp ah, 6
        jae @F
        xor ah, 1
    @@: lea esi, r.w.BC
        movzx edx, ah
        add esi, edx; источник данных
        mov cl, 2
        cmp ah, 6
        jne @SetR
        mov cl, 3
        call vHLdetect
        mov esi, vHL
        mov dx, r.w.HL
        cmp dh, 40h
        jae @SetR
        test port80, 20h
        jnz @RlcRom
        cmp dx, MaxRomAddr
        ja @SetR
        test blk, 1
        jnz @SetR
        add esi, 32768; данные будут помещены в ОЗУ
@SetR:  popf
        jz @ResR
        or [esi], ch
        jmp @RlcRom
@ResR:  not ch
        and [esi], ch
        jmp @RlcRom
 
@NextDD:
        cmp al, 0DDh
        je @NextFD
        ; команды с регистром IX
@NextED:
        cmp al, 0EDh
        jne @NextFD
        ; команды Z80
        mov al, ah
        cmp al, 40h
        jb @ED_UnDoc
        cmp al, 0A0h
        jae @NextED_A0
        and al, 7
        jnz @ED_Out
        ;IN R,(C)
        and ah, 38h
        shr ah, 3
        cmp ah, 6
        jae @F
        xor ah, 1
@@:     lea esi, r.w.BC
        movzx eax, ah
        add esi, eax; приемник данных
        mov ax, r.w.BC
        xchg ah, al
        call InPortProc
        or al, al
        mov al, r.b.rF
        lahf
        and ax, 1110110000000001b
        or al, ah
        mov r.b.rF, al
@ED_UnDoc:
        inc pc
        inc pc
        mov al,3
        jmp @Complete

@ED_Out:
        dec al
        jnz @SBC_RP
        ;OUT (C),R
        and ah, 38h
        shr ah, 3
        cmp ah, 6
        jae @F
        xor ah, 1
@@:     lea esi, r.w.BC
        movzx eax, ah
        add esi, eax; источник данных
        mov al, [esi]
        mov ah, r.b.rC
        call OutPortProc
        jmp @ED_UnDoc
 
@SBC_RP:
        dec al
        jnz @LD_RP_nn
        mov al, ah
        lea ebx, r.w.BC
        and al, 30h
        shr al, 3
        cmp al, 6
        jb @F
        lea ebx, spw-6
@@:     movzx edx, al
        add ebx, edx
        lea esi, r.w.HL
        test ah, 8
        mov ah, r.b.rF
        jnz @ADC_RP
        sahf
        mov ax, [esi]
        sbb ax, [ebx]
        mov [esi], ax
        pushf
        pop ax
        and ax, 08FBh
@SBC_RP_Comm:
        shr ah, 1
        or al, ah
        mov r.b.rF, al
        inc pc
        inc pc
        mov al,4
        jmp @Complete
@ADC_RP:
        sahf
        mov ax, [ebx]
        adc ax, [esi]
        mov [esi], ax
        pushf
        pop ax
        and ax, 08F9h
        jmp @SBC_RP_Comm

@LD_RP_nn: dec al
        jnz @NegA
        mov cl, ah
        shr eax, 16
        mov dx, ax
        invoke vImm16, ax
        ;LD (nnnn),RP
        lea ebx, r.w.BC
        test cl, 8
        pushf
        and cl, 30h
        shr cl, 3
        cmp cl, 6
        jb @F
        lea ebx, spw-6
@@:     movzx ecx, cl
        add ebx, ecx
        popf
        jnz @NextCB_4B
        mov bx, [ebx]
        cmp dh, 40h
        jae @CB43ram
        test port80, 20h
        jnz @CB43rom
        cmp dx, MaxRomAddr
        ja @CB43ram
        test blk, 1
        jnz @CB43ram
        add eax, 32768; данные будут помещены в ОЗУ
@CB43ram: mov [eax], bx
        VideoMem dx
        inc dx
        VideoMem dx
@CB43rom: inc pc
        inc pc
        inc pc
        inc pc
        mov al, 6
        jmp @Complete

@NextCB_4B:
        mov ax, [eax]
        mov [ebx], ax
        jmp @CB43rom
 
@NegA:  dec al
        jnz @NextED_45
        ;NEG
        mov al, r.b.rA
        neg al
        mov r.b.rA, al
        pushf
        pop ax
        and ax, 08FBh
        shr ah, 1
        or al, ah
        mov r.b.rF, al
        mov al, 2
@ED_2comm:
        inc pc
        inc pc
        jmp @Complete
 
@NextED_45:
        dec al
        jnz @NextIM0
        test ah, 8
        jnz @C9Comm ;RETI
        ;RETN
        mov al, iif
        mov ah, al
        and ax, 0000101011111010b
        shr ah, 1
        or al, ah
        mov iif, al
        jmp @C9Comm

@NextIM0:
        dec al
        jnz @NextED_47
        mov al, 2
        mov imx, 0
        test ah, 10h
        jz @ED_2comm
        mov imx, 1
        test ah, 8
        jz @ED_2comm
        mov imx, 2
        jmp @ED_2comm
 
@NextED_47:
        test ah, 20h
        jnz @NextRRD
        test ah, 8
        jnz @NextED_4F
        ;LD I,A
        test ah, 10h
        jnz @NextED_57
        mov al, r.b.rA
        mov r.b.rI, al
        mov al, 3
        jmp @ED_2comm
@NextED_57:
        mov al, r.b.rI
        mov r.b.rA, al
@ED_57comm:
        or al, al
        lahf
        mov al, iif
        and ax, 1110100000000010b
        or ah, al
        mov al, r.b.rF
        and al, 1
        or al, ah
        mov r.b.rF, al
        mov al, 3
        jmp @ED_2comm

@NextED_4F:
        test ah, 10h
        jnz @NextED_5F
        mov al, r.b.rA
        mov r.b.rR, al
        mov al, 3
        jmp @ED_2comm
@NextED_5F:
        mov al, r.b.rR
        mov r.b.rA, al
        jmp @ED_57comm

@NextED_A0:
        and al, 7
        jnz @NextED_A1
        ;LDI,LDD,LDIR,LDDR
        mov ch, ah
@CBA0Repeat:
        call vHLdetect; источник данных
        call vDEdetect; приемник данных
        mov esi, vHL
        mov ebx, vDE
        mov dx, r.w.DE
        cmp dh, 40h
        jae @CBA0ram
        test port80, 20h
        jnz @CBA0rom
        cmp dx, MaxRomAddr
        ja @CBA0ram
        test blk, 1
        jnz @CBA0ram
        add ebx, 32768; данные будут помещены в ОЗУ
@CBA0ram:
        mov al, [esi]
        mov [ebx], al
        VideoMem dx
        inc dx
        VideoMem dx
@CBA0rom:
        mov ax, 1
        test ch, 8
        jz @F
        neg ax
@@:     add r.w.DE, ax
        add r.w.HL, ax
        and r.b.rF, 11101001b
        mov al, 5
        dec r.w.BC
        jz @ED_2comm
        or r.b.rF, 4
        test ch, 10h
        jz @ED_2comm
        jmp @CBA0Repeat
 
@NextED_A1:
        dec al
        jnz @NextED_A2
        ;CPI,CPD,CPIR,CPDR
        mov ch, ah
@CBA1Repeat:
        call vHLdetect; источник данных
        cmp r.b.rA, al
        pushf
        mov ax, 1
        test ch, 8
        jz @F
        neg ax
@@:     add r.w.HL, ax
        popf
        lahf
        mov al, r.b.rF
        and ax,1111101000000011b
        or al, ah
        or al, 2
        mov r.b.rF, al
        mov al, 5
        dec r.w.BC
        jz @ED_2comm
        or r.b.rF, 4
        test ch, 10h
        jz @ED_2comm
        zf
        jnz @ED_2comm
        jmp @CBA1Repeat
 
@NextED_A2:
        dec al
        jnz @NextED_A3
        ;INI,IND,INIR,INDR
        mov ch, ah
@CBA2Repeat:
        call vHLdetect; приемник данных
        mov esi, vHL
        mov dx, r.w.HL
        cmp dh, 40h
        jae @CBA2ram
        test port80, 20h
        jnz @CBA2rom
        cmp dx, MaxRomAddr
        ja @CBA2ram
        test blk, 1
        jnz @CBA2ram
        add esi, 32768; данные будут помещены в ОЗУ
        jmp @CBA2ram
@CBA2rom:
        lea esi, r.b.rF
@CBA2ram:
        mov ax, r.w.BC
        xchg ah, al
        call InPortProc
        VideoMem dx
        inc dx
        VideoMem dx
        mov ax, 1
        test ch, 8
        jz @F
        neg ax
@@:     add r.w.HL, ax
        and r.b.rF, 10111111b
        or r.b.rF, 2
        mov al, 5
        dec r.b.rB
        jz @F
        test ch, 10h
        jz @ED_2comm
        jmp @CBA2Repeat
@@:     or r.b.rF, 40h
        jmp @ED_2comm

@NextED_A3:
        ;OUTI,OUTD,OUTIR,OUTDR
        mov ch, ah
@CBA3Repeat:
        call vHLdetect
        mov ah, r.b.rC
        call OutPortProc
        mov ax, 1
        test ch, 8
        jz @F
        neg ax
@@:     add r.w.HL, ax
        and r.b.rF, 10111111b
        or r.b.rF, 2
        mov al, 5
        dec r.b.rB
        jz @F
        test ch, 10h
        jz @ED_2comm
        jmp @CBA3Repeat
@@:     or r.b.rF, 40h
        jmp @ED_2comm

@NextRRD:
;       inc pc
        mov al, 1; недокументированная операция
        jmp @ED_2comm

@NextFD:
        lea esi,r.w.IX
        cmp al, 0FDh
        jne @F
        lea esi,r.w.IY
@@:     ror eax, 8
        test al, 40h
        jz @IX09; коды команд 00-3Fh, 80-0BFh
        test al, 80h
        jnz @IXCB; коды команд 0CB-0FFh
	cmp al, 067h
        jne @IX6F
        ;MOV IXH,A
	mov ax, [esi]
	mov ah, r.b.rA
        mov [esi], ax
        mov al, 2
        jmp @ED_2comm
@IX6F:	cmp al, 06Fh
        jne @F
        ;MOV IXL,A
	mov ax, [esi]
	mov al, r.b.rA
        mov [esi], ax
        mov al, 2
        jmp @ED_2comm
@@:	cmp al, 07Ch
        jne @F
        ;MOV A,IXH
	mov ax, [esi]
	mov r.b.rA, ah
        mov al, 2
        jmp @ED_2comm
@@:	cmp al, 07Dh
        jne @F
        ;MOV A,IXL
	mov ax, [esi]
	mov r.b.rA, al
        mov al, 2
        jmp @ED_2comm
	;MOV R,R
@@:     movsx dx, ah
        add dx, word ptr [esi]
        ;теперь содержимое dx равно IX+d
        lea esi, r.w.BC
        mov ah, al
        and al, 7
        cmp al, 6
        jae @F
        xor al, 1
    @@: shr ah, 3
        and ah, 7
        cmp ah, 6
        jae @F
        xor ah, 1
    @@: cmp ah, 6; =6 приемник данных IX+d иначе это источник данных
        jne @IXsrc
        movzx ebx, al
        add esi, ebx; источник данных
        invoke vImm16, dx
        mov ebx, eax
        mov al, [esi]
        cmp dh, 40h
        jae @MovIX00
        test port80, 20h
        jnz @MovIX01
        cmp dx, MaxRomAddr
        ja @MovIX00
        test blk, 1
        jnz @MovIX00
        add ebx, 32768; данные будут помещены в ОЗУ
@MovIX00: mov [ebx], al
        VideoMem dx
@MovIX01: mov al, 5
        inc pc
        inc pc
        inc pc
        jmp @Complete
@IXsrc: movzx ebx, ah
        add esi, ebx; приемник данных
        invoke vImm16, dx
        mov ebx, eax
        mov al, [ebx]
        mov [esi], al
        jmp @MovIX01
 
@IX09:  test al, 80h
	jnz @IX86
	mov cl, al
        and al, 7
        dec al
        jnz @IX22
        mov al, cl
        test al, 8
        jz @IX21
        ;ADD IX,RP
        lea ebx, r.w.BC
        and al, 30h
        shr al, 3
        cmp al, 6
        jb @F
        lea ebx, spw-6
@@:     movzx edx, al
        add ebx, edx
        cmp al, 4
        jnz @F
        push esi
        pop ebx
@@:     mov ax, [ebx]
        add ax, [esi]
        mov [esi], ax
        lahf
        and ah, 00111001b
        mov al, r.b.rF
        and al, 11000100b
        or al, ah
        mov r.b.rF, al
        mov al, 4
        jmp @ED_2comm
 
@IX21:  ror eax, 8; LD IX,nn
        mov [esi], ax
        mov al, 4
        inc pc
        inc pc
        inc pc
        inc pc
        jmp @Complete

@IX22:  dec al
        jnz @IX23
        ror eax, 8; LD (nn),IX
        mov dx, ax
        invoke vImm16, ax
        test cl, 8
        jnz @IX2A
        mov bx, [esi]
        cmp dh, 40h
        jae @IX22ram
        test port80, 20h
        jnz @IX22rom
        cmp dx, MaxRomAddr
        ja @IX22ram
        test blk, 1
        jnz @IX22ram
        add eax, 32768; данные будут помещены в ОЗУ
@IX22ram: mov [eax], bx
        VideoMem dx
        inc dx
        VideoMem dx
@IX22rom: inc pc
        inc pc
        inc pc
        inc pc
        mov al, 6
        jmp @Complete
@IX2A:  mov ax, [eax]
        mov [esi], ax
        jmp @IX22rom
@IX23:	dec al
        jnz @IX34
	inc word ptr[esi]
        test cl, 8
        mov al, 2
        jz @ED_2comm
@IX2B:	dec word ptr[esi]
	dec word ptr[esi]
        jmp @ED_2comm
@IX34:	movsx dx, ah
	add dx, word ptr [esi]
        ;теперь содержимое dx равно IX+d
	cmp al, 3h ; 
	je @IX36
	mov ch, al
        invoke vImm16, dx
        mov esi, eax
	cmp dh, 40h
        jae @F
        test port80, 20h
        jnz @IX34rom
        cmp dx, MaxRomAddr
        ja @F
        test blk, 1
        jnz @F
        add esi, 32768; данные будут помещены в ОЗУ
@@:     test ch, 1
        jz @IX35
        inc byte ptr [esi]
@IX34ram: pushf
        pop ax
        and ax, 08F8h
        shr ah, 1
        or al, ah
        mov ah, r.b.rF
        and ah, 1
        or al, ah
        mov r.b.rF, al
        VideoMem dx
@@:     mov al, 6
        inc pc
	inc pc
	inc pc
        jmp @Complete

@IX34rom: test ch, 1
        jz @IX35rom
        mov al, [esi]
        inc al
        jmp @IX34ram
 
@IX35:  dec byte ptr [esi]
@IX35ram: pushf
        pop ax
        and ax, 08FAh
        shr ah, 1
        or al, ah
        mov ah, r.b.rF
        and ah, 1
        or al, ah
        mov r.b.rF, al
        VideoMem dx
@@:     mov al, 6
        inc pc
	inc pc
	inc pc
        jmp @Complete
@IX35rom: mov al, [esi]
        dec al
        jmp @IX35ram
 
@IX36: ;MOV (IX+d), n
	ror eax, 8
	mov ch, ah
        invoke vImm16, dx
        mov esi, eax
	cmp dh, 40h
        jae @IX36ram
        test port80, 20h
        jnz @IX36rom
        cmp dx, MaxRomAddr
        ja @IX36ram
        test blk, 1
        jnz @IX36ram
        add esi, 32768; данные будут помещены в ОЗУ
@IX36ram: mov [esi], ch
        VideoMem dx
@IX36rom: mov al, 6
        inc pc
        inc pc
        inc pc
        inc pc
	jmp @Complete

@IX86:	;ADD, ADC, SUB, SBB, ...
        movsx dx, ah
	add dx, word ptr [esi]
        ;теперь содержимое dx равно IX+d
	push eax
        invoke vImm16, dx
        mov esi, eax
	pop eax
	shr al, 3
        and al, 7
	mov ch,al
        cmp al, 3
        ja @IXA6
	GetFlags
        mov al, r.b.rA; приемник данных
        pushf
        and ch,ch
        jnz @IX8E
        popf
        add al, [esi]
        jmp @F
@IX8E:  dec ch
        jnz @IX96
        popf
        adc al, [esi]
    @@: mov r.b.rA, al
        pushf
        pop ax
        and ax, 0000100011111001b
        shr ah, 1
        or al, ah
        mov r.b.rF, al
        mov al, 5
        inc pc
	inc pc
	inc pc
        jmp @Complete
@IX96:  dec ch
        jnz @IX9E
        popf
        sub al, [esi]
        jmp @F
@IX9E:  popf
        sbb al, [esi]
    @@: mov r.b.rA, al
        pushf
        pop ax
        and ax, 0000100011111011b
        shr ah, 1
        or al, ah
        mov r.b.rF, al
        mov al, 5
        inc pc
	inc pc
	inc pc
        jmp @Complete
@IXA6:	;AND, OR, XOR, CMP
        and al, 3
        mov ch, al
	GetFlags
        mov al, r.b.rA; приемник данных
        pushf
        and ch,ch
        jnz @IXAE
        ;AND R
        popf
        and al, [esi]
        mov r.b.rA, al
        lahf
        and ah, 11111100b
        or ah, 10h
        mov r.b.rF, ah
        mov al, 5
        inc pc
        inc pc
        inc pc
        jmp @Complete
@IXAE:  dec ch
        jnz @IXB6
        ;XOR R
        popf
        xor al, [esi]
        jmp @F
@IXB6:  dec ch
        jnz @IXBE
        ;OR R
        popf
        or al, [esi]
    @@: mov r.b.rA, al
        lahf
        and ah, 11101100b
        mov r.b.rF, ah
        mov al, 5
        inc pc
        inc pc
        inc pc
        jmp @Complete
@IXBE:	;CMP R
        popf
        cmp al, [esi]
        pushf
        pop ax
        and ax, 0000100011010011b
        shr ah, 1
        or al, ah
        mov ah, [esi]
        and ah, 28h
        or al, ah
        mov r.b.rF, al
        mov al, 5
        inc pc
        inc pc
        inc pc
        jmp @Complete
@IXCB:
@IXE1: cmp al, 0E1h
        jne @IXE5
        ;POP IX
        call vSPdetect
        mov [esi], ax
        add spw, 02h
        mov al, 5
        inc pc
	inc pc
        jmp @Complete
 
@IXE5: cmp al, 0E5h
        jne @IXE3
        ;PUSH
        sub spw, 02h
        call vSPdetect
        mov ebx, vSP
        mov dx, spw
        cmp dh, 40h
        jae @F
        test port80, 20h
        jnz @IXE5rom
        cmp dx, MaxRomAddr
        ja @F
        test blk, 1
        jnz @F
        add ebx, 32768; данные будут помещены в ОЗУ
@@:     mov ax, [esi]
        mov [ebx], ax
@IXE5rom:
        VideoMem dx
        inc dx
        VideoMem dx
        mov al, 5
	inc pc
        inc pc
        jmp @Complete

@IXE3: cmp al, 0E3h
        jne @IXE9
	; EX (SP),IX
        call vSPdetect
        push ax
        mov ebx, vSP
        mov ax, [esi]
        mov [ebx], ax
        pop ax
        mov [esi], ax
	mov dx, spw
        VideoMem dx
        inc dx
        VideoMem dx
        inc pc
	inc pc
        mov al, 7
        jmp @Complete

@IXE9: cmp al, 0E9h
        jne @IXF9
        ;PCIX
        mov ax, [esi]
        mov pc, ax
        mov al, 2
        jmp @Complete

@IXF9: cmp al, 0F9h
        jne @Complete
	;LD SP,IX
        mov ax, [esi]
        mov spw, ax
        inc pc
	inc pc
        mov al, 7
        ; работа с портами ВЕКТОРА
        ; работа с таймером ВИ53
@Complete:
        mov ah, port80[1]
        mov port80, ah
        pop esi
        pop edx
        pop ecx
        pop ebx
        ret
Z80Processor ENDP
