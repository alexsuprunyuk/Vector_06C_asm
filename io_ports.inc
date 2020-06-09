CalcDiskAddr PROC
        cmp port19, 5
        jbe @SectorOK
        mov port19, 1
@SectorOK:        
        movzx eax, port1A
        mov ebx, eax
        shl ebx, 3
        shl eax, 1
        add ebx, eax
        mov al, port1C
        and al, 4
        jz @Side0
        inc al
@Side0: xor al, 5
        add al, port19
        dec al
        movzx eax, al
        add ebx, eax
        shl ebx, 10
        mov ax, SectIndx
        movzx eax, ax
        add ebx, eax
        inc SectIndx
        and SectIndx, 3FFh
        jnz @CalcReturn
        test port1B, 10h
        jz @ResetStatusBits
        inc port19
        cmp port19, 5
        jbe @CalcReturn
        or port1B[1], 10h
@ResetStatusBits:
        mov DriveEx, 0
@CalcReturn:
        ret        
CalcDiskAddr ENDP

OutPortProc PROC
        and ah, ah
        jnz @Oport1
        lea esi, port0
@@:     mov [esi], al
        ret
 
@Oport1: cmp ah, 1
        jne @Oport2
        and al, 0Fh
        lea esi, port1
        mov ah, [esi]
        and ah, 0F0h
        or al, ah
        jmp @B
 
@Oport2: cmp ah, 2
        jne @Oport3
        lea esi, port2[8]
        jmp @B
 
@Oport3: cmp ah, 3
        jne @Oport8
        lea esi, port3
        test port0, 2
        jnz @B
        inc esi
        jmp @B

@Oport8: cmp ah, 8
        jne @Oport9
        lea esi, port8
        inc esi
        mov ah, al
        and ah, 0F0h
        cmp ah, 30h
        jne @Oport8_A
        and byte ptr[esi],0FEh
        mov portB, 0
        dec esi
        jmp @B
@Oport8_A: cmp ah, 70h
        jne @Oport8_9
        and byte ptr[esi],0FDh
        mov portA, 0
        dec esi
        jmp @B
@Oport8_9: cmp ah, 0B0h
        jne @B
        and byte ptr[esi],0FBh
        mov port9, 0
        dec esi
        jmp @B

@Oport9: cmp ah, 9
        jne @OportA
        lea esi, port9
        xor byte ptr[port8+1],1
        test [port8+1],1
        jnz @B
        inc esi
		mov [esi], al
		dec esi
		mov ax,[esi]
		shl ax, 1
		mov p9cnt, ax
        ret

@OportA: cmp ah, 0Ah
        jne @OportB
        lea esi, portA
        xor byte ptr[port8+1],2
        test [port8+1],2
        jnz @B
        inc esi
		mov [esi], al
		dec esi
		mov ax,[esi]
		shl ax, 1
		mov pAcnt, ax
        ret

        
@OportB: cmp ah, 0Bh
        jne @OportC
        lea esi, portB
        xor byte ptr[port8+1],4
        test [port8+1],4
        jnz @B
        inc esi
		mov [esi], al
		dec esi
		mov ax,[esi]
		shl ax, 1
		mov pBcnt, ax
        ret

@OportC: cmp ah, 0Ch
        jne @OportD
@OportCa: lea esi, portC
        movzx ebx, port2[8]
        add esi, ebx
        jmp @B
@OportD: cmp ah, 0Dh
        je @OportCa
@OportE: cmp ah, 0Eh
        je @OportCa
@OportF: cmp ah, 0Fh
        je @OportCa

@Oport10: cmp ah, 10h
        jne @Oport18
        lea esi, port10
        jmp @B
        
@Oport18: cmp ah, 18h
        jne @Oport19
        lea esi, port18
        mov [esi], al
        cmp DriveEx,0
        jz @B; будет выполняться команда поиска
        test port1B[1], 1
        jz @B; команда еще не выполняется
        mov ah, port1B
        and ah, 10100000b
        cmp ah, 10100000b
        jne @B; это не команда записи секторов
        call CalcDiskAddr                
        mov al, port18
        cmp pFDD, 0
        jne @WrMemAddr; диск вставлен запись возможна
        or port1B[1], 80h
        jmp @B
@WrMemAddr: 
        and port1B[1], 7Fh
        mov esi, pFDD
        add esi, ebx
        jmp @B
        
@Oport19: cmp ah, 19h               
        jne @Oport1A
        lea esi, port19
        jmp @B
        
@Oport1A: cmp ah, 1Ah               
        jne @Oport1B
        lea esi, port1A
        cmp al, 80
        jb @TrackAllowed
        xor al, al
@TrackAllowed:        
        mov [esi], al
        inc esi
        jmp @B
        
@Oport1B: cmp ah, 1Bh               
        jne @Oport1C
        mov SectIndx, 0
        mov DriveEx, 255
        mov DrvExCnt, 8192
        and port1B[1], 11111000b
        mov ah, al
        and ah, 0F0h
        jnz @NonPark
        mov word ptr port1A, 0
        jmp @Op1Bcom
@NonPark:
        cmp ah, 10h
        jne @NonSeek
        mov ah, port18
        cmp ah, 80
        jb @TrackAllowedForSeek
        xor ah, ah
@TrackAllowedForSeek:        
        mov port1A, ah
        mov port1A[1], ah
        jmp @Op1Bcom
@NonSeek:
        and ah, 0E0h
        cmp ah, 60h
        jne @NonStepB
        mov stepD, 255
        jmp @stepComm
@NonStepB:
        cmp ah, 40h
        jne @NonStepF
        mov stepD, 1
        jmp @stepComm
@NonStepF:
        cmp ah, 20h
        jne @Op1Bcom
@stepComm:
        mov ah, port1A
        add ah, stepD
        cmp ah, 80
        jb @StepAllowed
        xor ah, ah
@StepAllowed:        
        mov port1A, ah
        test al, 10h
        jz @Op1Bcom
        mov port1A[1], ah                                
@Op1Bcom:
        lea esi, port1B
        jmp @B
        
@Oport1C: cmp ah, 1Ch
        jne @Oport80
        mov SectIndx, 0
        mov DriveEx, 255
        mov DrvExCnt, 8192
        and word ptr port1B, 1111110001111111b
        lea esi, port1C
        jmp @B

@Oport80: cmp ah, 80h
        jne @F
        mov port80[1], al
        and al, 0F9h
        mov ah, port80
        and ah, 6
        or al, ah
        mov port80, al
@@:     ret
OutPortProc ENDP

InPortProc PROC
        cmp ah, 0FEh
        jne @Iport18
        mov port3, al
        mov ah, 2

@Iport18: cmp ah, 18h
        jne @Iport19
        mov al, 255
        cmp DriveEx,0
        jz @F; команда уже завершилась
        test port1B[1], 1
        jz @F; команда еще не выполняется
        mov ah, port1B
        and ah, 10100000b
        cmp ah, 10000000b
        jne @F; это не команда чтения
        call CalcDiskAddr                
        cmp pFDD, 0
        jne @RdMemAddr; диск вставлен чтение возможно
        or port1B[1], 80h
        jmp @F
@RdMemAddr: 
        and port1B[1], 7Fh
        add ebx, pFDD
        mov al, [ebx]
        jmp @F

@Iport19: cmp ah, 19h
        jne @Iport1A
        mov al, port19
        jmp @F

@Iport1A: cmp ah, 1Ah
        jne @Iport1B
        mov al, port1A[1]
        jmp @F

@Iport1B: cmp ah, 1Bh
        jne @Iport1
        and port1B[1], 11111011b
        mov ah, port1B
        test ah, 80h
        jnz @NoTrackCheck
        cmp port1A, 0
        jnz @NoTrackCheck
        or port1B[1], 4
@NoTrackCheck:
        cmp DrvExCnt, 0        
        jz @Ip1Bstep3
        dec DrvExCnt
        cmp DriveEx, 255
        jne @Ip1Bstep2
        shr DriveEx, 1
        jmp @Ip1Bstep4
@Ip1Bstep2:
        or port1B[1], 1; команда начала выполняться        
        test ah, 80h
        jz @notRW; это не команда чтения/записи
        or port1B[1], 3; установить запрос данных+выполнение
        cmp DriveEx, 0
        jnz @Ip1Bstep4
@Ip1Bstep3:
        and port1B[1], 10010100b ; команда завершена       
@Ip1Bstep4: 
        mov al, port1B[1]
        jmp @F
@notRW: cmp DriveEx, 0
        jz @Ip1Bstep3
        shr DriveEx, 1
        jmp @Ip1Bstep4
        
@Iport1:
        cmp ah, 1
        jne @Iport2
        mov ah, r.b.rR
        and ah, 10h
        mov al, port1
        and al, 0EFh
        or al, ah 
        jmp @F
@Iport2:
        mov al, 255; заглушка
        cmp ah, 2
        jne @F
        mov dl, port3
        lea ebx, port2
        and dl, dl
        jnz @Iport2nz
        mov cl, 7
        mov al, [ebx]
        inc ebx
@p2loop: and al, [ebx]
        inc ebx
        dec cl
        jnz @p2loop
        jmp @F
@Iport2nz:
        mov cl, 8
@po2loop:
        shr dl, 1
        jnc @Ip2e
        inc ebx
        dec cl
        jnz @po2loop
        mov al, 255
        jmp @F
@Ip2e:  mov al, [ebx]
@@:     mov [esi], al
        ret
InPortProc ENDP
