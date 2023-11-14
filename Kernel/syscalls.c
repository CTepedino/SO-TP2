#include <stdint.h>
#include <defs.h>
#include <audioDriver.h>
#include <time.h>
#include <lib.h>
#include <stdio.h>
#include <interrupts.h>
#include <memoryManager.h>
#include <semaphore.h>
#include <scheduler.h>
#include <pipe.h>


static int sys_read(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_screenInfo(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_textPosition(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_getAllKeys(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_getReleasedKeys(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static int sys_write(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_putRectangle(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_putCircle(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static int sys_beep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_sleep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static int sys_getRTC(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_getREGS(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static int sys_malloc(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_free(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_memoryInfo(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static int sys_addProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_killProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_killCurrentProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_getCurrentPid(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_setProcessPriority(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_blockProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_unblockProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_waitForChildren(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_yield(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_schedulerInfo(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static int sys_openSem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_closeSem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_postSem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_waitSem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_openNewSem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static int sys_openPipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_createNewPipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_closePipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_readPipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static int sys_writePipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static int sys_getStatus(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static int (*syscalls[])(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) = {
        sys_read,  sys_screenInfo, sys_textPosition, sys_getAllKeys, sys_getReleasedKeys, sys_write,sys_putRectangle,
        sys_putCircle ,sys_beep,sys_sleep, sys_getRTC, sys_getREGS, sys_malloc, sys_free, sys_memoryInfo,
        sys_addProcess, sys_killProcess, sys_killCurrentProcess, sys_getCurrentPid, sys_setProcessPriority,
         sys_blockProcess, sys_unblockProcess, sys_waitForChildren, sys_yield, sys_schedulerInfo, sys_openSem, sys_closeSem,
        sys_postSem, sys_waitSem, sys_openPipe, sys_createNewPipe, sys_closePipe, sys_readPipe, sys_writePipe, sys_openNewSem,
        sys_getStatus,
};

int syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t rax){
    return syscalls[rax](rdi, rsi, rdx, rcx, r8, r9);
}



static int sys_read(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return fRead((char *) rdi,rsi);
}

static int sys_screenInfo(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    screenInfo((uint32_t *) rdi, (uint32_t *) rsi);
    return 0;
}

static int sys_textPosition(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    setTextPosition(rdi,rsi);
    return 0;
}

static int sys_getAllKeys(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    getAllKeys((char *) rdi);
    return 0;
}

static int sys_getReleasedKeys(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    getReleasedKeys((char *) rdi);
    return 0;
}


static int sys_write(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return fWrite(rdi,(const char *)rsi,rdx);
}

static int sys_putRectangle(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    putRectangle(rdi,rsi,rdx,rcx,r8);
    return 0;
}

static int sys_putCircle(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    putCircle(rdi,rsi, rdx, rcx);
    return 0;
}


static int sys_beep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    _sti();
    beep((uint8_t) rdi, rsi);
    _cli();
    return 0;
}

static int sys_sleep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    _sti();
    sleep(rdi);
    _cli();
    return 0;
}


static int sys_getRTC(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    getRTC((timeStruct *) rdi);
    return 0;
}

static int sys_getREGS(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    getREGS((int *) rdi, (uint64_t *) rsi);
    return 0;
}

static int sys_malloc(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
     return (uint64_t) memAlloc(rdi);
}

static int sys_free(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    memFree((void *) rdi);
    return 0;
}

static int sys_memoryInfo(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    memoryInfo();
    return 0;
}

static int sys_addProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return addProcess((void *)rdi, (char *) rsi, rdx, (char **)rcx, r8,(int *) r9);
}

static int sys_killProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    killProcess(rdi);
    return 0;
}

static int sys_killCurrentProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    killCurrentProcess();
    return 0;
}

static int sys_getCurrentPid(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return getCurrentPid();
}

static int sys_setProcessPriority(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    setProcessPriority(rdi, rsi);
    return 0;
}

static int sys_blockProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    blockProcess(rdi);
    return 0;
}

static int sys_unblockProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    unblockProcess(rdi);
    return 0;
}

static int sys_waitForChildren(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    waitForChildren(rdi);
    return 0;
}

static int sys_yield(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    yield();
    return 0;
}

static int sys_schedulerInfo(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    schedulerInfo();
    return 0;
}

static int sys_openSem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return openSem(rdi, rsi);
}

static int sys_closeSem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    closeSem(rdi);
    return 0;
}

static int sys_postSem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    postSem(rdi);
    return 0;
}

static int sys_waitSem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    waitSem(rdi);
    return 0;
}

static int sys_openNewSem(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return openNewSem(rdi);
}

static int sys_openPipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return openPipe(getCurrentPid(), rdi, rsi);
}

static int sys_createNewPipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return createNewPipe();
}

static int sys_closePipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    closePipe(getCurrentPid(), rdi);
    return 0;
}

static int sys_readPipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return readPipe(rdi, (char *) rsi, rdx);
}

static int sys_writePipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return writePipe(rdi, (char *) rsi, rdx);
}

static int sys_getStatus(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    return getStatus(rdi);
}