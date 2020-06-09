.data
KeyTbl dw 0FFh, 1F7h, 1EFh, 1DFh, 1BFh, 17Fh, 0FFh, 0FFh ;00h, F1,F2,F3,F4,F5, 06h, 07h
       dw 0F7h, 0FEh, 0FDh, 0FFh, 1FEh, 0FBh, 0FFh, 0FFh ;BS,Tab,(RShift)ПС,0Bh, Home, Enter(ВК), 0Eh, 0Fh
       dw 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh ;10-17
       dw 0DFh, 07Fh, 0FFh, 1FBh, 0FFh, 0EFh, 0BFh, 1FDh ;Up, Down, 1Ah, AP2, 1Ch, Left, Right, PgUp(СТР)
       dw 77Fh, 2FDh, 2FBh, 2F7h, 2EFh, 2DFh, 2BFh, 27Fh ; !"#$%&'
       dw 3FEh, 3FDh, 3FBh, 3F7h, 3EFh, 3DFh, 3BFh, 37Fh ; ()*+,-./
       dw 2FEh, 2FDh, 2FBh, 2F7h, 2EFh, 2DFh, 2BFh, 27Fh ; 01234567
       dw 3FEh, 3FDh, 3F7h, 3FBh, 3EFh, 3DFh, 3BFh, 37Fh ; 89:;<=>?
       dw 4FEh, 4FDh, 4FBh, 4F7h, 4EFh, 4DFh, 4BFh, 47Fh ; @ABCDEFG
       dw 5FEh, 5FDh, 5FBh, 5F7h, 5EFh, 5DFh, 5BFh, 57Fh ; HIJKLMNO
       dw 6FEh, 6FDh, 6FBh, 6F7h, 6EFh, 6DFh, 6BFh, 67Fh ; PQRSTUVW
       dw 7FEh, 7FDh, 7FBh, 7F7h, 7EFh, 7DFh, 7BFh, 77Fh ; XYZ[\]^_
EncTbl db "\B@VH:FISWUAPR[OLDXT]ZJKYEGMC^NQ"

.code
KeyDownToChar PROC
 push dx
 cmp ax, 36h; Rshift нажата ?
 mov dl, 0Ah; ПС
 je @@0
 cmp ax, 3Bh; F1<<F5
 jb @@2 ; остальные комбинации с меньшим кодом были просмотрены в сообщении WM_CHAR
 cmp ax, 40h;
 jae @@1
 ;F1-F5 нажата
 sub al, 3Ah
 mov dl,al
 jmp @@0
@@1: mov dl, 0Ch
 cmp al, 47h; Home
 je @@0
 mov dl, 1Fh
 cmp al, 49h; PgUp
 je @@0
 mov dl, 18h
 cmp al, 48h; Up
 je @@0
 inc dl
 cmp al, 50h; Dn
 je @@0
 mov dl, 1Dh
 cmp al, 4Bh; Left
 je @@0
 inc dl
 cmp al, 4Dh; Right
 je @@0
@@2: xor dl,dl 
@@0: movzx eax,dl
 pop dx
 ret   
KeyDownToChar ENDP

QWEtoJCU PROC
 push esi
 lea esi,EncTbl
 cmp al, 27h
 je @@0
 cmp al, 22h
 je @@0
 inc esi
 cmp al, ','
 je @@0
 cmp al, '<'
 je @@0
 inc esi
 cmp al, '.'
 je @@0
 cmp al, '>'
 je @@0
 inc esi
 cmp al, ';'
 je @@0
 cmp al, ':'
 je @@0
 inc esi
 cmp al, '['
 je @@0
 cmp al, '{'
 je @@0
 inc esi
 cmp al, ']'
 je @@0
 cmp al, '}'
 je @@0
 inc esi
 cmp al, 40h ; если меньше или равно @ то выход без перекодировки
 jbe @@1;
 sub al, 41h
 add esi, eax
@@0: mov al, [esi] 
@@1: pop esi
 ret
QWEtoJCU ENDP 

KeyToPort2 PROC
    push esi
    push ebx
    lea esi,KeyTbl
    cmp al, 60h
    jb @F
    sub al, 20h; для символов {|}~¦
@@: movzx eax, al
    rol eax,1
    add esi, eax
    mov ax, [esi]; получаем код столбца и байта состояния
    movzx ebx, ah
    lea esi, port2
    add esi, ebx
    and [esi], al; помещаем код сканирования в port2
@@: pop ebx
    pop esi
    ret
KeyToPort2 ENDP