#include <shell.h>

unsigned int scr_width;
unsigned int scr_height;


void processCommand(char * readbuf);
int searchCommand(char * command);

#define COMMAND_LIST_LENGTH 21

static Command commandList[] = {
        {"help", "Despliega una lista con los programas disponibles.", help},
        {"time", "Imprime en pantalla la fecha y hora del sistema.", time},
        {"dividebyzero", "Ejecuta un programa que divide por cero, causando una excepcion.", divideByZero},
        {"invalidopcode", "Ejecuta un programa que intenta realizar una operacion invalida, causando una excepcion.", invalidOpCode},
        {"regstatus", "Imprime en pantalla el ultimo estado de los registros guardado.", regStatus},
        {"pong", "Abre el juego Pong. El paddle izquierdo se controla con \'W\' y \'S\'.El derecho con \'I\' y \'K\'.", pong},
        {"testmm", "(DEBUG)Test para memory manager", test_mm},
        {"testprocess", "(DEBUG)Test para procesos", test_processes},
        {"testprio", "(DEBUG)Test para prioridad de procesos", test_prio},
        {"testsync", "(DEBUG)Test para sincronizacion", test_sync},
        {"testargs", "(DEBUG)Imprime en pantalla argc y argv", argTest},
        {"mem", "Imprime el estado de la memoria", memoryInfo},
        {"kill", "Mata un proceso dado su ID.", killShell},
        {"nice", "Cambia la prioridad de un proceso dado su ID y la nueva prioridad.", nice},
        {"block", "Cambia el estado de un proceso entre bloqueado y listo dado su ID.", block},
        {"ps", "Imprime la lista de todos los procesos con sus propiedades.", schedulerInfo},
        {"loop", "Imprime su ID con un saludo cada una determinada cantidad de segundos.", loop},
        {"wc", "Cuenta la cantidad de lineas del input", wc},
        {"cat", "Imprime el stdin tal como lo recibe", cat},
        {"filter", "Filtra las vocales del input", filter},
        {"phylo","Implementa el problema de los filósofos comensales", phylo}
};


void shellStart(){
    screenInfo(&scr_width, &scr_height);
    textPosition(0, scr_height);
    colorPrint("Bienvenido!\n\n");
    println("Que modulo desea correr?\n");
    executeCommand(commandList[0], 0, NULL, 1);
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
    int foreground = 1;
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
            if(!strcmp(currentArg, "&")){
                foreground = 0;
            } else {
                strcpy(currentArg, args[argc]);
                argc++;
            }
            currentArg = strtok(NULL, ' ');
        }
        char *argv[MAX_ARG_COUNT];
        for(int i =0; i<MAX_ARG_COUNT; i++){
            argv[i] = args[i];
        }
        executeCommand(commandList[commandIndex], argc, argv, foreground);
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

void executeCommand(Command command, int argc, char ** argv, int foreground){
    int fds[2];
    uint64_t childPid;
    if(foreground){
        fds[0] = STDIN;
        fds[1] = STDOUT;
    } else {
        fds[0] = STDIN;
        fds[1] = -1;
    }
    childPid = execve(command.function, command.name, argc, argv, 0, fds);
    if(foreground){
        waitForChildren(childPid);
    }
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
    killCurrent();
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
    killCurrent();
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
    killCurrent();
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
    killCurrent();
}

void killShell(int argc, char ** argv){
    if (argc != 1){
        print("Usage: kill <pid>");
    }
    kill(my_atoi(argv[0]));
    killCurrent();
}

void nice(int argc, char ** argv){
    if(argc != 2){
        print("Usage: nice <pid> <priority>");
    }
    setPriority(my_atoi(argv[0]), my_atoi(argv[1]));
    killCurrent();
}

void block(int argc, char ** argv){
    if (argc != 1){
        print("Usage: kill <pid>");
    }
    blockProcess(my_atoi(argv[0]));
    killCurrent();
}