#include <stdint.h>
#include <defs.h>
#include <audioDriver.h>
#include <time.h>
#include <lib.h>
#include <stdio.h>
#include <interrupts.h>
#include <memoryManager.h>
#include <semaphore.h>

#define SYSCALL_COUNT 14

static void sys_read(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_screenInfo(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_textPosition(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_getAllKeys(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_getReleasedKeys(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static void sys_write(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_putRectangle(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_putCircle(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static void sys_beep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_sleep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static void sys_getRTC(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_getREGS(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static void sys_malloc(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_free(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static void sys_open_sem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_close_sem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_post_sem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void sys_wait_sem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static void (*syscalls[SYSCALL_COUNT])(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) = {
        sys_read,  sys_screenInfo, sys_textPosition, sys_getAllKeys, sys_getReleasedKeys, sys_write,sys_putRectangle,
        sys_putCircle ,sys_beep,sys_sleep, sys_getRTC, sys_getREGS, sys_malloc, sys_free, sys_open_sem, sys_close_sem,
        sys_post_sem, sys_wait_sem,
};

void syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t rax){
    if (rax < SYSCALL_COUNT) {
        syscalls[rax](rdi, rsi, rdx, rcx, r8, r9);
    }
}



static void sys_read(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    _sti();
    read(rdi,(char *) rsi,rdx);
    _cli();
}

static void sys_screenInfo(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    screenInfo((uint32_t *) rdi, (uint32_t *) rsi);
}

static void sys_textPosition(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    setTextPosition(rdi,rsi);
}

static void sys_getAllKeys(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    getAllKeys((char *) rdi);
}

static void sys_getReleasedKeys(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    getReleasedKeys((char *) rdi);
}


static void sys_write(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    write(rdi,(const char *)rsi,rdx);
}

static void sys_putRectangle(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    putRectangle(rdi,rsi,rdx,rcx,r8);
}

static void sys_putCircle(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    putCircle(rdi,rsi, rdx, rcx);
}


static void sys_beep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    _sti();
    beep((uint8_t) rdi, rsi);
    _cli();
}

static void sys_sleep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    _sti();
    sleep(rdi);
    _cli();
}


static void sys_getRTC(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    getRTC((timeStruct *) rdi);
}

static void sys_getREGS(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    getREGS((int *) rdi, (uint64_t *) rsi);
}

static void sys_malloc(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
     rsi = (uint64_t) memAlloc( (size_t) rdi);
}

static void sys_free(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    memFree((void *) rdi);
}

static void sys_open_sem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    rdx = open_sem(rdi, rsi);
}

static void sys_close_sem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    rsi = close_sem(rdi);
}

static void sys_post_sem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    rsi = post_sem(rdi);
}

static void sys_wait_sem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    rsi = wait_sem(rdi);
}