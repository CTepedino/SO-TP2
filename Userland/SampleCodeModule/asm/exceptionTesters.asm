GLOBAL divideByZero
GLOBAL invalidOpCode

section .text

divideByZero:
    mov rax, 5
    mov rbx, 0
    div rbx
    ret

invalidOpCode:
    ud2
    ret