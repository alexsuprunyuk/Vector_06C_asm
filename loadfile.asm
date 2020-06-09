.486				; 32 bit when .486 appears before .MODEL
.MODEL FLAT,STDCALL
option casemap :none  ; case sensitive

include \masm32\include\windows.inc

include \masm32\include\user32.inc
include \masm32\include\kernel32.inc
include \masm32\include\gdi32.inc
include \masm32\include\comdlg32.inc

includelib \masm32\lib\user32.lib
includelib \masm32\lib\kernel32.lib
includelib \masm32\lib\gdi32.lib
includelib \masm32\lib\comdlg32.lib
	
.data

szFilter  db	"Programm Files",0, "*.com",0,\
                "Read Only Memory Files",0, "*.rom",0,\
                "Saved Memory Files",0, "*.sav",0,\
                "Fixed Disk Image",0, "*.fdd",0,\
		        "All Files",0,"*.*",0,0

DefExtn	  db	"com",0				; Default extension for file

.data?

ofn   OPENFILENAME <>
EXTRN	dwFileSize:DWORD

.Code
;------------------------------------------------------------------------------
FileInitialize	proc	hwnd:HWND

	mov	ofn.lStructSize, sizeof OPENFILENAME
	mov	eax, hwnd
	mov	ofn.hWndOwner, eax
	mov	ofn.hInstance, 0
	mov	ofn.lpstrFilter, offset szFilter
	mov	ofn.lpstrCustomFilter, 0
	mov	ofn.nMaxCustFilter, 0
	mov	ofn.nFilterIndex, 0
	mov	ofn.lpstrFile, 0		; Set in Open and Close functions
	mov	ofn.nMaxFile, MAX_PATH
	mov	ofn.lpstrFileTitle, 0		; Set in Open and Close functions
	mov	ofn.nMaxFileTitle, MAX_PATH
	mov	ofn.lpstrInitialDir, 0
	mov	ofn.lpstrTitle, 0
	mov	ofn.Flags, 0			; Set in Open and Close functions
	mov	ofn.nFileOffset, 0
	mov	ofn.nFileExtension, 0
	mov	ofn.lpstrDefExt,offset DefExtn	; default extension is com
	mov	ofn.lCustData, 0
	mov	ofn.lpfnHook, 0
	mov	ofn.lpTemplateName, 0
	ret

FileInitialize	endp

FileOpenDlg		proc hwnd:HWND, pstrFileName:DWORD, pstrTitleName:DWORD

	mov	eax,hwnd
	mov	ofn.hWndOwner, eax
	mov	eax, pstrFileName
	mov	ofn.lpstrFile, eax
	mov	eax,pstrTitleName
	mov	ofn.lpstrFileTitle, eax
	mov	ofn.Flags, 0

	invoke	GetOpenFileName, ADDR ofn
	or eax, eax
	jz @F
    mov eax, ofn. nFilterIndex
@@:
	ret

FileOpenDlg		endp

FileSaveDlg		proc hwnd:HWND, pstrFileName:DWORD, pstrTitleName:DWORD

	mov	eax,hwnd
	mov	ofn.hWndOwner, eax
	mov	eax,pstrFileName
	mov	ofn.lpstrFile,  eax
	mov	eax, pstrTitleName
	mov	ofn.lpstrFileTitle, eax
	mov	ofn.Flags, OFN_OVERWRITEPROMPT
	
	invoke	GetSaveFileName, ADDR ofn

	ret

FileSaveDlg		endp

LoadFile	proc uses ebx, pstrFileName:DWORD, pbmfh:DWORD

LOCAL	bSuccess:DWORD,  dwHighSize:DWORD, dwBytesRead:DWORD, hFile:HANDLE

	invoke	CreateFile, pstrFileName, GENERIC_READ, FILE_SHARE_READ, 0, \
			    OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0
	mov	hFile,eax

	.IF eax==INVALID_HANDLE_VALUE
	    mov	eax,0
	    ret
	.ENDIF

	invoke GetFileSize, hFile, ADDR dwHighSize
	mov dwFileSize, eax

	.IF dwHighSize
	    invoke CloseHandle, hFile
	    mov	eax,0
	    ret
	.ENDIF

	invoke ReadFile, hFile, pbmfh, dwFileSize, ADDR dwBytesRead, 0
	mov	bSuccess,eax
	invoke CloseHandle, hFile

	mov	eax, pbmfh
	ret

LoadFile	endp

SaveImage	proc uses ebx, pstrFileName:DWORD, pbmfh:DWORD, fSize:DWORD

LOCAL	bSuccess:DWORD, dwBytesWritten:DWORD, hFile:HANDLE

	mov	ebx, pbmfh			; Get pointer to MAPFILE

	invoke	CreateFile, pstrFileName, GENERIC_WRITE, 0, 0, \
			    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0
	mov	hFile, eax
	
	.IF eax==INVALID_HANDLE_VALUE
	    mov	eax,0
	    ret
	.ENDIF

	invoke WriteFile, hFile, pbmfh, fSize, ADDR dwBytesWritten, 0
	mov	bSuccess, eax

	invoke	CloseHandle, hFile

	mov	eax, dwBytesWritten

	.IF !bSuccess || eax!=fSize
	    invoke DeleteFile, ADDR pstrFileName
	    mov	eax, FALSE
	.ELSE
	    mov	eax, TRUE
	.ENDIF

	ret
	
SaveImage	endp

	end

