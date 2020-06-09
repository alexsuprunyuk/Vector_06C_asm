;набор структур и переменных для описания
;регистров процессора Z80

;регистровые пары РОН
WordRegs STRUCT
	BC	dw ?
	DE	dw ?
	HL	dw ?
	AF	dw ?
	aBC	dw ?
	aDE	dw ?
	aHL	dw ?
	aAF	dw ?
	IX	dw ?
	IY	dw ?
	IR	dw ?
WordRegs ENDS

;регистры РОН
ByteRegs STRUCT
	rC	db ?
	rB	db ?
	rE	db ?
	rD	db ?
	rL	db ?
	rH	db ?
	rF	db ?
	rA	db ?
	rCa	db ?
	rBa	db ?
	rEa	db ?
	rDa	db ?
	rLa	db ?
	rHa	db ?
	rFa	db ?
	rAa	db ?
	IXl	db ?
	IXh	db ?
	IYl	db ?
	IYh	db ?
	rR	db ?
	rI	db ?
ByteRegs ENDS

Z80REGS UNION
	b ByteRegs <>
	w WordRegs <>
Z80REGS ENDS

;макросы для определения состояний флагов процессора Z80

GetFlags MACRO
	mov al, r.b.rF
	mov bl, al
;	and al, cPF
;	shl al, 1
;	mov bh, al
	pushf
	pop ax
	mov al, bl
;	and ah, 0F7h
;	or ah, bh
	push ax
	popf
	ENDM

sf	MACRO
	test r.b.rF, cSF
	ENDM
zf	MACRO
	test r.b.rF, cZF
	ENDM
hf	MACRO
	test r.b.rF, cHF
	ENDM
pf	MACRO
	test r.b.rF, cPF
	ENDM
nf	MACRO
	test r.b.rF, cNF
	ENDM
cf	MACRO
	test r.b.rF, cCF
	ENDM

.const
;константы для описания флагов процессора Z80
cSF EQU 80h
cZF EQU 40h
cHF EQU 10h
cPF EQU 04h
cNF EQU 02h
cCF EQU 01h

.data
ALIGN 8
MaxRomAddr dw 4096, 0
MyRom	db 03Eh, 09Bh, 0D3h, 004h, 03Eh, 088h, 0D3h, 000h, 031h, 0F0h, 0DCh, 011h, 0AAh, 055h, 0D5h, 0E1h
db 07Dh, 02Fh, 0BCh, 0C2h, 00Eh, 000h, 03Eh, 0A8h, 001h, 040h, 000h, 0D3h, 008h, 091h, 0D2h, 01Bh
db 000h, 048h, 03Eh, 006h, 0D3h, 000h, 0AFh, 0D3h, 010h, 002h, 003h, 002h, 003h, 0B9h, 0C2h, 029h
db 000h, 0B8h, 0C2h, 029h, 000h, 0C3h, 039h, 000h, 0C9h, 011h, 009h, 000h, 060h, 06Ah, 0CDh, 045h
db 005h, 02Dh, 036h, 0FFh, 019h, 036h, 0FFh, 02Ch, 036h, 081h, 004h, 0C2h, 03Ch, 000h, 0FBh, 076h
db 0CDh, 0A1h, 002h, 021h, 060h, 007h, 011h, 0E9h, 0C4h, 03Eh, 008h, 006h, 007h, 0CDh, 0B7h, 006h
db 021h, 09Fh, 000h, 0E5h, 03Eh, 0C3h, 032h, 0F1h, 0DCh, 032h, 0F4h, 0DCh, 03Ah, 0F7h, 0DEh, 0FEh
db 0F7h, 0CAh, 04Bh, 004h, 0FEh, 0EFh, 0CAh, 090h, 000h, 0FEh, 0E7h, 0CAh, 084h, 000h, 0CDh, 05Eh
db 005h, 0C2h, 09Dh, 005h, 0CDh, 0D9h, 000h, 0C2h, 048h, 001h, 0CDh, 038h, 001h, 0C2h, 0E7h, 002h
db 0CDh, 0D9h, 000h, 0C2h, 04Bh, 004h, 0CDh, 007h, 001h, 0C2h, 074h, 002h, 0C3h, 04Bh, 004h, 03Eh
db 09Bh, 0D3h, 004h, 03Eh, 003h, 0D3h, 000h, 0FBh, 076h, 0CDh, 0A1h, 002h, 03Ah, 0F7h, 0DEh, 0FEh
db 0D7h, 0C2h, 0CAh, 000h, 021h, 0DAh, 006h, 011h, 0DAh, 0C0h, 03Eh, 00Fh, 006h, 002h, 0CDh, 0B7h
db 006h, 01Eh, 0DDh, 03Eh, 008h, 006h, 00Dh, 0CDh, 0B7h, 006h, 003h, 079h, 0E6h, 008h, 03Eh, 007h
db 0C2h, 0D4h, 000h, 03Dh, 0D3h, 000h, 0C3h, 0A7h, 000h, 006h, 004h, 0DBh, 007h, 0F6h, 0E0h, 04Fh
db 03Eh, 08Bh, 0D3h, 004h, 079h, 0D3h, 007h, 0E3h, 0E3h, 0DBh, 005h, 0E6h, 008h, 0CAh, 002h, 001h
db 03Eh, 07Fh, 0A1h, 0D3h, 007h, 0DBh, 005h, 02Fh, 0E6h, 008h, 0CAh, 002h, 001h, 005h, 0C2h, 0E4h
db 000h, 0B7h, 03Eh, 09Bh, 0D3h, 004h, 0C9h, 03Eh, 082h, 0D3h, 004h, 006h, 0FFh, 078h, 0D3h, 005h
db 03Eh, 0FEh, 0D3h, 007h, 0DBh, 006h, 0FEh, 055h, 0C2h, 02Eh, 001h, 03Eh, 0FFh, 0D3h, 007h, 0DBh
db 006h, 0FEh, 0AAh, 0C2h, 02Eh, 001h, 078h, 0E6h, 07Fh, 03Ch, 057h, 0C3h, 036h, 001h, 078h, 0D6h
db 020h, 047h, 0FAh, 00Dh, 001h, 0AFh, 0B7h, 0C9h, 03Eh, 00Bh, 0D3h, 019h, 04Fh, 0E3h, 0E3h, 0DBh
db 019h, 0B9h, 0CAh, 046h, 001h, 0AFh, 0B7h, 0C9h, 021h, 0ECh, 007h, 03Eh, 08Ah, 0CDh, 0B2h, 006h
db 021h, 01Fh, 002h, 022h, 0F2h, 0DCh, 021h, 05Ch, 001h, 022h, 0F5h, 0DCh, 03Eh, 09Bh, 0D3h, 004h
db 0DBh, 007h, 0E6h, 01Fh, 04Fh, 006h, 00Ah, 0DBh, 005h, 0E6h, 070h, 0FEh, 040h, 0C2h, 05Ch, 001h
db 0DBh, 006h, 0E6h, 01Fh, 0B9h, 0C2h, 05Ch, 001h, 005h, 0C2h, 067h, 001h, 0DBh, 006h, 047h, 03Eh
db 098h, 0D3h, 004h, 079h, 0D3h, 006h, 0AFh, 0D3h, 005h, 016h, 0FAh, 015h, 0CAh, 05Ch, 001h, 0DBh
db 005h, 0E6h, 070h, 0FEh, 070h, 0C2h, 08Bh, 001h, 03Eh, 09Bh, 0D3h, 004h, 078h, 0E6h, 0E0h, 0C2h
db 05Ch, 001h, 01Eh, 003h, 03Eh, 008h, 0CDh, 0F1h, 0DCh, 0FEh, 055h, 0CAh, 0B3h, 001h, 0FEh, 0AAh
db 0C2h, 0F4h, 0DCh, 01Dh, 0C2h, 0A4h, 001h, 0CDh, 0D8h, 004h, 05Fh, 07Eh, 0FEh, 000h, 0C2h, 0B7h
db 001h, 021h, 0F1h, 0DEh, 07Eh, 02Bh, 0BEh, 0C2h, 0F4h, 0DCh, 057h, 02Bh, 046h, 0CDh, 0D8h, 004h
db 0F5h, 07Eh, 0FEh, 000h, 0CAh, 00Ah, 002h, 087h, 087h, 087h, 087h, 087h, 04Fh, 0F1h, 023h, 07Eh
db 0BBh, 0C2h, 0CDh, 001h, 0D5h, 0C5h, 023h, 011h, 07Eh, 020h, 07Eh, 002h, 00Ah, 0AEh, 077h, 023h
db 00Ch, 015h, 0C2h, 0EAh, 001h, 0C1h, 069h, 060h, 0CDh, 045h, 005h, 073h, 0D1h, 0CDh, 07Dh, 004h
db 0CAh, 0CDh, 001h, 07Ah, 0FEh, 001h, 0C2h, 0CDh, 001h, 0C9h, 0F1h, 093h, 0CAh, 0CDh, 001h, 03Ch
db 0C2h, 000h, 000h, 0CDh, 07Dh, 004h, 0CAh, 000h, 000h, 01Dh, 004h, 015h, 0C3h, 0CDh, 001h, 0D5h
db 016h, 070h, 0FEh, 0FFh, 0CCh, 055h, 002h, 0CDh, 02Ch, 002h, 0D1h, 0C9h, 01Eh, 060h, 0DBh, 005h
db 0A2h, 0FEh, 040h, 0CAh, 000h, 000h, 0BBh, 0C2h, 02Eh, 002h, 0DBh, 006h, 0F5h, 03Eh, 09Ah, 0D3h
db 004h, 0AFh, 0D3h, 005h, 01Dh, 0CAh, 04Fh, 002h, 0DBh, 005h, 0A2h, 0BAh, 0C2h, 044h, 002h, 03Eh
db 09Bh, 0D3h, 004h, 0F1h, 0C9h, 0CDh, 02Ch, 002h, 0FEh, 0E6h, 0C2h, 055h, 002h, 0C9h, 074h, 065h
db 06Dh, 069h, 072h, 061h, 07Ah, 06Fh, 077h, 020h, 064h, 02Eh, 073h, 06Fh, 06Bh, 06Fh, 06Ch, 06Fh
db 077h, 020h, 061h, 02Eh, 021h, 0DEh, 007h, 03Eh, 087h, 0D5h, 0CDh, 0B2h, 006h, 0D1h, 001h, 000h
db 000h, 021h, 000h, 080h, 07Dh, 0D3h, 007h, 07Ch, 0D3h, 005h, 0DBh, 006h, 002h, 023h, 003h, 078h
db 0BAh, 0C2h, 084h, 002h, 021h, 000h, 000h, 0CDh, 0B5h, 003h, 024h, 07Ch, 0BAh, 0C2h, 097h, 002h
db 0C9h, 021h, 0D7h, 002h, 0F5h, 0C5h, 0E5h, 006h, 00Fh, 078h, 0D3h, 002h, 07Eh, 0D3h, 00Ch, 01Ch
db 0D3h, 00Ch, 023h, 0D3h, 00Ch, 01Dh, 005h, 0D3h, 00Ch, 0F2h, 0A9h, 002h, 03Eh, 08Ah, 0D3h, 000h
db 03Eh, 0FDh, 0D3h, 003h, 0DBh, 002h, 032h, 0F7h, 0DEh, 03Eh, 088h, 0D3h, 000h, 0AFh, 0D3h, 002h
db 03Dh, 0D3h, 003h, 0E1h, 0C1h, 0F1h, 0C9h, 02Dh, 02Dh, 080h, 080h, 02Dh, 02Dh, 080h, 080h, 02Dh
db 02Dh, 080h, 080h, 02Dh, 02Dh, 080h, 080h, 021h, 0CAh, 007h, 03Eh, 08Ah, 0CDh, 0B2h, 006h, 03Eh
db 034h, 032h, 0D0h, 0DEh, 0CDh, 03Fh, 004h, 03Eh, 003h, 0CDh, 02Ch, 004h, 03Eh, 010h, 032h, 00Ch
db 011h, 03Eh, 001h, 0D3h, 019h, 011h, 000h, 011h, 0CDh, 0C6h, 003h, 021h, 000h, 011h, 011h, 0F1h
db 0DEh, 00Eh, 01Fh, 03Eh, 066h, 086h, 0F5h, 07Eh, 012h, 0F1h, 023h, 01Bh, 00Dh, 0C2h, 015h, 003h
db 096h, 0C2h, 000h, 000h, 012h, 03Ah, 004h, 011h, 04Fh, 021h, 000h, 011h, 0EBh, 0AFh, 03Ch, 0D3h
db 019h, 0C5h, 0CDh, 0C6h, 003h, 0C1h, 0C2h, 000h, 000h, 00Dh, 0CAh, 07Eh, 003h, 0DBh, 019h, 021h
db 00Ch, 011h, 0BEh, 0DAh, 02Eh, 003h, 03Ah, 00Bh, 011h, 0B7h, 0CAh, 069h, 003h, 03Ah, 0D0h, 0DEh
db 0E6h, 004h, 03Ah, 0D0h, 0DEh, 0CAh, 062h, 003h, 0E6h, 0FBh, 032h, 0D0h, 0DEh, 0D3h, 01Ch, 0C3h
db 02Dh, 003h, 0F6h, 004h, 032h, 0D0h, 0DEh, 0D3h, 01Ch, 0CDh, 02Ah, 004h, 03Ah, 00Eh, 011h, 0FEh
db 028h, 0CCh, 02Ah, 004h, 021h, 0D2h, 0DEh, 034h, 07Eh, 0D3h, 01Ah, 0C3h, 02Dh, 003h, 001h, 080h
db 011h, 02Ah, 000h, 011h, 0EBh, 07Bh, 091h, 07Ah, 098h, 0EBh, 0D2h, 09Bh, 003h, 00Ah, 0CDh, 0B0h
db 003h, 003h, 023h, 07Bh, 091h, 07Ah, 098h, 0D8h, 0C3h, 08Dh, 003h, 019h, 07Dh, 091h, 06Fh, 07Ch
db 098h, 067h, 01Ah, 0CDh, 0B0h, 003h, 07Bh, 091h, 07Ah, 098h, 0D8h, 01Bh, 02Bh, 0C3h, 0A2h, 003h
db 077h, 07Dh, 0FEh, 000h, 0C0h, 0E5h, 0C5h, 0CDh, 045h, 005h, 00Eh, 008h, 036h, 07Eh, 02Ch, 00Dh
db 0C2h, 0BCh, 003h, 0C1h, 0E1h, 0C9h, 0CDh, 03Fh, 004h, 03Eh, 084h, 026h, 002h, 0D3h, 01Bh, 0DBh
db 01Bh, 00Fh, 0D2h, 0CFh, 003h, 03Ah, 00Ch, 011h, 0FEh, 010h, 0CAh, 011h, 004h, 0FEh, 008h, 0CAh
db 003h, 004h, 0FEh, 009h, 0CAh, 003h, 004h, 0DBh, 01Bh, 0A4h, 0CAh, 0E7h, 003h, 0DBh, 018h, 012h
db 01Ch, 0C2h, 0E7h, 003h, 014h, 0DBh, 01Bh, 0A4h, 0CAh, 0F5h, 003h, 0DBh, 018h, 012h, 01Ch, 0C2h
db 0F5h, 003h, 014h, 0DBh, 01Bh, 0A4h, 0CAh, 003h, 004h, 0DBh, 018h, 012h, 01Ch, 0C2h, 003h, 004h
db 014h, 0DBh, 01Bh, 0A4h, 0CAh, 011h, 004h, 0DBh, 018h, 012h, 01Ch, 0C2h, 011h, 004h, 014h, 0DBh
db 01Bh, 00Fh, 0DAh, 01Fh, 004h, 0DBh, 01Bh, 0E6h, 05Ch, 0C9h, 03Eh, 05Bh, 0D3h, 01Bh, 0AFh, 03Dh
db 0C2h, 02Fh, 004h, 03Ah, 0D0h, 0DEh, 0D3h, 01Ch, 0DBh, 01Bh, 00Fh, 0DAh, 033h, 004h, 0C9h, 03Ah
db 0D0h, 0DEh, 0D3h, 01Ch, 0DBh, 01Bh, 007h, 0D0h, 0C3h, 03Fh, 004h, 021h, 098h, 007h, 03Eh, 00Dh
db 0CDh, 0B2h, 006h, 03Eh, 011h, 032h, 0F6h, 0DEh, 021h, 0FCh, 004h, 022h, 0F2h, 0DCh, 021h, 04Bh
db 004h, 022h, 0F5h, 0DCh, 0CDh, 0ADh, 004h, 057h, 0B7h, 01Fh, 05Fh, 082h, 067h, 0CDh, 0ADh, 004h
db 0BCh, 0DAh, 067h, 004h, 082h, 032h, 0F6h, 0DEh, 01Eh, 00Ch, 0C3h, 0A4h, 001h, 02Eh, 000h, 060h
db 0CDh, 045h, 005h, 07Eh, 0A7h, 0C8h, 023h, 0FEh, 081h, 0C2h, 083h, 004h, 0A7h, 0C9h, 0D5h, 0DBh
db 001h, 0E6h, 010h, 05Fh, 0DBh, 001h, 0E6h, 010h, 0BBh, 0CAh, 094h, 004h, 05Fh, 016h, 001h, 0DBh
db 001h, 0E6h, 010h, 014h, 0BBh, 0CAh, 09Fh, 004h, 07Ah, 087h, 087h, 0D1h, 0C9h, 0E5h, 0D5h, 0CDh
db 08Eh, 004h, 047h, 0B7h, 01Fh, 04Fh, 021h, 000h, 000h, 016h, 020h, 0CDh, 08Eh, 004h, 0D5h, 016h
db 000h, 05Fh, 019h, 0D1h, 05Fh, 090h, 0D2h, 0CBh, 004h, 078h, 093h, 0B9h, 0D2h, 0AFh, 004h, 015h
db 0C2h, 0BBh, 004h, 029h, 07Ch, 0D1h, 0E1h, 0C9h, 0C5h, 0D5h, 021h, 0D0h, 0DEh, 0E5h, 001h, 023h
db 000h, 03Eh, 0FFh, 0CDh, 0F1h, 0DCh, 077h, 023h, 080h, 047h, 03Eh, 008h, 00Dh, 0C2h, 0E3h, 004h
db 02Bh, 078h, 096h, 096h, 07Eh, 0E1h, 0C2h, 0DDh, 004h, 0D1h, 0C1h, 0C9h, 0C5h, 0D5h, 00Eh, 000h
db 057h, 0DBh, 001h, 0E6h, 010h, 05Fh, 0DBh, 001h, 0E6h, 010h, 0BBh, 0CAh, 006h, 005h, 007h, 007h
db 007h, 007h, 079h, 017h, 04Fh, 03Ah, 0F6h, 0DEh, 03Dh, 0C2h, 018h, 005h, 07Ah, 0B7h, 0F2h, 03Ah
db 005h, 079h, 0FEh, 0E6h, 0C2h, 02Eh, 005h, 0AFh, 032h, 0F4h, 0DEh, 0C3h, 038h, 005h, 0FEh, 019h
db 0C2h, 001h, 005h, 03Eh, 0FFh, 032h, 0F4h, 0DEh, 016h, 009h, 015h, 0C2h, 001h, 005h, 03Ah, 0F4h
db 0DEh, 0A9h, 0D1h, 0C1h, 0C9h, 0D5h, 07Dh, 007h, 007h, 007h, 06Fh, 07Ch, 01Fh, 0E6h, 070h, 057h
db 01Fh, 082h, 085h, 0C6h, 018h, 06Fh, 07Ch, 0E6h, 01Fh, 0C6h, 0C0h, 067h, 0D1h, 0C9h, 021h, 000h
db 0FFh, 022h, 0D5h, 0DEh, 0AFh, 032h, 0D2h, 0DEh, 032h, 0D3h, 0DEh, 03Ch, 032h, 0D4h, 0DEh, 0CDh
db 007h, 006h, 03Eh, 000h, 0C2h, 08Fh, 005h, 021h, 000h, 0FFh, 0E5h, 011h, 091h, 005h, 00Eh, 00Ch
db 01Ah, 0BEh, 03Eh, 000h, 0C2h, 08Eh, 005h, 013h, 023h, 00Dh, 0C2h, 080h, 005h, 03Dh, 0E1h, 0B7h
db 0C9h, 000h, 04Fh, 053h, 020h, 020h, 020h, 020h, 020h, 020h, 043h, 04Fh, 04Dh, 021h, 0B2h, 007h
db 03Eh, 08Ch, 0CDh, 0B2h, 006h, 021h, 000h, 001h, 022h, 0D5h, 0DEh, 044h, 021h, 010h, 0FFh, 00Eh
db 010h, 07Eh, 0B7h, 0CAh, 0D6h, 005h, 0CDh, 0DDh, 005h, 078h, 0C6h, 004h, 047h, 03Ah, 0D2h, 0DEh
db 0B7h, 0C2h, 000h, 000h, 023h, 00Dh, 0C2h, 0B1h, 005h, 0CDh, 07Ah, 005h, 0CAh, 0D6h, 005h, 011h
db 010h, 000h, 019h, 0C3h, 0AFh, 005h, 050h, 021h, 000h, 001h, 0C3h, 097h, 002h, 0F5h, 0E5h, 032h
db 0D3h, 0DEh, 03Eh, 001h, 032h, 0D4h, 0DEh, 0F5h, 0CDh, 007h, 006h, 0CAh, 0F3h, 005h, 03Eh, 0FFh
db 032h, 0D2h, 0DEh, 0F1h, 02Ah, 0D5h, 0DEh, 011h, 080h, 000h, 019h, 022h, 0D5h, 0DEh, 03Ch, 0FEh
db 009h, 0C2h, 0E4h, 005h, 0E1h, 0F1h, 0C9h, 0C5h, 0D5h, 021h, 000h, 000h, 039h, 022h, 0D0h, 0DEh
db 0CDh, 041h, 006h, 0F3h, 0D3h, 010h, 0F9h, 02Ah, 0D5h, 0DEh, 03Eh, 000h, 00Eh, 020h, 0D1h, 083h
db 082h, 073h, 023h, 072h, 023h, 0D1h, 083h, 082h, 073h, 023h, 072h, 023h, 00Dh, 0C2h, 01Eh, 006h
db 04Fh, 03Eh, 000h, 0D3h, 010h, 02Ah, 0D0h, 0DEh, 0F9h, 0FBh, 079h, 0CDh, 08Ch, 006h, 0D1h, 0C1h
db 0C9h, 03Ah, 0D4h, 0DEh, 011h, 080h, 0FFh, 021h, 000h, 004h, 019h, 03Dh, 0C2h, 04Ah, 006h, 0EBh
db 03Ah, 0D3h, 0DEh, 02Fh, 0FEh, 0FCh, 0D2h, 05Bh, 006h, 0D6h, 010h, 0D6h, 004h, 06Fh, 007h, 007h
db 0E6h, 0FCh, 067h, 07Dh, 02Eh, 000h, 019h, 007h, 007h, 007h, 007h, 0E6h, 00Ch, 0C5h, 04Fh, 03Eh
db 010h, 0B1h, 0C1h, 0C9h, 03Ah, 0D3h, 0DEh, 06Fh, 026h, 000h, 029h, 029h, 029h, 029h, 03Eh, 0F0h
db 084h, 067h, 03Ah, 0D4h, 0DEh, 03Dh, 087h, 085h, 06Fh, 03Eh, 01Ch, 0C9h, 0F5h, 0D5h, 021h, 000h
db 000h, 039h, 022h, 0D0h, 0DEh, 0CDh, 074h, 006h, 0F3h, 0D3h, 010h, 0F9h, 0D1h, 03Eh, 000h, 0D3h
db 010h, 02Ah, 0D0h, 0DEh, 0F9h, 0FBh, 0EBh, 0D1h, 0F1h, 0BCh, 0C2h, 0AFh, 006h, 095h, 0C8h, 03Eh
db 0FFh, 0C9h, 006h, 002h, 011h, 0E2h, 0D8h, 032h, 0F7h, 0DCh, 03Ah, 0F7h, 0DCh, 0E6h, 07Fh, 04Fh
db 0D5h, 07Eh, 012h, 013h, 03Ah, 0F7h, 0DCh, 0B7h, 0F2h, 0CEh, 006h, 07Eh, 012h, 013h, 023h, 00Dh
db 0C2h, 0C1h, 006h, 0D1h, 014h, 005h, 0C2h, 0BAh, 006h, 0C9h, 01Fh, 039h, 060h, 0C0h, 0CFh, 099h
db 098h, 098h, 098h, 099h, 0CFh, 0C0h, 060h, 039h, 01Fh, 080h, 0C0h, 060h, 033h, 031h, 091h, 011h
db 011h, 011h, 095h, 037h, 030h, 060h, 0C0h, 080h, 0C7h, 08Dh, 08Ch, 08Fh, 08Dh, 08Dh, 0A7h, 0E0h
db 036h, 0B6h, 036h, 0BEh, 0BEh, 0B6h, 022h, 000h, 0CDh, 0CDh, 0EDh, 0FDh, 0DDh, 0CDh, 0CDh, 000h
db 083h, 086h, 0E6h, 0B6h, 0B6h, 0B6h, 0E3h, 000h, 0A7h, 0EDh, 0E1h, 0C3h, 0C1h, 0EDh, 0A7h, 000h
db 01Ch, 0B6h, 0B6h, 036h, 0B6h, 0B6h, 01Ch, 000h, 0F1h, 0DBh, 0DBh, 0F1h, 0D8h, 0D9h, 0F1h, 000h
db 0ECh, 099h, 035h, 06Dh, 0C1h, 0A1h, 061h, 0C0h, 0E3h, 0B6h, 0B6h, 086h, 086h, 0B6h, 0B3h, 0E0h
db 099h, 0DBh, 0DEh, 0DCh, 0DCh, 0DEh, 09Bh, 000h, 0BBh, 06Dh, 06Dh, 06Dh, 06Dh, 06Ch, 038h, 000h
db 0B3h, 0B6h, 0B6h, 0B6h, 0B6h, 0B6h, 073h, 000h, 09Eh, 0DBh, 0DBh, 0DEh, 0DBh, 0DBh, 09Eh, 000h
db 0F3h, 0DBh, 0DBh, 0DBh, 0F3h, 0DBh, 0DBh, 0F3h, 0DBh, 01Bh, 01Bh, 01Ah, 09Ch, 01Ah, 01Bh, 0DBh
db 018h, 018h, 018h, 018h, 018h, 018h, 05Ah, 07Eh, 073h, 0DBh, 0DBh, 0DBh, 0DBh, 0DBh, 0DBh, 073h
db 000h, 001h, 0C1h, 06Dh, 06Dh, 061h, 061h, 0C0h, 0E3h, 0B6h, 0B6h, 0B6h, 0B7h, 0B6h, 0B6h, 0E3h
db 083h, 0DFh, 0D6h, 0D6h, 096h, 016h, 0D6h, 096h, 03Fh, 0ABh, 0BFh, 0C0h, 0FFh, 0EFh, 0C4h, 0ECh
db 0FFh, 0FFh, 080h, 0C0h, 07Fh, 0FCh, 0D5h, 0FDh, 003h, 0FFh, 0F7h, 023h, 037h, 0FFh, 0FFh, 001h
db 003h, 0FEh, 00Fh, 0FFh, 080h, 092h, 080h, 092h, 080h, 092h, 080h, 092h, 080h, 0FFh, 0F0h, 0FFh
db 001h, 049h, 001h, 049h, 001h, 049h, 001h, 049h, 001h, 0FFh, 0FFh, 0FEh, 0FEh, 0FFh, 0FCh, 0FCh
db 0FFh, 081h, 081h, 0FFh, 0FFh, 07Fh, 07Fh, 0FFh, 03Fh, 03Fh, 0FFh, 0FEh, 0FFh, 0FFh, 055h, 0FFh
db 0C0h, 0C7h, 0C0h, 0FFh, 055h, 054h, 0FFh, 003h, 0C6h, 003h, 0FFh, 054h, 0C1h, 020h, 00Fh, 001h
db 007h, 0E4h, 004h, 007h, 020h, 0C1h, 083h, 004h, 0F0h, 080h, 0E0h, 027h, 020h, 0E0h, 004h, 083h
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

.data?
;элементы компьютера vector06c
rom	db 32768 dup(?) ;ПЗУ
ram	db 65536 dup(?) ;ОЗУ
ram3	db 65536 dup(?) ;Электронны диск 3 страница
ram2	db 65536 dup(?) ;Электронны диск 2 страница
ram1	db 65536 dup(?) ;Электронны диск 1 страница
ram0	db 65536 dup(?) ;Электронны диск 0 страница
pc dw ? ;счетчик команд
spw dw ? ;регистр стека
r Z80REGS <>

port0 db ?
port1 db ?
port2 db 9 dup(?) ;8 байт для режима 8A (сканирование клавиатуры)
;и 1 для режима 88 (создание таблицы цветов)
port3 db 2 dup(?) ;1 байт для режима 8A (сканирование клавиатуры)
;и 1 для режима 88 (скроллинг экрана)
port4 db ?
port5 db ?
port6 db ?
port7 db ?
port8 db 2 dup(?) ;2-й байт служебный, для указания мл.ст. байта портов 9-B
port9 db 2 dup(?) ;два байта для задания мл. и ст. байтов
;коэффициента пересчета
portA db 2 dup(?) ;два байта для задания мл. и ст. байтов
;коэффициента пересчета
portB db 2 dup(?) ;два байта для задания мл. и ст. байтов
;коэффициента пересчета

portC db 16 dup(?) ;таблица цветов
;порты дисковода пока не определены
port10 db ?
port80 db 2 dup (?) ;назначение битов порта 80h:
;0 - включение Sinclair экрана / пока не использую
;1 - инверсия старшего бита адреса ОЗУ / пока не использую
;2 - инверсия сигнала STECK, обмен местами ОЗУ и RAM - диска
;3 - обращение к RAM - диску в операциях с регистром M (HL)
;4 - немаскируемое прерывание при обращении к порту с адресом ***1****
;5 - иммитация ПЗУ для адресов 0000-3FFFh, блокировка RAM - диска при обращении к адресам 4000-5FFFh
;6 - не используется
;7 - блокировка обращения к RAM - диску в стековых операциях
; второй байт используется для задержки выставления битов 1 и 2 port80 на одну команду

blk db ? ; подключено ПЗУ или ОЗУ. Если было нажатие комбинации
;БЛК+СБР, т.е. программа начального загрузчика выполнила свою функцию, то
;данная переменная устанавливается в 1. Если были нажаты ВВОД+БЛК, то данная
;переменная сбрасывается в 0
iif db ? ;флаг разрешения прерываний
imx db ? ;флаг выбора режима прерываний
;указатели на значения хранимые в памяти по адресам в регистровых парах
;будут использованы для отображения в отладчике
vBC dd ?
vDE dd ?
vHL dd ?
vIX dd ?
vIY dd ?
vPC dd ? ;указатель на команду
vSP dd ? ;указатель на значение хранимое в стеке
stack db ? ; для хранения состояния сигнала СТЕК
;порты дисковода
port18 db ? ;Данные (номер дорожки для команды поиск)
port19 db ? ;Сектор
port1A db 2 dup(?) ;Дорожка
port1B db 2 dup(?) ;Регистр управления/статуса
port1C db ? ;Выбор диска, стороны, запуск мотора
stepD db ? ;Направление движения головки
SectIndx dw ? ; считываемый байт сектора
DriveEx db ?; выполнение команды : 0 - завершилась, 1..254 - выполняется, 255 - еще не выполняется
DrvExCnt dw ?