#include <stdint.h>
#include <keyboardDriver.h>
#include <time.h>

static void int_20(uint64_t * RSP);
static void int_21(uint64_t * RSP);

static void (*int_handlers[])(uint64_t * RSP) = {int_20, int_21};

void irqDispatcher(uint64_t irq, uint64_t * RSP){
    int_handlers[irq](RSP);
}


static void int_20(uint64_t * RSP){
    timer_handler();
}
static void int_21(uint64_t * RSP){
    keyboard_handler(RSP);
}