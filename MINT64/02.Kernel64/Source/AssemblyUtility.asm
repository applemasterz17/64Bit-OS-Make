; 포트로부터 1바이트를 읽음
; PARAM: 포트 번호
kInPortByte:
    push rdx

    mov rdx, rdi
    mov rax, 0
    in al, dx

    pop rdx
    ret