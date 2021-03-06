.data?
RGB_Table dd 16 dup (?)
portC_prev db 16 dup(?)
ColorChanged db ?
b8bytes db 8 dup(?)

BGR_RGBI MACRO
    xor eax, eax
    mov al, [ebx]; ��� ����� � ������� �������
    and al, 7h  ;red
    shl al, 5
    rol eax, 8    
    mov al, [ebx]
    and al, 38h ;green
    shl al, 2
    rol eax, 8
    mov al, [ebx]
    and al, 0C0h ;blue
    jz @F
    or al, 20h
@@: ; ������ eax �������� ��� ����� � ������� RGBI
    ENDM
    
.code
CompareColorTables PROC
    push ebx
    push ecx
    push esi
    lea esi, portC
    lea ebx, portC_prev
    mov ecx, 4
@@: mov eax, [esi]
    cmp [ebx], eax ; ��������� �� ���� � ������� ������ �������?
    jne @@0
    mov eax, 4
    add esi, eax
    add ebx, eax
    loop @B
    jmp @@1; ��� ���������
@@0: mov ColorChanged, 1; ���� ��������
    ;����� ��������� ���������
    lea esi, portC
    lea ebx, portC_prev
    mov ecx, 4
@@: mov eax, [esi]
    mov [ebx], eax
    mov eax, 4
    add esi, eax
    add ebx, eax
    loop @B
    ;�������������� ����� � ������ TrueColor
    lea ebx, portC
    lea esi, RGB_Table
    mov ecx, 16
@@2: BGR_RGBI
    mov [esi], eax
    inc ebx
    inc esi    
    inc esi    
    inc esi    
    inc esi
    loop @@2    
@@1: pop esi
    pop ecx
    pop ebx
    ret       
CompareColorTables ENDP

PixelToMem16 PROC
; eax - ����� ���������� ������� � ������� ������� 8000h-FFFFh
    push ebx
    push ecx
    push esi
    and eax, 1FFFh
    or ah, 0E0h
    lea ebx, ram
    add ebx, eax
    mov ah, [ebx]; ��������� ���� �� ��������� E000-FFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    mov [esi], al; ������� ������ � ��������� ����� ��� �������������
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    sub ebx, eax
    mov ah, [ebx]; ��������� ���� �� ��������� C000-DFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 1
    or [esi], al; ��������� ����� ��� �������������
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    sub ebx, eax
    mov ah, [ebx]; ��������� ���� �� ��������� A000-BFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 2
    or [esi], al; ��������� ����� ��� �������������
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    sub ebx, eax
    mov ah, [ebx]; ��������� ���� �� ��������� 8000-9FFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 3
    or [esi], al; ��������� ����� ��� �������������
    dec esi
    shr ah, 1
    loop @B
    ;��������� b8bytes �������� �������� 8 ��������
    ;���������� �������� �� ��������� ������� � ����������� DIBitmap
    lea eax, ram
    sub ebx, eax
    and ebx, 1FFFh
    movzx eax, bl
    shl eax, 12 ;����� ������ *1024 (32 ������� �� 8 �������� �� 4 ����� �� ������)*2*2
    movzx ebx, bh
    shl ebx, 6 ;����� ������� *32 (8 �������� �� 4 ����� �� ������)*2
    add ebx, eax
    mov eax, pBufferB
    and eax, eax
    jz @@0; ���� ��������� ��� ������ �� �� ��������� ������ DIBitmap
    add ebx, eax
    ;��������� �� ������ DIBitmap �������
    mov ecx, 8
    push edi
    lea edi, RGB_Table
    lea esi, b8bytes
@@: mov al, [esi]
    movzx eax, al
    shl eax, 2
    add eax, edi
    mov eax, [eax]
    mov [ebx], eax
    mov [ebx+4], eax
    add ebx, 2048
    mov [ebx], eax
    mov [ebx+4], eax
    sub ebx, 2040
    inc esi
    loop @B
    pop edi
@@0: pop esi
    pop ecx
    pop ebx
ret
PixelToMem16 ENDP

PixelToMem4 PROC
; eax - ����� ���������� ������� � ������� ������� 8000h-FFFFh
    push ebx
    push ecx
    push esi
    and eax, 9FFFh
    lea ebx, ram
    add ebx, eax
    mov ah, [ebx]; ��������� ���� �� ��������� 8000-9FFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 3
    mov [esi], al; ������� ������ � ��������� ����� ��� �������������
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    add ebx, eax
    mov ah, [ebx]; ��������� ���� �� ��������� A000-BFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 2
    or [esi], al; ��������� ����� ��� �������������
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    add ebx, eax
    mov ah, [ebx]; ��������� ���� �� ��������� C000-DFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 5
    or [esi], al; ��������� ����� ��� �������������
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    add ebx, eax
    mov ah, [ebx]; ��������� ���� �� ��������� E000-FFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 4
    or [esi], al; ��������� ����� ��� �������������
    dec esi
    shr ah, 1
    loop @B
    ;��������� b8bytes �������� �������� 16 ��������
    ;���������� �������� �� ��������� ������� � ����������� DIBitmap
    lea eax, ram
    sub ebx, eax
    and ebx, 1FFFh
    movzx eax, bl
    shl eax, 12 ;����� ������ *1024 (32 ������� �� 8 �������� �� 4 ����� �� ������)*2*2
    movzx ebx, bh
    shl ebx, 6 ;����� ������� *32 (8 �������� �� 4 ����� �� ������)*2
    add ebx, eax
    mov eax, pBufferB
    and eax, eax
    jz @@0; ���� ��������� ��� ������ �� �� ��������� ������ DIBitmap
    add ebx, eax
    ;��������� �� ������ DIBitmap �������
    mov ecx, 8
    push edi
    lea edi, RGB_Table
    lea esi, b8bytes
@@: mov al, [esi]
    shr al, 4
    movzx eax, al
    shl eax, 2
    add eax, edi
    mov eax, [eax]
    mov [ebx], eax
    add ebx, 2048
    mov [ebx], eax
    inc ebx
    inc ebx
    inc ebx
    inc ebx
    mov al, [esi]
    and al, 0Fh
    movzx eax, al
    shl eax, 2
    add eax, edi
    mov eax, [eax]
    mov [ebx], eax
    sub ebx, 2048
    mov [ebx], eax
    inc ebx
    inc ebx
    inc ebx
    inc ebx
    inc esi
    loop @B
    pop edi
@@0: pop esi
    pop ecx
    pop ebx
ret
PixelToMem4 ENDP

SetBorderPixel PROC
    push ebx
    push ecx
    push edi
    ;���������� �������� � ����������� DIBitmap
    movzx ebx, bBorderRow ; ������ � ������� ��������� ���� �������
    shl ebx, 5 ;����� ������ *32 (4 ������� �� 4 ����� �� ������)*2
    mov eax, pBBufferB
    and eax, eax
    jz @@0; ���� ��������� ��� ������ �� �� ��������� ������ DIBitmap
    add ebx, eax
    ;��������� �� ������ DIBitmap �������
    lea edi, RGB_Table
    mov al, port2[8]; ���� �������
    and al, 15
    movzx eax, al
    shl eax, 2
    add eax, edi
    mov eax, [eax]
    mov edi, 4
    mov cl, 8
@@: mov [ebx], eax
    add ebx, edi
    dec cl
    jnz @B
@@0: 
    pop edi
    pop ecx
    pop ebx
ret
SetBorderPixel ENDP
