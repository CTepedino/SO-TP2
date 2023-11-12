#include <shell.h>

unsigned int scr_width;
unsigned int scr_height;


void processCommand(char * readbuf);
int searchCommand(char * command);

#define COMMAND_LIST_LENGTH 8
static Command commandList[] = {
        {"help", "Despliega una lista con los programas disponibles.", help},
        {"time", "Imprime en pantalla la fecha y hora del sistema.", time},
        {"dividebyzero", "Ejecuta un programa que divide por cero, causando una excepcion.", divideByZero},
        {"invalidopcode", "Ejecuta un programa que intenta realizar una operacion invalida, causando una excepcion.", invalidOpCode},
        {"regstatus", "Imprime en pantalla el ultimo estado de los registros guardado.", regStatus},
        {"pong", "Abre el juego Pong. El paddle izquierdo se controla con \'W\' y \'S\'.El derecho con \'I\' y \'K\'.", pong},
        {"testmm", "(DEBUG)Test para memory manager", testmm},
        {"testargs", "(DEBUG)Imprime en pantalla argc y argv", argTest},
        {"testprocesses", "(DEBUG)Test para procesos", test_processes},
        {"testprio", "(DEBUG)Test para prioridad de procesos", test_prio},
        {"kill","Mata un proceso dado su ID.",killProcess},
        {"nice","Cambia la prioridad de un proceso dado su ID y la nueva prioridad.",niceProcess},
        {"block"," Cambia el estado de un proceso entre bloqueado y listo dado su ID.",blockProcess},
        {"ps"," Imprime la lista de todos los procesos con sus propiedades.",ps},
        {"loop","  Imprime su ID con un saludo cada una determinada cantidad de segundos.",loopProcess}
};


void shellStart(){
    screenInfo(&scr_width, &scr_height);
    textPosition(0, scr_height);
    colorPrint("Bienvenido!\n\n");
    println("Que modulo desea correr?\n");
    help();
    println("\nPara correr los modulos, ingrese el comando correspondiente y presione enter.");
    while(1){
        colorPrint("$ ");
        char readbuf[READBUF_LENGTH] = {0};
        scan(readbuf, READBUF_LENGTH);
        processCommand(readbuf);
    }
}

void processCommand(char * readbuf){
    char command[MAX_NAME_LENGTH];
    int commandIndex = -1;
    int validCommand = 0;
    int i = 0;
    for(; i<MAX_NAME_LENGTH; i++){
        if( readbuf[i] == 0 || readbuf[i]==' '){
            command[i] = 0;
            validCommand = 1;
            break;
        } else {
            command[i] = readbuf[i];
        }
    }
    if (validCommand){
        commandIndex = searchCommand(command);
    }

    if (commandIndex >=0){
        int argc = 0;
        char args[MAX_ARG_COUNT][READBUF_LENGTH];
        char * argbuf = readbuf + i;
        char * currentArg = strtok(argbuf, ' ');
        while(currentArg != NULL){
            strcpy(currentArg, args[argc]);
            argc++;
            currentArg = strtok(NULL, ' ');
        }
        char *argv[MAX_ARG_COUNT];
        for(int i =0; i<MAX_ARG_COUNT; i++){
            argv[i] = args[i];
        }
        commandList[commandIndex].function(argc, argv);
    } else {
        print(command);
        println(": comando invalido");
    }
}

int searchCommand(char * command){
    for(int i = 0; i<COMMAND_LIST_LENGTH; i++){
        if (strcmp(command, commandList[i].name)==0){
            return i;
        }
    }
    return -1;
}


void help(){
    int spaces;
    char buffer[MAX_NAME_LENGTH];
    for(int i=0; i< COMMAND_LIST_LENGTH; i++){
        toUpper(buffer, commandList[i].name);
        colorPrint(buffer);
        spaces = MAX_NAME_LENGTH - strlen(commandList[i].name);
        for(int i = 0; i<spaces; i++){
            putChar(' ');
        }
        println(commandList[i].desc);
    }
}
void argTest(int argc, char ** argv){
    print("argc: ");
    char argcBuf[10];
    intToString(argc, argcBuf, 10);
    println(argcBuf);
    print("argv: {");
    for(int i=0; i<argc; i++){
        print(argv[i]);
        if (i+1<argc) {
            print(", ");
        }
    }
    println("}");
}


void time(){
    timeStruct time;
    char buffer[5];
    print("Fecha y hora actual: ");
    getTime(&time);
    intToString(time.dayOfMonth, buffer, 10);
    print(buffer);
    putChar('-');
    intToString(time.month,buffer,10);
    print(buffer);
    putChar('-');
    intToString(time.year, buffer, 10);
    print(buffer);
    putChar(' ');
    time.hour = (time.hour + 21)%24;
    intToStringL(time.hour, buffer, 10, 2);
    print(buffer);
    putChar(':');
    intToStringL(time.minute, buffer, 10, 2);
    print(buffer);
    putChar('\n');
}


static char * regNames[] = {"RAX", "RBX", "RCX", "RDX", "RBP","RSI", "RDI", "R8 ", "R9 ", "R10",
                            "R11", "R12", "R13", "R14", "R15", "RSP", "RIP", "RFLAGS"};


static void printSingleReg(char * regName, uint64_t reg, char * buffer){
    uint64_t nameLen = strlen(regName);
    print(regName);
    putChar(':');
    for(int i = 7-nameLen; i>0 ;i--){
        putChar(' ');
    }
    intToStringL(reg, buffer, 16, 16);
    print(buffer);
    putChar('\n');
}

void regStatus(){
    uint64_t regs[18]={0};
    char regBuffer[17];
    int status;
    getREGS(&status,regs);
    if (status) {
        for (int i = 0; i <= 17; i++) {
            printSingleReg(regNames[i], regs[i], regBuffer);
        }
    }
    else{
        print("No hay un status de registros guardado. Puede guardar uno en cualquier momento apretando la tecla F1\n");
    }
}

typedef struct MM_rq {
    void * address;
    uint32_t size;
} mm_rq;

static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

uint32_t GetUint2() {
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;
}

uint32_t GetUniform2(uint32_t max) {
    uint32_t u = GetUint2();
    return (u + 1.0) * 2.328306435454494e-10 * max;
}


void * memset2(void * destination, int32_t c, uint64_t length)
{
    uint8_t chr = (uint8_t)c;
    char * dst = (char*)destination;

    while(length--)
        dst[length] = chr;

    return destination;
}

uint8_t memcheck2(void *start, uint8_t value, uint32_t size) {
    uint8_t *p = (uint8_t *) start;
    uint32_t i;

    for (i = 0; i < size; i++, p++)
        if (*p != value)
            return 0;

    return 1;
}

void testmm(int argc, char ** argv){
    mm_rq mm_rqs[128];
    uint64_t rq;
    uint32_t total;
    uint64_t max_memory = 128*128*512;
    int fIndex = 0;
    while(1){
        fIndex++;
        print("i");
        rq = 0;
        total = 0;
        while(rq < 128 && total < max_memory){
            mm_rqs[rq].size = GetUniform2(max_memory - total - 1) + 1;
            mm_rqs[rq].address = memAlloc(mm_rqs[rq].size);
            if(mm_rqs[rq].address){
                total += mm_rqs[rq].size;
                rq++;
            }
        }
        uint32_t i;
        for(i = 0; i<rq; i++){
            if (mm_rqs[i].address)
                memset2(mm_rqs[i].address, i, mm_rqs[i].size);
        }
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                if (!memcheck2(mm_rqs[i].address, i, mm_rqs[i].size)) {
                    print("test_mm ERROR f:");
                    printInt(fIndex);
                    print("\n");
                    return;
                }

        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                memFree(mm_rqs[i].address);
    }

}
/*
char * fn_aux(){
    char * aux = memAlloc(80);
    if (aux==NULL){
        print("NULL\n");
    }
    for (int i = 0; i<9; i++){
        aux[i]='a';
    }
    aux[9]=0;
    return aux;
}

void testmm(int argc, char ** argv){
    print("Memory before malloc:\n");
    memoryInfo();
    char * aux = fn_aux();
    print(aux);
    print("\nMemory after malloc:\n");
    memoryInfo();
    memFree(aux);
    print("Memory after free:\n");
    memoryInfo();
}*/