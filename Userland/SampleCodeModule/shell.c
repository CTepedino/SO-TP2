#include <shell.h>
#include <generalLib.h>
#include <stdint.h>
#include <string.h>
#include <pong.h>
#include <exceptionTesters.h>

unsigned int scr_width;
unsigned int scr_height;


const char* commands[] = {"help", "time", "dividebyzero", "invalidopcode", "regstatus", "pong", "testmm"};
const char* commands_desc[] = {
        "Despliega una lista con los programas disponibles.\n",
        "Imprime en pantalla la fecha y hora del sistema.\n",
        "Ejecuta un programa que divide por cero, causando una excepcion\n",
        "Ejecuta un programa que intenta realizar una operacion invalida, causando una excepcion\n",
        "Imprime en pantalla el ultimo estado de los registros guardado.\n",
        "Abre el juego Pong. El paddle izquierdo se controla con \'W\' y \'S\'.El derecho con \'I\' y \'K\'.\n",
        "Test para Memory Manager\n",
};

static void (*commands_functions[])() = {help, time, divideByZero, invalidOpCode, regStatus, pong, testmm};



void help(){
    int spaces;
    char buffer[20];
    for(int i = 0; i < COMMANDS_LENGTH; i++){
        toUpper(buffer,commands[i]);
        colorPrint(buffer);
        spaces = 20 - strlen(commands[i]);
        for(int i = 0 ; i<spaces;i++){
            putChar(' ');
        }
        print(commands_desc[i]);
    }
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



static int indexCommand(char* readbuf) {
    char *p = readbuf;
    for(int i=0; i<COMMANDS_LENGTH; i++) {
        if(strcmp(p,commands[i]) == 0){
            return i;
        }
    }
    return -1;
}



void shellStart() {

    screenInfo(&scr_width, &scr_height);
    textPosition(0, scr_height);

    colorPrint("Bienvenido!\n\n");
    print("Que modulo desea correr?\n\n");
    help();
    print("Para correr los modulos, ingrese el comando correspondiente y presione enter.\n\n");

    while(1) {
        colorPrint("$ ");
        char command[READBUF_LENGTH] = {0};
        scan(command,READBUF_LENGTH);

        int index;
        if((index = indexCommand(command))>=0) {
            commands_functions[index]();
        }
        else {
            print(command);
            print(": comando invalido\n");
        }
    }
}

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

void testmm(){
    print("Memory before malloc:\n");
    memoryInfo();
    char * aux = fn_aux();
    print(aux);
    print("\nMemory after malloc:\n");
    memoryInfo();
    memFree(aux);
    print("Memory after free:\n");
    memoryInfo();
}

