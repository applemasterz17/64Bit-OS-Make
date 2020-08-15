[ORG 0x00] ; 코드의 시작 
[BITS 16] ; 이하 코드는 16 비트 코드로 설정

SECTION .text ; text 섹션을 정의 

jmp 0x1000:START ; CS 세그먼트 레지스터에 0x1000 을 복사, START 레이블로 이동 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Environment Value
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SECTORCOUNT:    dw  0x0000 ; 현재 실행 중인 섹터 번호 저장 
TOTALSECTORCOUNT    equ 100 ; 총 섹터 수, 최대 1152 섹터 까지 가능 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Code Section 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, cs
    mov ds, ax
    mov ax, 0xB800
    mov es, ax      ; ES segment 는 비디오 메모리 구간 

    ; 각 섹터 별로 코드 생성 
    %assign i   0
    %rep TOTALSECTORCOUNT
        %assign i   i + 1

        mov ax, 2
        mul word [ SECTORCOUNT ]
        mov si, ax 
        mov byte [ es: si + ( 160 * 2 ) ], '0' + ( i % 10)
        add word [ SECTORCOUNT ], 1    

        %if i == TOTALSECTORCOUNT
            jmp $
        %else
            jmp ( 0x1000 + i * 0x20 ): 0x0000
        %endif

        times ( 512 - ( $ - $$ ) % 512 )    db 0x00
    %endrep