[ORG 0x00]
[BITS 16]

SECTION .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Code Section 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x1000
    mov ds, ax 
    mov es, ax 

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Turn On A20 Gate 
    ; If Failed BIOS, Try System Control Port 
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov ax, 0x2401
    int 0x15
    jc .A20GATEERROR
    jmp .A20GATESUCCESS

.A20GATEERROR:
    in al, 0x92
    or al, 0x02
    and al, 0xFE
    out 0x92, al

.A20GATESUCCESS:
    cli ; No Interrupt
    lgdt [ GDTR ] ; Set GDT to Processsor 

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Get Into Protected Mode
    ; Disable Paging, Disable Cache, Internal FPU, Disable Align Check,
    ; Enable Protected Mode 
    ; PG=0, CD=1, Nw=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, P#=1
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, 0x4000003B
    mov cr0, eax

    ; 커널 코드 세그먼트를 0x00을 기준으로 하는 것으로 교체하고 EIP의 값을 0x00을 기준으로 재설정
    ; CS 세그먼트 셀렉터 : EIP
    jmp dword 0x08: ( PROTECTEDMODE - $$ + 0x10000 )

[BITS 32]
PROTECTEDMODE:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Create Stack 0x00000000 ~ 0x0000FFFF 64KB
    mov ss, ax
    mov esp, 0xFFFE
    mov ebp, 0xFFFE

    ; Print Change Success Message
    push ( SWITCHSUCCESSMESSAGE - $$ + 0x10000 )
    push 2
    push 0
    call PRINTMESSAGE
    add esp, 12

    ; Jump 0x10200, There is C Lang Kernel Code
    ; CS Segment Selector -> Kernel Code Discriptor(0x08)
    jmp dword 0x08: 0x10200

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Function Code Section 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PRINTMESSAGE:
    push ebp
    mov ebp, esp
    push esi
    push edi
    push eax
    push ecx
    push edx

    ; We Don't Need Set ES Register To Use Video Memory Address
    ; Because, 32bit Can Access 32bit Offset

    ; Y Address 
    mov eax, dword [ ebp + 12 ]
    mov esi, 160 ; One Line = 2 * 80 Colume
    mul esi
    mov edi, eax

    ; X Address 
    mov eax, dword [ ebp + 8 ]
    mov esi, 2 
    mul esi 
    add edi, eax

    ; String Address 
    mov esi, dword [ ebp + 16 ]

.MESSAGELOOP:
    ; Compare String is NULL
    mov cl, byte [ esi ]
    cmp cl, 0
    je .MESSAGEEND

    ; If Not NULL, Print and Set Counter
    mov byte [ edi + 0xB8000 ], cl
    add esi, 1
    add edi, 2
    jmp .MESSAGELOOP

.MESSAGEEND:
    pop edx
    pop ecx
    pop eax
    pop edi 
    pop esi
    pop ebp
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Data Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Need To Align Data Below
align 8, db 0

; GDTR's Last 8Byte Align
dw 0x0000

GDTR:
    dw GDTEND - GDT - 1 ; Size 
    dd ( GDT - $$ + 0x10000 ) ; 32bit Base Address 
                              ; GDT Linear Address - Curr + Physical Address 

GDT:
    NULLDescriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00 
        db 0x00
        db 0x00

    CODEDESCRIPTOR:
        ; Create Code Segment Descriptor 
        dw 0xFFFF ; Limit [15:0]                         (2byte)
        dw 0x0000 ; Base [15:0]                          (2byte)
        db 0x00 ; Base [23:16]                           (1byte)
        db 0x9A ; P=1, DPL=0, Code Segment:Execute/Read  (1byte)
        db 0xCF ; G=1, D/B=1, L=0, Limit [19:16]         (1byte)
        db 0x00 ; Base [31:24]                           (1byte)

    DATADESCRIPTOR:
        ; Create Data Segment Discriptor 
        dw 0xFFFF ; Limit [15:0]                         (2byte)
        dw 0x0000 ; Base [15:0]                          (2byte)
        db 0x00 ; Base [23:16]                           (1byte)
        db 0x92 ; P=1, DPL=0, Data Segment:Read/Write    (1byte)
        db 0xCF ; G=1, D/B=1, L=0, Limit [19:16]         (1byte)
        db 0x00 ; Base [31:24]                           (1byte)
GDTEND:

SWITCHSUCCESSMESSAGE: db 'Switch To Protected Mode Success !!', 0

; 섹터 나머지 부분을 채움 
times 512 - ( $ - $$ ) db 0x00