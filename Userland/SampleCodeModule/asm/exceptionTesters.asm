GLOBAL divideByZero
GLOBAL invalidOpCode

EXTERN killCurrent

section .text

divideByZero:
    mov rax, 5
    mov rbx, 0
    div rbx
    call killCurrent
    ret

invalidOpCode:
    ud2
    call killCurrent
    ret