#include <videoDriver.h>
#include <lib.h>
#include <interrupts.h>
#define ZERO_EXCEPTION_ID 0x0
#define INVALID_OP_ID 6

extern void read(uint64_t fd, char *buffer, uint64_t length);
extern void write(uint64_t fd, const char * string, uint64_t count);

extern uint64_t getSampleCodeModuleAddress();
extern uint64_t getStackBase();

static char * regs[] = {"RAX", "RBX", "RCX", "RDX", "RBP","RSI", "RDI", "R8 ", "R9 ", "R10",
                        "R11", "R12", "R13", "R14", "R15", "RSP", "RIP", "RFLAGS"};

static void printERR(const char * message);
static void printREGS(uint64_t * RSP);
static void printREG(char * regName, int nameLen, uint64_t reg, char * buffer);

static void zero_division();
static void invalid_op_code();



void exceptionDispatcher(int exception, uint64_t * RSP) {

	switch (exception){
        case ZERO_EXCEPTION_ID:
            zero_division();
            break;
        case INVALID_OP_ID:
            invalid_op_code();
            break;
        default:
            return;
    }
    printREGS(RSP);
    char rstMsg[] = "Presione cualquier tecla para reiniciar.";
    _sti();
    write(2, rstMsg, strlen(rstMsg));
    read(0, rstMsg, 1);
    _cli();

    uint32_t w,h;
    w = getWidth();
    h = getHeight();
    for(uint32_t i=0; i<w;i++){
        for(uint32_t j=0;j<h;j++){
            putPixel(0, i,j);
        }
    }

    RSP[15] = getSampleCodeModuleAddress();
    RSP[18] = getStackBase() - 8 * 3;

}

static void zero_division() {
	printERR("\nERROR: La division por cero esta indefinida\n");
}

static void invalid_op_code(){
    printERR("\nERROR: Codigo de operacion invalido\n");
}

static void printERR(const char * message){
    write(2,message, strlen(message));
}

static void printREGS(uint64_t * RSP) {
    char regBuffer[16];

   //registros generales
   for(int i=0; i<=14;i++){
       printREG(regs[i], 3, RSP[14-i], regBuffer);
   }
   //rsp
   printREG(regs[15], 3, RSP[18], regBuffer);

   //rip
   printREG(regs[16], 3, RSP[15], regBuffer);

   //rflags
   printREG(regs[17], 6, RSP[17], regBuffer);

}

static void printREG(char * regName, int nameLen, uint64_t reg, char * buffer){
    write(2, regName, nameLen);
    write(2, ":     ", 8-nameLen);
    intToString(reg, buffer, 16, 16);
    write(2, buffer, 16);
    write(2, "\n", 1);
}
