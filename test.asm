
outfile 'test.bin'    ; name the file to create

.model tiny
.code
.186
           ; seg = 07C0h and offset 0000h, we can read in 127 MBR's on our stack.
           org  00h
           
           ; =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
           ; set up the stack and seg registers (real mode)
           mov  ax,07C0h                ; set our stack, DS, ES
           mov  ds,ax                   ;
           mov  es,ax                   ;
           xor  ax,ax                   ;
           mov  ss,ax                   ; first push at 0x07BFE
           mov  sp,7C00h                ;
           
           ; =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
           ; simply display a string and hlt
           mov  si,offset prt_string
           call display_string
hang:      hlt
           jmp  short hang


; =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
; display a char to the screen using the BIOS
; On entry:
;       al = char to display
;
display_char proc near uses ax bx
           mov  ah,0Eh             ; print char service
           xor  bx,bx              ; 
           int  10h                ; output the character
           ret
display_char endp

; =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
; display a string to the screen using the BIOS
; On entry:
;   ds:si -> asciiz string to display
;
display_string proc near uses ax si
           cld
@@:        lodsb
           or   al,al
           jz   short @f
           call display_char
           jmp  short @b
@@:        ret
display_string endp

; =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
; string data

; we need to be at a known location within the source, so the mbrtest.c code
;  knows where to put the string.  the mbrtest.c will build the string and 
;  place it here.
           org 100h
prt_string:

; disk identifier and partition entry(s)
           org (200h-4-2-(4*16)-2)
           
           ; =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
           ; modern OS's write a 32-bit signature here to help
           ; identify the media device.  The 16-bits that follow
           ; might be used on some systems to indicate copy-protection.
           dd  ?              ; identifier written at FDISK time
           dw  0              ; reserved
           
           dup (16*4),0       ; four 16-byte partitions
           
           dw  0AA55h
.end
