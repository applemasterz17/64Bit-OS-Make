[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07c0:START

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Environment Value
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
TOTALSECTORCOUNT:   dw  1024

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Code Section 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    ; Initialize DS segment(boot loader), ES segment(video memory)
    mov ax, 0x07C0 
    mov ds, ax ; DS = 0x07C0 * 16 = 0x7C00
    mov ax, 0xB800 
    mov es, ax ; ES = 0xB800 * 16 = 0xB8000

    ; Make Stack 0x0000:0000 ~ 0x0000:FFFF, 64KB 
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xFFFE
    mov bp, 0xFFFE

    mov si, 0   ;set for screen clear loop 

.SCREENCLEARLOOP:
    ; Start Screen Clearing 
    mov byte [ es: si ], 0
    mov byte [ es: si + 1 ], 0x0B
    add si, 2
    cmp si, 80 * 25 * 2
    jl .SCREENCLEARLOOP

    ; Print Start Message
    push MESSAGE1
    push 0
    push 0
    call PRINTMESSAGE
    add sp, 6

    ; Print OS Image Loading Message
    push IMAGELOADINGMESSAGE
    push 1
    push 0
    call PRINTMESSAGE
    add sp, 6

RESETDISK:
    ; Call BIOS Reset Function 
    mov ax, 0
    mov dl, 0
    int 0x13
    jc HANDLEDISKERROR

SETREADDATA:
    ; Set Destination Address to ES segment
    mov si, 0x1000
    mov es, si
    mov bx, 0x0000
    mov di, word [ TOTALSECTORCOUNT ]

READDATA:
    ; Check Total Sector Count
    cmp di, 0
    je READEND
    sub di, 0x1

    ; Call Sector Read Function
    mov ah, 0x02
    mov al, 0x1
    mov ch, byte [ TRACKNUMBER ]
    mov cl, byte [ SECTORNUMBER ]
    mov dh, byte [ HEADNUMBER ]
    mov dl, 0x00
    int 0x13
    jc HANDLEDISKERROR

    ; Add Destination Address 0x200
    add si, 0x0020
    mov es, si

    ; Check Sector Num 1 ~ 18 
    mov al, byte [ SECTORNUMBER ]
    add al, 0x01
    mov byte [ SECTORNUMBER ], al
    cmp al, 19
    jl READDATA

    ; IF Sector Num is 19, Toggle Head, Sector Num Set 1 
    xor byte [ HEADNUMBER ], 0x01
    mov byte [ SECTORNUMBER ], 0x01

    ; IF Head Change 1->0, Change Track 
    cmp byte [ HEADNUMBER ], 0x00
    jne READDATA

    ; Change Track 
    add byte [ TRACKNUMBER ], 0x01
    jmp READDATA

READEND:
    ; Print OS Image Loading Complete
    push LOADINGCOMPLETEMESSAGE
    push 1
    push 20
    call PRINTMESSAGE
    add sp, 6

    ; Jump to Loaded OS Image
    jmp 0x1000:0x0000

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Function Code Section 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HANDLEDISKERROR:
    push DISKERRORMESSAGE
    push 1
    push 20
    call PRINTMESSAGE
    jmp $

PRINTMESSAGE:
    ; Prolog
    push bp
    mov bp, sp
    push es
    push si
    push di
    push ax
    push cx
    push dx

    ; Set ES segment to Video Memory Address 
    mov ax, 0xB800
    mov es, ax

    ; Calculate X, Y from Video Memory Address 
    ; Y Address
    mov ax, word [ bp + 6 ]
    mov si, 160
    mul si
    mov di, ax
    ; X Address
    mov ax, word [ bp + 4 ]
    mov si, 2
    mul si
    add di, ax
    ; String
    mov si, word [ bp + 8 ]

.MESSAGELOOP:
    ; Compare String is NULL
    mov cl, byte [ si ]
    cmp cl, 0
    je .MESSAGEEND

    ; IF Not Null, Print Message
    mov byte [ es: di ], cl
    add si, 1
    add di, 2
    jmp .MESSAGELOOP

.MESSAGEEND:
    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Data Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
MESSAGE1:   db  'applemasterz OS Boot Loader Start', 0

DISKERRORMESSAGE:       db  'DISK Error', 0
IMAGELOADINGMESSAGE:    db  'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db  'Complete!', 0

SECTORNUMBER:   db  0x02
HEADNUMBER:     db  0x00
TRACKNUMBER:    db  0x00

times   510 - ( $ - $$ )    db  0x00
db  0x55
db  0xAA