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
GLOBAL sys_memoryInfo
GLOBAL sys_kill
GLOBAL sys_nice
GLOBAL sys_block
GLOBAL sys_ps
GLOBAL sys_getpid
GLOBAL sys_addProcess
GLOBAL sys_killProcess
GLOBAL sys_killCurrentProcess
GLOBAL sys_getCurrentPid
GLOBAL sys_setProcessPriority
GLOBAL sys_blockProcess
GLOBAL sys_unblockProcess
GLOBAL sys_waitForChildren 
GLOBAL sys_yield
GLOBAL sys_schedulerInfo  
GLOBAL sys_open_sem
GLOBAL sys_close_sem
GLOBAL sys_post_sem
GLOBAL sys_wait_sem
GLOBAL sys_open_new_sem

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

sys_memoryInfo:
    systemcall 14
sys_kill:
    systemcall 15
sys_nice:
    systemcall 16
sys_block:
    systemacall 17
sys_ps: 
    systemcall 18
sys_getpid:
    systemcall 19

sys_addProcess:
    systemcall 15

sys_killProcess:
    systemcall 16

sys_killCurrentProcess:
    systemcall 17

sys_getCurrentPid:
    systemcall 18

sys_setProcessPriority:
    systemcall 19

sys_blockProcess:
    systemcall 20

sys_unblockProcess:
    systemcall 21

sys_waitForChildren:
    systemcall 22

sys_yield:
    systemcall 23

sys_schedulerInfo:
    systemcall 24
    
sys_open_sem:
    systemcall 25

sys_close_sem:
    systemcall 26

sys_post_sem:
    systemcall 27 

sys_wait_sem:
    systemcall 28 
sys_open_new_sem:
    systemcall 29

