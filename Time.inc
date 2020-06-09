RDTSCM MACRO
       xor eax,eax
       cpuid
       rdtsc
       ENDM
	   
.data
ALIGN 8
	TCLCK dd 0, 0
	

.code
       
GetTimeConstants MACRO
	   xor eax,eax
           invoke QueryPerformanceFrequency, ADDR TCLCK
           cmp eax, 0
		   je @oldalg
               mov eax, DWORD PTR TCLCK
               shl eax, 10
               xor edx, edx
               mov ebx, 4000000; вычислить колво тактов соотвествующих частоте 4Mhz
               div ebx
           jmp @comonalg
@oldalg:	   xor eax,eax
	   mov TCLCK,eax
	   mov ecx, 50; измерить 1c
@@tmloop:  push ecx
	   RDTSCM
           push eax
           mov eax, 20; sleep на 20 мс (50Hz)
           invoke Sleep, eax
	   RDTSCM
           mov edx, eax
           pop eax
           sub edx, eax
           mov eax, edx
	   add eax, TCLCK
	   mov TCLCK, eax
	   pop ecx
	   dec ecx
	   jnz @@tmloop

                  xor edx, edx
                  mov ebx, 4000000; вычислить колво тактов соотвествующих частоте 4Mhz
                  div ebx
@comonalg:
       mov h1MHz, eax
       mov h50Hz, 80000; кол-во циклов в 50Hz
       mov h22kHz, f22kHz
	   mov p9f,0
	   mov pAf,0
	   mov pBf,0
                 ENDM
Cycle4MHz   MACRO
       RDTSCM
       mov TCLCK,eax
       xor eax,eax
       cpuid
       mov eax, TCLCK
       sub eax, h1MHzStart
       cmp eax, h1MHz
       jb @@2
		cmp port9,0
		je @testpa
		dec p9cnt
		cmp p9cnt,0
		jne @testpa
		xor p9f,48
		mov ax, word ptr port9
		shl ax, 1
		mov p9cnt, ax
@testpa:
		cmp portA,0
		je @testpb
		dec pAcnt
		cmp pAcnt,0
		jne @testpb
		xor pAf,32;48
		mov ax, word ptr portA
		shl ax, 1
		mov pAcnt, ax
@testpb:
		cmp portB,0
		je @test22kHz
		dec pBcnt
		cmp pBcnt,0
		jne @test22kHz
		xor pBf,48
		mov ax, word ptr portB
		shl ax, 1
		mov pBcnt, ax
@test22kHz:
       dec h22kHz
       inc h50HzStart
       inc b12_8kHz
       cmp b12_8kHz, fRow
       jb @NoBorderCall
       mov b12_8kHz, 0
       call SetBorderPixel
       dec bBorderRow
@NoBorderCall:
       mov al, r.b.rR
       and al, 80h
       inc r.b.rR
       and r.b.rR, 7Fh
       or r.b.rR, al
       cmp CmdLength, 0
       jz @@0
       dec CmdLength
       jnz @@1
@@0:   
       ;вставить вызов подпрограммы расшифровки команд если нет режима отладки
       mov al, NeedBpx
       and al, al
       jz @@NoBpx
       mov ax, pc
       cmp ax, BpxAddr
       jne @@NoBpx
       mov NeedBpx, 0
       invoke DialogBoxParam,hInstance,ADDR DialogName,0,ADDR DlgRegsProc,0
@@NoBpx:
       call Z80Processor
       ;по возвращении из п.п al содержит кол-во тактов на выполнение команды ¬≈ “ќ–ј
	   shl al,2
       mov CmdLength, al
@@1:   
       mov eax,TCLCK
       mov h1MHzStart, eax
@@2:       
       ENDM

Cycle50Hz   MACRO handle
       mov eax, h50HzStart
       cmp eax, h50Hz
       jb @F
       mov bBorderRow, 0
       mov eax, 8001; —генерировать прерывание
       invoke PostMessage, handle, WM_COMMAND, eax, 0
       xor eax,eax
       mov h50HzStart, eax
@@:       
            ENDM

Cycle22kHz MACRO
           cmp h22kHz,0
           jne @F
           mov h22kHz, f22kHz
		   ;звук из пищалки
		   xor eax, eax
             mov al, port0
             and al, 1
             add al, al
             add al, al
             add al, al
             mov ah, al
		 ; порты ВИ53
		 add ah, p9f
	         add ah, pAf
		 add al, pAf
		 add al, pBf
             call PlayWave
@@:
           ENDM
.const
fRow EQU 240
f22kHz EQU 181

.data
h1MHz dd ?
h22kHz dd ?
h50Hz dd ?
h1MHzStart dd ?
h50HzStart dd ?
p9cnt dw ?
pAcnt dw ?
pBcnt dw ?
p9f db ?
pAf db ?
pBf db ?
b12_8kHz db ?
bBorderRow db ?
