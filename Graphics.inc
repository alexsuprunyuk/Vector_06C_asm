.data?
RGB_Table dd 16 dup (?)
portC_prev db 16 dup(?)
ColorChanged db ?
b8bytes db 8 dup(?)

BGR_RGBI MACRO
    xor eax, eax
    mov al, [ebx]; код цвета в формате вектора
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
@@: ; теперь eax содержит код цвета в формате RGBI
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
    cmp [ebx], eax ; изменился ли цвет в таблице цветов ВЕКТОРА?
    jne @@0
    mov eax, 4
    add esi, eax
    add ebx, eax
    loop @B
    jmp @@1; нет изменений
@@0: mov ColorChanged, 1; цвет поменяли
    ;нужно сохранить изменения
    lea esi, portC
    lea ebx, portC_prev
    mov ecx, 4
@@: mov eax, [esi]
    mov [ebx], eax
    mov eax, 4
    add esi, eax
    add ebx, eax
    loop @B
    ;конвертировать цвета в формат TrueColor
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
; eax - Адрес выводимого спрайта в формате вектора 8000h-FFFFh
    push ebx
    push ecx
    push esi
    and eax, 1FFFh
    or ah, 0E0h
    lea ebx, ram
    add ebx, eax
    mov ah, [ebx]; прочитать байт из плоскости E000-FFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    mov [esi], al; очистка памяти и установка битов при необходимости
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    sub ebx, eax
    mov ah, [ebx]; прочитать байт из плоскости C000-DFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 1
    or [esi], al; установка битов при необходимости
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    sub ebx, eax
    mov ah, [ebx]; прочитать байт из плоскости A000-BFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 2
    or [esi], al; установка битов при необходимости
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    sub ebx, eax
    mov ah, [ebx]; прочитать байт из плоскости 8000-9FFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 3
    or [esi], al; установка битов при необходимости
    dec esi
    shr ah, 1
    loop @B
    ;структура b8bytes содержит значения 8 пикселов
    ;реализация перехода от координат вектора к координатам DIBitmap
    lea eax, ram
    sub ebx, eax
    and ebx, 1FFFh
    movzx eax, bl
    shl eax, 12 ;номер строки *1024 (32 столбца по 8 пикселов по 4 байта на пиксел)*2*2
    movzx ebx, bh
    shl ebx, 6 ;номер столбца *32 (8 пикселов по 4 байта на пиксел)*2
    add ebx, eax
    mov eax, pBufferB
    and eax, eax
    jz @@0; если указатель уже закрыт то не заполнять массив DIBitmap
    add ebx, eax
    ;указатель на данные DIBitmap получен
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
; eax - Адрес выводимого спрайта в формате вектора 8000h-FFFFh
    push ebx
    push ecx
    push esi
    and eax, 9FFFh
    lea ebx, ram
    add ebx, eax
    mov ah, [ebx]; прочитать байт из плоскости 8000-9FFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 3
    mov [esi], al; очистка памяти и установка битов при необходимости
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    add ebx, eax
    mov ah, [ebx]; прочитать байт из плоскости A000-BFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 2
    or [esi], al; установка битов при необходимости
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    add ebx, eax
    mov ah, [ebx]; прочитать байт из плоскости C000-DFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 5
    or [esi], al; установка битов при необходимости
    dec esi
    shr ah, 1
    loop @B
    mov eax, 2000h
    add ebx, eax
    mov ah, [ebx]; прочитать байт из плоскости E000-FFFFh
    lea esi, b8bytes[7]
    mov ecx, 8
@@: mov al, ah
    and al, 1
    shl al, 4
    or [esi], al; установка битов при необходимости
    dec esi
    shr ah, 1
    loop @B
    ;структура b8bytes содержит значения 16 пикселов
    ;реализация перехода от координат вектора к координатам DIBitmap
    lea eax, ram
    sub ebx, eax
    and ebx, 1FFFh
    movzx eax, bl
    shl eax, 12 ;номер строки *1024 (32 столбца по 8 пикселов по 4 байта на пиксел)*2*2
    movzx ebx, bh
    shl ebx, 6 ;номер столбца *32 (8 пикселов по 4 байта на пиксел)*2
    add ebx, eax
    mov eax, pBufferB
    and eax, eax
    jz @@0; если указатель уже закрыт то не заполнять массив DIBitmap
    add ebx, eax
    ;указатель на данные DIBitmap получен
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
    ;реализация перехода к координатам DIBitmap
    movzx ebx, bBorderRow ; строка в которую выводится байт бордюра
    shl ebx, 5 ;номер строки *32 (4 пиксела по 4 байта на пиксел)*2
    mov eax, pBBufferB
    and eax, eax
    jz @@0; если указатель уже закрыт то не заполнять массив DIBitmap
    add ebx, eax
    ;указатель на данные DIBitmap получен
    lea edi, RGB_Table
    mov al, port2[8]; цвет бордюра
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
