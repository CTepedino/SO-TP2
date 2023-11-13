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
    //executeCommand(commandList[0], 0, NULL, 1);
    println("\nPara correr los modulos, ingrese el comando correspondiente y presione enter.");
    while(1){
        colorPrint("$ ");
        char readbuf[READBUF_LENGTH] = {0};
        scan(readbuf, READBUF_LENGTH);
        processCommand(readbuf);
    }
}

void processCommand(char * readbuf){
    char cmd1[MAX_NAME_LENGTH];
    int commandIndex = -1;
    int validCommand = 0;
    int i = 0;
    int foreground = 1;
    int pipe = -1;
    for(; i<MAX_NAME_LENGTH; i++){
        if( readbuf[i] == 0 || readbuf[i]==' '){
            cmd1[i] = 0;
            validCommand = 1;
            break;
        } else {
            cmd1[i] = readbuf[i];
        }
    }
    if (validCommand){
        commandIndex = searchCommand(cmd1);
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
        for(int j=0; j<argc; j++){
            if (strcmp(args[j], "|")==0){
                if (pipe!=-1 || j==argc-1){
                    println("sintaxis invalida");
                    return;
                } else {
                    pipe = j;
                }
            }
            if (strcmp(args[j], "&")==0){
                if (j!=argc-1){
                    println("sintaxis invalida");
                    return;
                } else {
                    foreground=0;
                }
            }
        }
        if(pipe==-1) {
            char *argv[MAX_ARG_COUNT];
            for (i = 0; i < MAX_ARG_COUNT; i++) {
                argv[i] = args[i];
            }
            if (!foreground){
                argc--;
            }
            int fds[2];
            if(foreground){
                fds[0] = STDIN;
                fds[1] = STDOUT;
            } else {
                fds[0] = -1;
                fds[1] = STDOUT;
            }
            int pid = execve(commandList[commandIndex].function, commandList[commandIndex].name, argc, argv, 0, fds);
            if (foreground){
                waitForChildren(pid);
            }
        } else {
            char * cmd2;
            cmd2 = args[pipe+1];
            int commandIndex2;
            commandIndex2 = searchCommand(cmd2);
            if (commandIndex2==-1){
                print(cmd2);
                println(": comando invalido");
                return;
            } else {
                char *argv1[MAX_ARG_COUNT];
                char * argv2[MAX_ARG_COUNT];
                int argc1=0;
                int argc2=0;
                for(; argc1<pipe;argc1++){
                    argv1[argc1]=args[argc1];
                }
                for(int aux = argc1+2; aux<argc;aux++){
                    argv2[argc2]=args[aux];
                    argc2++;
                }
                if (!foreground){
                    argc2--;
                }
                int pipeId = createNewPipe();
                int fd[2] = {pipeId, STDOUT};
                execve(commandList[commandIndex].function, commandList[commandIndex].name, argc1, argv1, 0, fd);
                fd[0] = STDIN;
                fd[1] = pipeId;
                int pid =execve(commandList[commandIndex2].function, commandList[commandIndex2].name, argc2, argv2, 0, fd);
                if (foreground) {
                    waitForChildren(pid);
                }
            }
        }
    } else {
        print(cmd1);
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

void killShell(int argc, char ** argv){
    if (argc != 1){
        print("Usage: kill <pid>");
    }
    kill(my_atoi(argv[0]));
}

void nice(int argc, char ** argv){
    if(argc != 2){
        print("Usage: nice <pid> <priority>");
    }
    setPriority(my_atoi(argv[0]), my_atoi(argv[1]));
}

void block(int argc, char ** argv){
    if (argc != 1){
        print("Usage: kill <pid>");
    }
    blockProcess(my_atoi(argv[0]));
}