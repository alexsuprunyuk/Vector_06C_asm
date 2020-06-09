.const
IDB_Accept EQU 100
IDB_Restore EQU 101
IDB_Cancel EQU 102
IDB_StepInto EQU 103
IDB_BC EQU 104
IDB_DE EQU 105
IDB_M EQU 106
IDB_PC EQU 107
IDB_SP EQU 108
IDB_Ports EQU 109
IDB_BPX EQU 110

IDB_sf EQU 200
IDB_zf EQU 201
IDB_hf EQU 202
IDB_pf EQU 203
IDB_nf EQU 204
IDB_cf EQU 205
IDB_sf_ EQU 206
IDB_zf_ EQU 207
IDB_hf_ EQU 208
IDB_pf_ EQU 209
IDB_nf_ EQU 210
IDB_cf_ EQU 211

IDB_p10 EQU 280
IDB_p18 EQU 288
IDB_p19 EQU 289
IDB_p1A EQU 290
IDB_p1B EQU 291
IDB_p1C EQU 292
IDB_p80 EQU 299

IDE_B EQU 301
IDE_C EQU 302
IDE_D EQU 303
IDE_E EQU 304
IDE_H EQU 305
IDE_L EQU 306
IDE_A EQU 307
IDE_B_ EQU 309
IDE_C_ EQU 310
IDE_D_ EQU 311
IDE_E_ EQU 312
IDE_H_ EQU 313
IDE_L_ EQU 314
IDE_A_ EQU 315
IDE_M EQU 317
IDE_bc EQU 318
IDE_de EQU 319
IDE_PC EQU 320
IDE_pc EQU 321
IDE_SP EQU 322
IDE_sp EQU 323
IDE_IX EQU 324
IDE_IY EQU 325
IDE_I EQU 326
IDE_R EQU 327
IDE_BPX EQU 328

IDE_p10 EQU 380
IDE_p18 EQU 388
IDE_p19 EQU 389
IDE_p1A EQU 390
IDE_p1B EQU 391
IDE_p1C EQU 392
IDE_p80 EQU 399

IDE_FDDpointer EQU 300
IDE_DrvEx EQU 301

.data
fmt db "%08X", 0
string db "00000000", 0
NeedBpx db 0
BpxAddr dw 0

.code
GetRegisterItemB PROTO :DWORD, :DWORD, :DWORD
SetRegisterItemB PROTO :DWORD, :BYTE, :DWORD
GetRegisterItemW PROTO :DWORD, :DWORD, :DWORD
SetRegisterItemW PROTO :DWORD, :WORD, :DWORD
SetAllRegisters PROTO :DWORD
SetAllPorts PROTO :DWORD

DlgRegsProc proc hDlg   :DWORD,
             uMsg   :DWORD,
             wParam :DWORD,
             lParam :DWORD
      .if uMsg == WM_INITDIALOG
        invoke SetAllRegisters, hDlg
      .elseif uMsg == WM_COMMAND
        mov eax, wParam
        cmp ax, IDB_Accept
        jne @@2
        ;не принимать значения для @bc, @de, @hl, @pc, @sp
        ;для них следует использовать свои кнопки
        invoke GetRegisterItemB, hDlg, ADDR r.b.rB, IDE_B
        invoke GetRegisterItemB, hDlg, ADDR r.b.rC, IDE_C
        invoke GetRegisterItemB, hDlg, ADDR r.b.rD, IDE_D
        invoke GetRegisterItemB, hDlg, ADDR r.b.rE, IDE_E
        invoke GetRegisterItemB, hDlg, ADDR r.b.rH, IDE_H
        invoke GetRegisterItemB, hDlg, ADDR r.b.rL, IDE_L
        invoke GetRegisterItemB, hDlg, ADDR r.b.rA, IDE_A
        invoke GetRegisterItemB, hDlg, ADDR r.b.rBa, IDE_B_
        invoke GetRegisterItemB, hDlg, ADDR r.b.rCa, IDE_C_
        invoke GetRegisterItemB, hDlg, ADDR r.b.rDa, IDE_D_
        invoke GetRegisterItemB, hDlg, ADDR r.b.rEa, IDE_E_
        invoke GetRegisterItemB, hDlg, ADDR r.b.rHa, IDE_H_
        invoke GetRegisterItemB, hDlg, ADDR r.b.rLa, IDE_L_
        invoke GetRegisterItemB, hDlg, ADDR r.b.rAa, IDE_A_
        invoke GetRegisterItemB, hDlg, ADDR r.b.rI, IDE_I
        invoke GetRegisterItemB, hDlg, ADDR r.b.rR, IDE_R
        invoke GetRegisterItemW, hDlg, ADDR r.w.IX, IDE_IX
        invoke GetRegisterItemW, hDlg, ADDR r.w.IY, IDE_IY
        invoke GetRegisterItemW, hDlg, ADDR pc, IDE_PC
        invoke GetRegisterItemW, hDlg, ADDR spw, IDE_SP
        xor ax,ax
        push ax
        invoke IsDlgButtonChecked, hDlg, IDB_sf
        dec al
        jnz @F
        pop ax
        or al, cSF
        push ax
@@:     invoke IsDlgButtonChecked, hDlg, IDB_zf
        dec al
        jnz @F
        pop ax
        or al, cZF
        push ax
@@:     invoke IsDlgButtonChecked, hDlg, IDB_hf
        dec al
        jnz @F
        pop ax
        or al, cHF
        push ax
@@:     invoke IsDlgButtonChecked, hDlg, IDB_pf
        dec al
        jnz @F
        pop ax
        or al, cPF
        push ax
@@:     invoke IsDlgButtonChecked, hDlg, IDB_nf
        dec al
        jnz @F
        pop ax
        or al, cNF
        push ax
@@:     invoke IsDlgButtonChecked, hDlg, IDB_cf
        dec al
        pop ax
        jnz @F
        or al, cCF
@@:     mov r.b.rF, al
        xor ax,ax
        push ax
        invoke IsDlgButtonChecked, hDlg, IDB_sf_
        dec al
        jnz @F
        pop ax
        or al, cSF
        push ax
@@:     invoke IsDlgButtonChecked, hDlg, IDB_zf_
        dec al
        jnz @F
        pop ax
        or al, cZF
        push ax
@@:     invoke IsDlgButtonChecked, hDlg, IDB_hf_
        dec al
        jnz @F
        pop ax
        or al, cHF
        push ax
@@:     invoke IsDlgButtonChecked, hDlg, IDB_pf_
        dec al
        jnz @F
        pop ax
        or al, cPF
        push ax
@@:     invoke IsDlgButtonChecked, hDlg, IDB_nf_
        dec al
        jnz @F
        pop ax
        or al, cNF
        push ax
@@:     invoke IsDlgButtonChecked, hDlg, IDB_cf_
        dec al
        pop ax
        jnz @F
        or al, cCF
@@:     mov r.b.rFa, al
        jmp @@1
@@2:    cmp ax, IDB_BC
        jne @F
        call vBCdetect
        invoke GetRegisterItemB, hDlg, vBC, IDE_bc
        jmp @@1
@@:     cmp ax, IDB_DE
        jne @F
        call vDEdetect
        invoke GetRegisterItemB, hDlg, vDE, IDE_de
        jmp @@1
@@:     cmp ax, IDB_M
        jne @F
        call vHLdetect
        invoke GetRegisterItemB, hDlg, vHL, IDE_M
        jmp @@1
@@:     cmp ax, IDB_PC
        jne @F
        call GetPCCmd
        invoke GetRegisterItemB, hDlg, vPC, IDE_pc
        jmp @@1
@@:     cmp ax, IDB_SP
        jne @F
        call vSPdetect
        invoke GetRegisterItemW, hDlg, vSP, IDE_sp
        jmp @@1
@@:     cmp ax, IDB_Restore
        jne @F
@@1:    invoke SetAllRegisters, hDlg
        jmp @@0
@@:     cmp ax, IDB_Cancel
        jne @F
        invoke EndDialog,hDlg,0
        jmp @@0
@@:     cmp ax, IDB_Ports
        jne @F
        invoke DialogBoxParam,hInstance,ADDR PortsDialogName,0,ADDR DlgPortsProc,0
        jmp @@0
@@:     cmp ax, IDB_StepInto
        jne @F
        mov al, r.b.rR
        and al, 80h
        inc r.b.rR
        and r.b.rR, 7Fh
        or r.b.rR, al
        RDTSCM
        mov h50HzStart, eax
        mov h1MHzStart, eax        
        ;вставить вызов подпрограммы расшифровки команд если нет режима отладки
        call Z80Processor
        ;по возвращении из п.п al содержит кол-во тактов на выполнение команды ВЕКТОРА
        mov CmdLength, al
        invoke SetAllRegisters, hDlg
@@:     cmp ax, IDB_BPX
        jne @@0
        invoke GetRegisterItemW, hDlg, ADDR BpxAddr, IDE_BPX
        mov NeedBpx, 255
        invoke EndDialog,hDlg,0
@@0:
      .elseif uMsg == WM_CLOSE
        invoke EndDialog,hDlg,0
      .endif
    xor eax, eax
    ret
DlgRegsProc endp

BinToHex PROC
        mov ah,al
        ror al,4
        and al,0Fh
        cmp al,9
        jle @F
        add al,7
@@:     add al,30h                
        and ah,0Fh
        cmp ah,9
        jle @F
        add ah,7
@@:     add ah,30h
        movzx eax, ax                
        ret
BinToHex ENDP        

HexToBin PROC
    cmp ah, 30h
    jb @@0
    cmp ah, 46h
    ja @@0
    cmp al, 30h
    jb @@0
    cmp al, 46h
    ja @@0
    sub ax, 3030h
    cmp ah, 9
    jle @F
    sub ah, 7
@@: cmp al, 9
    jle @F
    sub al, 7
@@: rol al, 4
    or al, ah
    movzx eax, al
    ret
@@0: xor eax, eax
     ret            
HexToBin ENDP

SetRegisterItemB PROC hDlg:DWORD, reg:BYTE, id:DWORD
    LOCAL szValue: DWORD       
    mov al, reg
    call BinToHex
    mov szValue, eax
    invoke SetDlgItemText, hDlg, id, ADDR szValue  
    ret
SetRegisterItemB ENDP

GetRegisterItemB PROC hDlg:DWORD, reg:DWORD, id:DWORD
    LOCAL szValue: DWORD       
    mov eax, 3
    invoke GetDlgItemText, hDlg, id, ADDR szValue, eax
    mov eax, szValue
    call HexToBin
    push ebx
    mov ebx, reg
    mov [ebx], al
    pop ebx
    ret
GetRegisterItemB ENDP

SetRegisterItemW PROC hDlg:DWORD, reg:WORD, id:DWORD
    LOCAL szValue: FWORD       
    mov ax, reg
    push ax
    mov al, ah
    call BinToHex
    mov word ptr szValue, ax
    pop ax
    call BinToHex
    mov dword ptr szValue[2], eax
    invoke SetDlgItemText, hDlg, id, ADDR szValue  
    ret
SetRegisterItemW ENDP

GetRegisterItemW PROC hDlg:DWORD, reg:DWORD, id:DWORD
    LOCAL szValue: TBYTE       
    mov eax, 5
    invoke GetDlgItemText, hDlg, id, ADDR szValue, eax
    push ebx
    mov ax, word ptr szValue[2]
    call HexToBin
    mov ebx, reg
    mov [ebx], al
    mov ax, word ptr szValue
    call HexToBin
    inc ebx
    mov [ebx], al
    pop ebx
    ret
GetRegisterItemW ENDP

SetAllRegisters PROC hDlg:DWORD
        invoke SetRegisterItemB, hDlg, r.b.rB, IDE_B
        invoke SetRegisterItemB, hDlg, r.b.rC, IDE_C
        invoke SetRegisterItemB, hDlg, r.b.rD, IDE_D
        invoke SetRegisterItemB, hDlg, r.b.rE, IDE_E
        invoke SetRegisterItemB, hDlg, r.b.rH, IDE_H
        invoke SetRegisterItemB, hDlg, r.b.rL, IDE_L
        invoke SetRegisterItemB, hDlg, r.b.rA, IDE_A
        invoke SetRegisterItemB, hDlg, r.b.rBa, IDE_B_
        invoke SetRegisterItemB, hDlg, r.b.rCa, IDE_C_
        invoke SetRegisterItemB, hDlg, r.b.rDa, IDE_D_
        invoke SetRegisterItemB, hDlg, r.b.rEa, IDE_E_
        invoke SetRegisterItemB, hDlg, r.b.rHa, IDE_H_
        invoke SetRegisterItemB, hDlg, r.b.rLa, IDE_L_
        invoke SetRegisterItemB, hDlg, r.b.rAa, IDE_A_
        invoke SetRegisterItemB, hDlg, r.b.rI, IDE_I
        invoke SetRegisterItemB, hDlg, r.b.rR, IDE_R
        invoke SetRegisterItemW, hDlg, r.w.IX, IDE_IX
        invoke SetRegisterItemW, hDlg, r.w.IY, IDE_IY
        call vBCdetect
        invoke SetRegisterItemB, hDlg, al, IDE_bc
        call vDEdetect
        invoke SetRegisterItemB, hDlg, al, IDE_de
        call vHLdetect
        invoke SetRegisterItemB, hDlg, al, IDE_M
        invoke SetRegisterItemW, hDlg, pc, IDE_PC
        call GetPCCmd
        invoke SetRegisterItemB, hDlg, al, IDE_pc        
        invoke SetRegisterItemW, hDlg, spw, IDE_SP
        call vSPdetect
        invoke SetRegisterItemW, hDlg, ax, IDE_sp
        mov al, r.b.rF
        and al, cSF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_sf, al
        mov al, r.b.rF
        and al, cZF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_zf, al
        mov al, r.b.rF
        and al, cHF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_hf, al
        mov al, r.b.rF
        and al, cPF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_pf, al
        mov al, r.b.rF
        and al, cNF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_nf, al
        mov al, r.b.rF
        and al, cCF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_cf, al
        mov al, r.b.rFa
        and al, cSF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_sf_, al
        mov al, r.b.rFa
        and al, cZF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_zf_, al
        mov al, r.b.rFa
        and al, cHF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_hf_, al
        mov al, r.b.rFa
        and al, cPF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_pf_, al
        mov al, r.b.rFa
        and al, cNF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_nf_, al
        mov al, r.b.rFa
        and al, cCF
        jz @F
        mov al, 1
@@:     invoke CheckDlgButton, hDlg, IDB_cf_, al
        invoke SetRegisterItemW, hDlg, BpxAddr, IDE_BPX
        ret
SetAllRegisters ENDP


DlgPortsProc proc hDlg   :DWORD,
             uMsg   :DWORD,
             wParam :DWORD,
             lParam :DWORD
      .if uMsg == WM_INITDIALOG
        invoke SetAllPorts, hDlg
      .elseif uMsg == WM_COMMAND
        mov eax, wParam
        cmp ax, IDB_p10
        jne @@p18
        invoke GetRegisterItemB, hDlg, ADDR port10, IDE_p10
        jmp @@0
        
@@p18:  cmp ax, IDB_p18
        jne @@p19
        invoke GetRegisterItemB, hDlg, ADDR port18, IDE_p18
        jmp @@0

@@p19:  cmp ax, IDB_p19
        jne @@p1A
        invoke GetRegisterItemB, hDlg, ADDR port19, IDE_p19
        jmp @@0

@@p1A:  cmp ax, IDB_p1A
        jne @@p1B
        invoke GetRegisterItemW, hDlg, ADDR port1A, IDE_p1A
        jmp @@0

@@p1B:  cmp ax, IDB_p1B
        jne @@p1C
        invoke GetRegisterItemW, hDlg, ADDR port1B, IDE_p1B
        jmp @@0

@@p1C:  cmp ax, IDB_p1C
        jne @@p80
        invoke GetRegisterItemB, hDlg, ADDR port1C, IDE_p1C
        jmp @@0

@@p80:  cmp ax, IDB_p80
        jne @@close
        invoke GetRegisterItemB, hDlg, ADDR port80, IDE_p80
        jmp @@0
@@close: cmp ax, IDB_Cancel
        jne @@0
        invoke EndDialog,hDlg,0
@@0:
      .elseif uMsg == WM_CLOSE
        invoke EndDialog,hDlg,0
      .endif
    xor eax, eax
    ret
DlgPortsProc endp

SetAllPorts PROC hDlg:DWORD
        invoke SetRegisterItemB, hDlg, port10, IDE_p10
        invoke SetRegisterItemB, hDlg, port18, IDE_p18
        invoke SetRegisterItemB, hDlg, port19, IDE_p19
        invoke SetRegisterItemW, hDlg, word ptr port1A, IDE_p1A
        invoke SetRegisterItemW, hDlg, word ptr port1B, IDE_p1B
        invoke SetRegisterItemB, hDlg, port1C, IDE_p1C
        invoke SetRegisterItemB, hDlg, port80, IDE_p80
        invoke wsprintf, ADDR string, ADDR fmt, pFDD
        invoke SetDlgItemText, hDlg, IDE_FDDpointer, ADDR string  
        invoke SetRegisterItemB, hDlg, DriveEx, IDE_DrvEx
        ret
SetAllPorts ENDP

DlgSaveCom proc hDlg   :DWORD,
             uMsg   :DWORD,
             wParam :DWORD,
             lParam :DWORD
LOCAL first_addr:WORD
LOCAL last_addr:WORD            
      .if uMsg == WM_INITDIALOG
        invoke SetRegisterItemW, hDlg, first_addr, IDE_B
        invoke SetRegisterItemW, hDlg, last_addr, IDE_E
      .elseif uMsg == WM_COMMAND
        mov eax, wParam
        cmp ax, IDB_DE
        jne @CloseDlg
        invoke GetRegisterItemW, hDlg, ADDR first_addr, IDE_B
        invoke GetRegisterItemW, hDlg, ADDR last_addr, IDE_E
    		invoke FileSaveDlg, hDlg, ADDR szFileName, ADDR szTitleName
    		.if !eax
		        invoke MessageBox, hDlg, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
    		    xor eax, eax
    		    ret
    		.endif
		    lea ebx, ram
		    movzx eax, first_addr
		    add ebx, eax
		    mov ax, last_addr
		    sub ax, first_addr
		    movzx eax, ax
    		invoke  SaveImage, ADDR szFileName, ebx, eax	; Save memory range as COM
    		.if !eax
		        invoke MessageBox, hDlg, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
    		.endif
        invoke EndDialog,hDlg,0
        xor eax, eax
        ret
@CloseDlg:
      .elseif uMsg == WM_CLOSE
        invoke EndDialog,hDlg,0
      .endif
    xor eax, eax
    ret
DlgSaveCom endp


DlgRom proc hDlg   :DWORD,
             uMsg   :DWORD,
             wParam :DWORD,
             lParam :DWORD
LOCAL first_addr:WORD
LOCAL last_addr:WORD            
      .if uMsg == WM_INITDIALOG
        invoke SetRegisterItemW, hDlg, first_addr, IDE_B
        invoke SetRegisterItemW, hDlg, last_addr, IDE_E
      .elseif uMsg == WM_COMMAND
        mov eax, wParam
        cmp ax, IDB_DE
        jne @CloseRomDlg
        invoke GetRegisterItemW, hDlg, ADDR first_addr, IDE_B
        invoke GetRegisterItemW, hDlg, ADDR last_addr, IDE_E
	    lea ebx, ram
	    movzx eax, first_addr
	    add ebx, eax
	    mov cx, last_addr
	    sub cx, first_addr
	    shr cx, 2
	    lea edx, rom
@@:	    mov eax, [ebx]
        mov [edx], eax
        inc ebx
        inc ebx
        inc ebx
        inc ebx
        inc edx
        inc edx
        inc edx
        inc edx
        dec cx
        jnz @B
        invoke EndDialog,hDlg,0
        xor eax, eax
        ret
@CloseRomDlg:
      .elseif uMsg == WM_CLOSE
        invoke EndDialog,hDlg,0
      .endif
    xor eax, eax
    ret
DlgRom endp

