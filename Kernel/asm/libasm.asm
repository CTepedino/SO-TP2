GLOBAL cpuVendor
GLOBAL setTimeFormat
GLOBAL getTime

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret


setTimeFormat: ;Para no devolver el tiempo en BCD
   push rbp
   mov rbp, rsp

   mov al,0x0B
   out 70h,al
   in al, 71h
   or al, 4
   out 71h, al

   mov rsp, rbp
   pop rbp
   ret

getTime:

    push rbp
    mov rbp, rsp

    mov rax, rdi
    out 70h, al
    in al, 71h

    mov rsp, rbp
    pop rbp
    ret

