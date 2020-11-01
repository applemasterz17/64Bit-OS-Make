[BITS 32]

; Main.c 에서 호출하기 위해 global 키워드로 외부 사용가능하게 등록 
global kReadCPUID, kSwitchAndExecute64bitKernel

SECTION .text

kReadCPUID:
    push ebp
    mov ebp, esp
    push eax
    push ebx
    push ecx
    push edx
    push esi

    ; dwEax 으로 CPUID 호출  
    mov eax, dword [ ebp + 8 ]
    cpuid 

    ; *pdwEAX 
    mov esi, dword [ ebp + 12 ]
    mov dword [ esi ], eax

    ; *pdwEBX
    mov esi, dword [ ebp + 16 ]
    mov dword [ esi ], ebx

    ; *pdwECX
    mov esi, dword [ ebp + 20 ]
    mov dword [ esi ], ecx

    ; *pdwEDX
    mov esi, dword [ ebp + 24 ]
    mov dword [ esi ], edx 

    pop esi
    pop edx
    pop ecx
    pop ebx 
    pop eax
    pop ebp
    ret 

kSwitchAndExecute64bitKernel:
    ; CR4 컨트롤 레지스터의 PAE 비트를 1로 설정 
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax 

    ; Enable PML4 Table's Address and Cache from CR3 Register
    mov eax, 0x100000
    mov cr3, eax

    ; IA32_EFER.LME 비트를 1로 설정해 IA-32e 모드를 활성화 시킴 
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x0100
    wrmsr

    ; CR0 의 NW=0, CD=0, PG=1 비트를 활성화 하고 캐싱과 페이징을 활성화 
    mov eax, cr0 
    or eax, 0xE0000000
    xor eax, 0x60000000
    mov cr0, eax 

    jmp 0x08:0x200000

    jmp $


