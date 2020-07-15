[ORG 0x00]	;set start address
[BITS 16]	;set 16bit

SECTION	.text	;set text section	

; set CS register, jump START:
jmp 0x07C0:START	

START:
	mov ax, 0x07C0
	mov ds, ax		;set DS register
	mov ax, 0xB800
	mov es, ax		;set ES register to video-memory-addr
	mov si, 0	;clear counter for SCREENCLEARLOOP 

.SCREENCLEARLOOP:
	mov byte [es:si], 0 		;clean text
	mov byte [es:si+1], 0x0B 	;set text,bg color
	add si, 2
	cmp si, 80*25*2
	jl .SCREENCLEARLOOP
	mov si, 0	;clear counter for MESSAGELOOP
	mov di, 0	;clear counter for MESSAGELOOP

.MESSAGELOOP:
	mov cl, byte [si+MESSAGE1]
	cmp cl, 0
	je .MESSAGEEND
	mov byte [es:di], cl
	add si, 1
	add di, 2
	jmp .MESSAGELOOP

.MESSAGEEND:
	jmp $

MESSAGE1:	db 'AKALI', 0;

times 510 - ( $ - $$ )	db	0x00
db 0x55
db 0xAA

;--------------------------------------------------------
;start copy image 

TOTALSECTORCOUNT:	dw	1024
SECTORNUMBER:		db	0x02
HEADNUMBER:			db	0x00
TRACKNUMBER:		db 	0x00

	; set destination by copied image 
	mov si, 0x1000
	mov es, si
	mov bx, 0x0000
	mov di, word [TOTALSECTORCOUNT]


READDATA:
	; check loop condition 
	cmp di, 0
	je READEND
	sub di, 1

	; call BIOS "read function" 
	mov ah, 0x2
	mov al, 0x1
	mov ch, byte [TRACKNUMBER]
	mov cl, byte [SECTORNUMBER]
	mov	dh, byte [HEADNUMBER]
	int 0x13
	jc HANDLEDISKERROR	;if failed, error handling

	; calculate sector, head, track
	add si, 0x0020
	mov es, si

	mov al, byte [SECTORNUMBER]
	add al, 0x1
	mov byte [SECTORNUMBER], al
	cmp al, 19
	jl READDATA

	xor byte [HEADNUMBER], 0x01
	mov byte [SECTORNUMBER], 0x01 

	cmp byte [HEADNUMBER], 0x00
	jne READDATA

	add byte [TRACKNUMBER], 0x01
	jmp READDATA
READEND:

HANDLEDISKERROR:


