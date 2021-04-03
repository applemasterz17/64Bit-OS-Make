[BITS 64]       ;이하의 코드는 64비트 코드로 설정 

SECTION .text   ;text 섹션(세그먼트)을 정의

;C 언어에서 호출할 수 있도록 이름을 노출함
global kInPortByte, kOutPortByte

; 포트에서 1바이트 읽어온다 
; PARAM: 포트 번호
kInPortByte:
    push rdx        ;함수에서 임시로 사용할 레지스터 스택에 저장
                    ;마지막 리턴 전에 스택에서 pop 해서 복원 

    mov rdx, rdi    ;RDX 레지스터에 파라미터1(포트번호) 저장
    mov rax, 0      ;RAX 레지스터를 초기화, 여기에 읽어온 값이 저장됨 
    in al, dx       ;DX 레지스터에 저장된 포트 어드레스에서 한 바이트 읽어
                    ;AL 레지스터에 저장, AL 레지스터는 함수 반환 값으로 사용됨

    pop rdx         ;함수에서 사용이 끝난 레지스터 복원
    ret

; 포트에 1바이트 보낸다(쓴다)
; PARAM: 포트 번호, 보낼 데이터 
kOutPortByte:
    push rdx        ;함수에서 임시로 사용할 레지스터 스택에 저장
    push rax        ;마지막 리턴 전에 스택에서 pop 해서 복원 

    mov rdx, rdi    ;RDX 레지스터에 파라미터1(포트번호) 저장
    mov rax, rsi    ;RAX 레지스터에 파라미터2(데이터) 저장
    out dx, al      ;DX 레지스터에 저장된 포트 어드레스에
                    ;AL 레지스터에 저장된 한 바이트를 쓴다

    pop rax         ;함수에서 사용이 끝난 레지스터 복원
    pop rdx
    ret