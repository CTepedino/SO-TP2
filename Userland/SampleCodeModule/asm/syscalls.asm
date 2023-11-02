GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_sleep
GLOBAL sys_screenInfo
GLOBAL sys_textPosition
GLOBAL sys_getRTC
GLOBAL sys_getREGS
GLOBAL sys_beep
GLOBAL sys_putRectangle
GLOBAL sys_putCircle
GLOBAL sys_getCurrentKeyPress
GLOBAL sys_getCurrentReleasedKeys
GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_open_sem
GLOBAL sys_close_sem
GLOBAL sys_post_sem
GLOBAL sys_wait_sem

section .text

%macro systemcall 1
    mov rax, %1
    int 0x80
    ret
%endmacro

sys_read:
    systemcall 0

sys_screenInfo:
    systemcall 1

sys_textPosition:
    systemcall 2

sys_getCurrentKeyPress:
    systemcall 3

sys_getCurrentReleasedKeys:
    systemcall 4

sys_write:
    systemcall 5

sys_putRectangle:
    systemcall 6

sys_putCircle:
    systemcall 7

sys_beep:
    systemcall 8

sys_sleep:
    systemcall 9

sys_getRTC:
    systemcall 10

sys_getREGS:
    systemcall 11

sys_malloc:
    systemcall 12

sys_free:
    systemcall 13

sys_open_sem
    systemcall 14

sys_close_sem
    systemcall 15

sys_post_sem
    systemcall 16 

sys_wait_sem
    systemcall 16 

