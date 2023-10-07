GLOBAL startSound
GLOBAL endSound

section .text

startSound:
    push rbp
    mov rbp, rsp

    mov al, 182
    out 0x43, al
    mov ax, di
    out 0x42, al
    mov al, ah
    out 0x42, al
    in al, 0x61
    or al, 0x03
    out 0x61, al

    mov rsp, rbp
    pop rbp
    ret


endSound:
    push rbp
	mov rbp, rsp

    in al, 0x61
    and al, 0xFC
    out 0x61, al

    mov rsp, rbp
    pop rbp
    ret

;codigo basado en https://github.com/mlombardia/arqui_tpe

