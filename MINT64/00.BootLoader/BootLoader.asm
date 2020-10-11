[ORG 0x00]
[BITS 16]

SECTION .text

; CS 세그먼트 레지스터는 mov 대신, jmp 를 이용해 초기화 (다른 세그먼트 레지스터는 mov 로 처리 가능)
; Initialize CS segment to 0x07c0(0x7c00), and goto START label
jmp 0x07c0:START

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Environment Variable
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
TOTALSECTORCOUNT:   dw  1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Code Section 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;; Start Real Mode ;;;;;
START:
    ; Initialize DS segment(boot loader), ES segment(video memory)
    mov ax, 0x07C0 
    mov ds, ax ; DS = 0x07C0 * 16 = 0x7C00
    mov ax, 0xB800 
    mov es, ax ; ES = 0xB800 * 16 = 0xB8000

    ; 0x0001:0000 부터 OS 이미지 로딩됨
    ; Create Stack 0x0000:0000 ~ 0x0000:FFFF, 64KB 
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xFFFE
    mov bp, 0xFFFE

    mov si, 0   ;set for screen clear loop 

.SCREENCLEARLOOP:
    ; Start Screen Clearing 
    mov byte [ es: si ], 0
    mov byte [ es: si + 1 ], 0x70 ;[bg_color][text_color]
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

    ; Set Destination [Memory Address] to [ES Segment Register]
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

    ; If Sector Num is 19, Toggle Head, Sector Num Set 1 
    xor byte [ HEADNUMBER ], 0x01
    mov byte [ SECTORNUMBER ], 0x01

    ; If Head Change 1->0, Change Track 
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

    ; If Not NULL, Print and Set Counter
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

; 부트섹터(총 512바이트) 의 마지막 2바이트(511, 512) 는 각 0x55, 0xAA 를 저장해야함 
; 0x55, 0xAA 를 해당 위치에 저장하면, 부트로더임을 알릴수 있음.
; times: times 뒤에 오는 횟수만큼 반복 작업 
; $: 현재 위치 / $$ 현재 섹션(.text) 시작 주소 
; ( $ - $$ ) : 현재 섹션(.text) 기준 현재 위치까지의 오프셋 
; 메모리 0x00 부터 .text 섹션이고, 511 부터 0x55, 0xAA 를 작성해야 하니까 
; 지금까지 사용한 만큼 제외하고 510 에서 오프셋 만큼 빼버려 나머지를 0x00 으로 채운다.
times   510 - ( $ - $$ )    db  0x00
db  0x55
db  0xAA
