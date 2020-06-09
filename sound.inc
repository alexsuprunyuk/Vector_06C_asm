; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

;     include files
;     ~~~~~~~~~~~~~
      include \masm32\INCLUDE\winmm.inc


      include \masm32\macros\macros.asm

;     libraries
;     ~~~~~~~~~
      includelib \masm32\LIB\winmm.lib

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
.const
bufsz EQU 2048
; «««««««««««««««««««««««««««««««««««««««««««««««««««««««««««««««««««««««««
.data?
vi53_hWave_Out dd ?
vi53_WF WAVEFORMATEX <>
ALIGN 8
vi53_Data1 db bufsz dup(?)
vi53_Data2 db bufsz dup(?)
sndbuf1 db bufsz dup(?)
sndbuf2 db bufsz dup(?)
sndPtr dd ?
sndbufPtr dd ?

vi53_WH1 WAVEHDR <>
vi53_WH2 WAVEHDR <>
vi53_parity db ?

.code
        waveOutOpen PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
        waveOutPause PROTO STDCALL :DWORD
        waveOutPrepareHeader PROTO STDCALL :DWORD,:DWORD,:DWORD
        waveOutReset PROTO STDCALL :DWORD
        waveOutRestart PROTO STDCALL :DWORD
        waveOutSetPitch PROTO STDCALL :DWORD,:DWORD
        waveOutSetPlaybackRate PROTO STDCALL :DWORD,:DWORD
        waveOutSetVolume PROTO STDCALL :DWORD,:DWORD
        waveOutUnprepareHeader PROTO STDCALL :DWORD,:DWORD,:DWORD
        waveOutWrite PROTO STDCALL :DWORD,:DWORD,:DWORD
        waveOutBreakLoop PROTO STDCALL :DWORD
        waveOutClose PROTO STDCALL :DWORD

Wave_callback proc hw:DWORD, uMsg:DWORD, dwInst:DWORD, dwPrm1:DWORD, dwPrm2:DWORD
    switch uMsg
    case WOM_OPEN
        ret
    case WOM_DONE
        cmp dwPrm1,0
        je @Exclb
                 lea eax, offset vi53_WH1
		cmp dwPrm1, eax
		jne @buf2mov
		lea edi, offset vi53_Data1
		lea esi, offset sndbuf2
		mov ecx,bufsz
		shr ecx, 2
		rep movsd [edi],[esi]
		jmp @playbuf
@buf2mov: lea edi, offset vi53_Data2
		lea esi, offset sndbuf1
		mov ecx,bufsz
		shr ecx, 2
		rep movsd [edi],[esi]
@playbuf: fn waveOutWrite, vi53_hWave_Out, dwPrm1, sizeof(WAVEHDR)
        ret
    case WOM_CLOSE
         fn waveOutUnprepareHeader, vi53_hWave_Out, dwPrm1, sizeof(WAVEHDR)
@Exclb:     ret
    endsw    
Wave_callback endp


OpenWaveOut proc hWnd:DWORD
    mov vi53_WF.wFormatTag, WAVE_FORMAT_PCM;
    mov vi53_WF.nChannels, 2;
    mov vi53_WF.nSamplesPerSec, 22050;
    mov vi53_WF.wBitsPerSample, 8
    mov vi53_WF.nBlockAlign, 2; nBlockAlign := wBitsPerSample div 8 * nChannels
    mov vi53_WF.nAvgBytesPerSec, 44100; nSamplesPerSec * nBlockAlign;
    mov vi53_WF.cbSize, 0;
	cmp vi53_hWave_Out,0
	jne @F
    fn waveOutOpen, ADDR vi53_hWave_Out, WAVE_MAPPER, ADDR vi53_WF, offset Wave_callback, 0, CALLBACK_FUNCTION
@@: nop
    return eax
OpenWaveOut endp

PrepareWave proc
    cmp vi53_hWave_Out,0
    je @F
    mov vi53_parity,0
    mov vi53_WH1.lpData, offset vi53_Data1
    mov vi53_WH1.dwBufferLength, bufsz
    mov vi53_WH1.dwLoops,1
    fn waveOutPrepareHeader, vi53_hWave_Out, ADDR vi53_WH1, sizeof(WAVEHDR)
	fn waveOutWrite, vi53_hWave_Out, ADDR vi53_WH1, sizeof(WAVEHDR)
    mov vi53_WH2.lpData, offset vi53_Data2
    mov vi53_WH2.dwBufferLength, bufsz
    mov vi53_WH2.dwLoops,1
    fn waveOutPrepareHeader, vi53_hWave_Out, ADDR vi53_WH2, sizeof(WAVEHDR)
	fn waveOutWrite, vi53_hWave_Out, ADDR vi53_WH2, sizeof(WAVEHDR)	
@@: ret
PrepareWave endp

PlayWave proc ; ax = Left&Right_bytes:WORD
             mov edi, sndbufPtr
             add edi, sndPtr
             mov [edi], al; Left Chl
             inc edi
             mov [edi], ah; Right Chl
             inc sndPtr
             inc sndPtr
             mov eax, sndPtr
             cmp eax, bufsz
             jl @bufnotready
             mov sndPtr, 0
             lea eax, sndbuf1
             cmp eax, sndbufPtr
             jne @swapbuf
             lea eax, sndbuf2
@swapbuf:    mov sndbufPtr, eax
@bufnotready: nop
	ret
PlayWave endp

StopWave proc
    cmp vi53_hWave_Out,0
    je @F
    xor eax,eax
    mov vi53_hWave_Out, eax
    fn waveOutReset,vi53_hWave_Out
    mov vi53_hWave_Out,0
@@: ret
StopWave endp


