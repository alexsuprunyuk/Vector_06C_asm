.586
.model flat,stdcall

option casemap:none
include \masm32\include\windows.inc
include \masm32\include\user32.inc
include \masm32\include\kernel32.inc
include \masm32\include\comctl32.inc
include \masm32\include\comdlg32.inc
include \masm32\include\gdi32.inc
include \masm32\include\masm32.inc
includelib \masm32\lib\user32.lib
includelib \masm32\lib\kernel32.lib
includelib \masm32\lib\comctl32.lib
includelib \masm32\lib\comdlg32.lib
includelib \masm32\lib\gdi32.lib
includelib \masm32\lib\masm32.lib

WinMain PROTO :DWORD,:DWORD,:DWORD,:DWORD
DlgRegsProc PROTO :DWORD,:DWORD,:DWORD,:DWORD
DlgPortsProc PROTO :DWORD,:DWORD,:DWORD,:DWORD
DlgSaveCom PROTO :DWORD,:DWORD,:DWORD,:DWORD
DlgRom PROTO :DWORD,:DWORD,:DWORD,:DWORD
FileInitialize PROTO :DWORD
FileOpenDlg	  PROTO :DWORD, :DWORD, :DWORD
FileSaveDlg	  PROTO :DWORD, :DWORD, :DWORD
LoadFile	  PROTO :DWORD, :DWORD
SaveImage	PROTO :DWORD, :DWORD, :DWORD

.data
ALIGN 8
ClassName db "Vector 06C Z80",0
AppName  db "Vector 06C",0
MenuName db "VectorMenu",0
IconName db "VectorIcon",0
CursorName db "VectorCursor",0
DialogName db "Registers", 0
PortsDialogName db "Ports", 0
RomDialogName db "SaveAddrs", 0
CantOpenFile db "Невозможно открыть файл",0
CantSaveFile db "Невозможно сохранить файл",0
char db 128 dup (32), 0

.data?
ALIGN 8
PUBLIC	dwFileSize
hInstance	dd ?
CommandLine dd ?
EncNeed db ? ; признак необходимости перекодировки QWERTY-JCUKENG
CmdLength db ? ;количество ВЕКТОРА циклов на выполнение команды
IntCnt db ? ;счетчик прерываний 50Hz, используется для вывода изображения на экран
hBuffer dd ? ;хэндл битмапа
pBuffer dd ? ;указатель на заголовок битмапа
pBufferB dd ? ;указатель на данные видеоряда DIBitmap
hBBuffer dd ? ;хэндл битмапа бордюра
pBBuffer dd ? ;указатель на заголовок битмапа бордюра
pBBufferB dd ? ;указатель на данные видеоряда DIBitmap бордюра
szFileName	db	MAX_PATH DUP (?)
szTitleName	db	MAX_PATH DUP (?)
hFDD    dd ? ;хэндл образа диска
pFDD    dd ? ;указатель на образ диска
dwFileSize dd ?;размер считанного файла

include vector06c.inc

.code
include Time.inc
include z80cmd.inc


start:
      mov sndPtr, 0
      lea eax, sndbuf1
      mov sndbufPtr, eax
    GetTimeConstants
	call ResetPC
    invoke GetModuleHandle, NULL
	mov    hInstance,eax
	invoke GetCommandLine
	mov CommandLine,eax
	invoke InitCommonControls
    mov pBufferB, 0
    mov pBBufferB, 0
    mov pFDD, 0
	invoke WinMain, hInstance,NULL,CommandLine, SW_SHOWDEFAULT
	invoke ExitProcess,eax
;////////////////////////////////////////////////////////////
WinMain proc hInst:HINSTANCE,hPrevInst:HINSTANCE,CmdLine:LPSTR,
CmdShow:DWORD
	LOCAL wc:WNDCLASSEX
	LOCAL msg:MSG
	LOCAL hwnd:HWND
	LOCAL xSize:DWORD
	LOCAL ySize:DWORD
	invoke GetSystemMetrics, SM_CXDLGFRAME
	shl eax, 1
	add eax, 536
	mov xSize, eax
	invoke GetSystemMetrics, SM_CXBORDER
	shl eax, 1
	add eax, xSize
	mov xSize, eax
	invoke GetSystemMetrics, SM_CYDLGFRAME
	shl eax, 1
	add eax, 512
	mov ySize, eax
	invoke GetSystemMetrics, SM_CYBORDER
	shl eax, 1
	add eax, ySize
	mov ySize, eax
	invoke GetSystemMetrics, SM_CYMENU
	add eax, ySize
	mov ySize, eax
	invoke GetSystemMetrics, SM_CYCAPTION
	add eax, ySize
	mov ySize, eax
	mov   wc.cbSize,SIZEOF WNDCLASSEX
	mov   wc.style, CS_NOCLOSE
	mov   wc.lpfnWndProc, OFFSET WndProc
	mov   wc.cbClsExtra,NULL
	mov   wc.cbWndExtra,NULL
	push  hInst
	pop   wc.hInstance
    invoke GetStockObject, BLACK_BRUSH
	mov   wc.hbrBackground,eax
	mov   wc.lpszMenuName,offset MenuName
	mov   wc.lpszClassName,OFFSET ClassName
	invoke LoadIcon,hInst,offset IconName
	mov   wc.hIcon,eax
	mov   wc.hIconSm,eax
	invoke LoadCursor,hInst,offset CursorName
	mov   wc.hCursor,eax
	invoke RegisterClassEx, addr wc
	INVOKE CreateWindowEx,NULL,ADDR ClassName,ADDR AppName,\
           WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,\
           CW_USEDEFAULT,xSize,ySize,NULL,NULL,\
           hInst,NULL
	mov   hwnd,eax
	INVOKE ShowWindow, hwnd,SW_SHOWNORMAL
	INVOKE UpdateWindow, hwnd
	push edi
	push esi
	push ecx
	        lea edi, rom
	        lea esi, MyRom
	        mov ecx, 512
			rep movsd [edi],[esi]
	pop ecx
	pop esi
	pop edi
StartLoop:		; Ожидаем сообщение, выполняем свои процедуры
    Cycle4MHz
    Cycle22kHz
    Cycle50Hz hwnd
	invoke	PeekMessage,ADDR msg,0,0,0,PM_NOREMOVE
	or	eax,eax
	jz	StartLoop
	invoke	GetMessage,ADDR msg,NULL,0,0
	or	eax,eax
	jz	ExitLoop
	invoke	TranslateMessage,ADDR msg
	invoke	DispatchMessage,ADDR msg
	jmp	StartLoop
ExitLoop:	mov	eax,msg.wParam
			ret
WinMain endp
;///////////////////////////////////////////////////
WndProc proc hWnd:HWND, uMsg:UINT, wParam:WPARAM, lParam:LPARAM
	LOCAL hdc:HDC
	.IF uMsg==WM_DESTROY
        invoke GlobalUnlock, pBuffer
	    mov pBufferB, 0
        invoke GlobalFree, hBuffer
        invoke GlobalUnlock, pBBuffer
	    mov pBBufferB, 0
        invoke GlobalFree, hBBuffer
        cmp pFDD, 0
        je @F
        invoke GlobalUnlock, pFDD
	    mov pFDD, 0
        invoke GlobalFree, hFDD
@@:		invoke PostQuitMessage,NULL
	.ELSEIF uMsg==WM_CREATE
	    ;заполнение файловой структуры
	    invoke FileInitialize, hWnd
	    ;создание буфера для видеопамяти
	    mov eax, 100000h; 512x512x4bytes per pixel
	    add eax, sizeof(BITMAPINFOHEADER)
	    invoke GlobalAlloc, GMEM_MOVEABLE or GMEM_ZEROINIT, eax
	    mov hBuffer, eax
	    invoke GlobalLock, hBuffer
	    mov pBuffer, eax
	    mov edx, pBuffer
        mov (BITMAPINFOHEADER ptr [edx]).biSize, sizeof(BITMAPINFOHEADER)
        mov eax, 512
        mov (BITMAPINFOHEADER ptr [edx]).biWidth, eax
        mov (BITMAPINFOHEADER ptr [edx]).biHeight, eax
        mov (BITMAPINFOHEADER ptr [edx]).biPlanes, 1
        mov (BITMAPINFOHEADER ptr [edx]).biBitCount, 32
        mov (BITMAPINFOHEADER ptr [edx]).biCompression, BI_RGB
        mov (BITMAPINFOHEADER ptr [edx]).biSizeImage, 0
        mov (BITMAPINFOHEADER ptr [edx]).biXPelsPerMeter, 0
        mov (BITMAPINFOHEADER ptr [edx]).biYPelsPerMeter, 0
        mov (BITMAPINFOHEADER ptr [edx]).biClrUsed, 0
        mov (BITMAPINFOHEADER ptr [edx]).biClrImportant, 0
        mov eax, pBuffer
        add eax, sizeof(BITMAPINFOHEADER)
        mov pBufferB, eax; указатель на биты данных
;создание буфера для бордюра
	    mov eax, 2000h; 512*4*4bytes per pixel
	    add eax, sizeof(BITMAPINFOHEADER)
	    invoke GlobalAlloc, GMEM_MOVEABLE or GMEM_ZEROINIT, eax
	    mov hBBuffer, eax
	    invoke GlobalLock, hBBuffer
	    mov pBBuffer, eax
	    mov edx, pBBuffer
        mov (BITMAPINFOHEADER ptr [edx]).biSize, sizeof(BITMAPINFOHEADER)
        mov (BITMAPINFOHEADER ptr [edx]).biWidth, 4
        mov (BITMAPINFOHEADER ptr [edx]).biHeight, 512
        mov (BITMAPINFOHEADER ptr [edx]).biPlanes, 1
        mov (BITMAPINFOHEADER ptr [edx]).biBitCount, 32
        mov (BITMAPINFOHEADER ptr [edx]).biCompression, BI_RGB
        mov (BITMAPINFOHEADER ptr [edx]).biSizeImage, 0
        mov (BITMAPINFOHEADER ptr [edx]).biXPelsPerMeter, 0
        mov (BITMAPINFOHEADER ptr [edx]).biYPelsPerMeter, 0
        mov (BITMAPINFOHEADER ptr [edx]).biClrUsed, 0
        mov (BITMAPINFOHEADER ptr [edx]).biClrImportant, 0
        mov eax, pBBuffer
        add eax, sizeof(BITMAPINFOHEADER)
        mov pBBufferB, eax; указатель на биты данных
        call OpenWaveOut
        call PrepareWave
	.ELSEIF uMsg==WM_COMMAND
        mov eax,wParam
        .if (ax == 1010); Выход
                  call StopWave
		  invoke PostMessage,hWnd,WM_CLOSE,0,0
		.elseif (ax== 1001); Загрузить COM, ROM, SAV
		    invoke FileOpenDlg, hWnd, ADDR szFileName, ADDR szTitleName; диалоговая панель открытия файлов
		    .if	!eax
            	xor eax, eax
	            ret
	        .endif
	        dec al
	        jnz @LROM
	        lea ebx, ram
	        mov ecx, 16384
	        xor eax, eax
@@:         mov [ebx], eax
            inc ebx
            inc ebx
            inc ebx
            inc ebx
            loop @B
	        invoke LoadFile, ADDR szFileName, ADDR ram[256]
		    .if	!eax
		        invoke MessageBox, hWnd, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
	        .endif
            call ResetPC
            mov blk, 1
            xor eax, eax
       		ret
@LROM:		dec al
       		jnz @LSAV
	        lea ebx, rom
	        mov ecx, 8192
	        xor eax, eax
@@:         mov [ebx], eax
            inc ebx
            inc ebx
            inc ebx
            inc ebx
            loop @B
	        invoke LoadFile, ADDR szFileName, ADDR rom
		    .if	!eax
		        invoke MessageBox, hWnd, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
	        .endif
	        mov eax, dwFileSize
	        and ax, 0FF00h
	        mov MaxRomAddr, ax
            call ResetPC
            xor eax, eax
            mov blk, al
       		ret
@LSAV:      dec al
            jnz @LFDD
            call ResetPC
            invoke LoadFile, ADDR szFileName, ADDR ram
		    .if	!eax
		        invoke MessageBox, hWnd, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
	        .endif
            mov ColorChanged, 1
            xor eax, eax
       		ret
@LFDD:      dec al
            jnz @L0000
            cmp pFDD, 0
            jne @pEx
            ;выделить память под образ диска
;    	    mov eax, 819200; 800Кбайт
    	    mov eax, 839680; 820Кбайт
    	    invoke GlobalAlloc, GMEM_MOVEABLE or GMEM_ZEROINIT, eax
    	    mov hFDD, eax
    	    invoke GlobalLock, hFDD
    	    mov pFDD, eax
@pEx:       invoke LoadFile, ADDR szFileName, pFDD
		    .if	!eax
		        invoke MessageBox, hWnd, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
	        .endif
            and port1B[1], 7Fh
            xor eax, eax
       		ret
@L0000:    	invoke LoadFile, ADDR szFileName, ADDR ram
		    .if	!eax
		        invoke MessageBox, hWnd, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
	        .endif
            call ResetPC
            mov blk, 1
            xor eax, eax
       		ret
		.elseif (ax== 1002); Загрузить без очистки памяти
		    invoke FileOpenDlg, hWnd, ADDR szFileName, ADDR szTitleName; диалоговая панель открытия файлов
		    .if	!eax
            	xor eax, eax
	            ret
	        .endif
	        invoke LoadFile, ADDR szFileName, ADDR ram[256]
		    .if	!eax
		        invoke MessageBox, hWnd, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
	        .endif
            xor eax, eax
       		ret
		.elseif (ax== 1004);  сохранить образ памяти в файл
    		invoke FileSaveDlg, hWnd, ADDR szFileName, ADDR szTitleName
    		.if !eax
		        invoke MessageBox, hWnd, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
    		    xor eax, eax
    		    ret
    		.endif
		    lea eax, vBC
		    lea ebx, ram
		    sub eax, ebx
    		invoke  SaveImage, ADDR szFileName, ADDR ram, eax	; Save memory image as SAV
    		.if !eax
		        invoke MessageBox, hWnd, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
    		.endif
   		    xor eax, eax
   		    ret
		.elseif (ax== 1005);  сохранить образ диска в файл
		    cmp pFDD, 0
		    jz @NothingSave
    		invoke FileSaveDlg, hWnd, ADDR szFileName, ADDR szTitleName
    		.if !eax
		        invoke MessageBox, hWnd, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
    		    xor eax, eax
    		    ret
    		.endif
    	    mov eax, 819200; 800Кбайт
    		invoke  SaveImage, ADDR szFileName, pFDD, eax	; Save memory image as SAV
    		.if !eax
		        invoke MessageBox, hWnd, ADDR CantOpenFile, ADDR AppName, \
					MB_ICONEXCLAMATION or MB_OK
    		.endif
@NothingSave:
   		    xor eax, eax
   		    ret
		.elseif (ax== 1006);  сохранить образ диапазон памяти в файл
            invoke DialogBoxParam,hInstance,ADDR RomDialogName,0,ADDR DlgSaveCom,0
		.elseif (ax== 2001);  переместить диапазон памяти в ROM
            invoke DialogBoxParam,hInstance,ADDR RomDialogName,0,ADDR DlgRom,0
		.elseif (ax== 2002);  переместить диапазон памяти из ROM
    	    lea ebx, rom
    	    mov cx, MaxRomAddr
    	    shr cx, 2
    	    lea edx, ram[256]
@@: 	    mov eax, [ebx]
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
		.elseif (ax== 4000);  Диалог РЕГИСТРЫ
            invoke DialogBoxParam,hInstance,ADDR DialogName,0,ADDR DlgRegsProc,0
		.elseif (ax== 4001);  Диалог ПОРТЫ
            invoke DialogBoxParam,hInstance,ADDR PortsDialogName,0,ADDR DlgPortsProc,0
        .elseif (ax== 8001); Обработчик прерываний ВЕКТОРА
;*********************************************************
;INT 50 Hz
        call CompareColorTables
        test ColorChanged, 1
        jz @F
        cmp IntCnt, 0
        jnz @F
        mov ColorChanged, 0
        mov cx, 8000h
        test port2[8], 10h
        jz @Color16
@Color4:
        push cx
        pop ax
        call PixelToMem4
        inc cx
        cmp ch, 0A0h
        jb @Color4
        jmp @F
@Color16:
        push cx
        pop ax
        call PixelToMem16
        inc cx
        cmp ch, 0A0h
        jb @Color16
	@@: inc IntCnt
	    cmp IntCnt, 3
	    jne @F
	    mov IntCnt, 0
	    invoke GetDC,hWnd
	    mov    hdc,eax
            invoke SetDIBitsToDevice, hdc, 0,0,4,512,0,0,0,512, pBBufferB, pBBuffer, DIB_RGB_COLORS
	    invoke SetDIBitsToDevice, hdc, 520,0,4,512,0,0,0,512, pBBufferB, pBBuffer, DIB_RGB_COLORS
            invoke SetDIBitsToDevice, hdc, 4,0,4,512,0,0,0,512, pBBufferB, pBBuffer, DIB_RGB_COLORS
	    invoke SetDIBitsToDevice, hdc, 524,0,4,512,0,0,0,512, pBBufferB, pBBuffer, DIB_RGB_COLORS
	    mov al, port3[1]
	    cmp al, 255
	    jne @partsImg
	    invoke SetDIBitsToDevice, hdc, 8,0,512,512,0,0,0,512, pBufferB, pBuffer, DIB_RGB_COLORS
	    jmp @CloseGraph
@partsImg:
        inc al
	    xor ah, ah
	    shl ax, 1
	    movzx ebx, ax
	    mov ecx, 512
	    sub ecx, ebx
	    push ebx
	    invoke SetDIBitsToDevice, hdc, 8,ebx,512,ecx,0,ebx,0,512, pBufferB, pBuffer, DIB_RGB_COLORS
	    pop ebx
	    invoke SetDIBitsToDevice, hdc, 8,0,512,ebx,0,0,0,ebx, pBufferB, pBuffer, DIB_RGB_COLORS
@CloseGraph:
	    invoke ReleaseDC,hWnd,hdc;test
    @@: test iif, 03h; прерывания разрешены ?
        jz @@NoInt; нет
        and iif, 0F0h; обработка прерывания началась, запретить последующие прерывания
        sub spw, 02h
        mov dx, 0038h; для режимов 0 и 1 адрес прерывания фиксирован 0038h
        cmp imx, 2
        jb @Int1
        mov ah, r.b.rI
        mov al, 0FFh
        invoke vImm16, ax
        mov dx, [eax]
 @Int1: call vSPdetect
        mov ebx, vSP
        call GetPCCmd
        cmp al, 76h; команда HALT ?
        jne @F; нет
        inc pc; да команда HALT, по возвращении из прерывания нужно перейти к следующей команде
    @@: mov ax, pc
        mov [ebx], ax
        mov pc, dx
        mov ax, spw
        push ax
        VideoMem ax
        pop ax
        inc ax
        VideoMem ax
   @@NoInt:
;**********************************************************
        .endif
	.ELSEIF uMsg==WM_KEYDOWN
                mov eax, lParam
                ror eax, 16
                and ax, 1FFh
                cmp al, 57h; F11 (ВВОД+БЛК)
                jne @F
                call ResetPC
                mov blk, 0
                jmp @@0
@@:             cmp al, 58h; F12 (БЛК+СБР)
                jne @F
                call ResetPC
                mov blk, 1
                jmp @@0
@@:             cmp ax, 285;RCtrl al, 3Ah; CapsLock (RUS/LAT)
                jne @F
                and port1, 7Fh; сбросить бит RUS/LAT
@@:             cmp al, 2Ah; LShift (CC)
                jne @F
                and port1, 0DFh; сбросить бит CC
@@:             cmp al, 1Dh; LCtrl (YC)
                jne @F
                and port1, 0BFh; сбросить бит YC
@@:             call KeyDownToChar
                and al,al
                jz @@0
                call KeyToPort2
@@0:
	.ELSEIF uMsg==WM_KEYUP
                mov eax, lParam
                ror eax, 16
                and ax, 1FFh
                cmp ax, 285;RCtrl al, 3Ah; CapsLock (RUS/LAT)
                jne @F
                xor EncNeed, 1; установить флаг перекодировки ЙЦУКЕНГ
                or port1, 80h; установить бит RUS/LAT
@@:             cmp al, 2Ah; LShift (CC)
                jne @F
                or port1, 20h; установить бит CC
@@:             cmp al, 1Dh; LCtrl (YC)
                jne @F
                or port1, 40h; установить бит YC
@@: ;очистка буфера нажатых клавиш
        xor dx, dx
        dec dx
        lea ebx, port2
        mov [ebx],dx
        inc ebx
        inc ebx
        mov [ebx],dx
        inc ebx
        inc ebx
        mov [ebx],dx
        inc ebx
        inc ebx
        mov [ebx],dx
	.ELSEIF uMsg==WM_CHAR
                mov eax, wParam
                cmp al, 8   ;BS
                je @@setCC
                cmp al, 22h ;'
                je @@rstCC
                cmp al, 27h ;"
                je @@rstCC
                cmp al, 42  ;*
                je @@rstCC
                cmp al, 43  ;+
                je @@rstCC
                cmp al, 45  ;-
                je @@setCC
                cmp al, 47  ;/
                je @@setCC
                cmp al, 58  ;:
                je @@setCC
                cmp al, 61  ;=
                je @@rstCC
                cmp al, 64  ;@
                je @@setCC
                cmp al, 94  ;^
                je @@setCC
                cmp al, 96  ;~
                je @@rstCC
                jmp @@skip
@@setCC:        or port1, 20h; установить бит CC
                jmp @@skip
@@rstCC:        and port1, 0DFh; сбросить бит CC

@@skip:         cmp al, 20h
                jae @F
                test port1, 40h
                jnz @F
                add al, 40h
@@:             cmp al, 60h
                jb @F
                sub al, 20h
@@:             cmp EncNeed, 0h
                je @F
                call QWEtoJCU
@@:             call KeyToPort2
	.ELSE
		invoke DefWindowProc,hWnd,uMsg,wParam,lParam
		ret
	.ENDIF
	xor    eax,eax
	ret
WndProc endp
;///////////////////////////////////////////////////
include KeyTable.inc
include Graphics.inc
include ProcDialog.inc
include Sound.inc
end start