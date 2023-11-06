#include <keyboardDriver.h>
#include <lib.h>

#define BUFFER_LEN 10
#define KEYMAP_LEN 59

static char keyboard_map[] = {
        '\0', ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', '\0', '*', '\0', ' ', '\0'
};

char buffer[BUFFER_LEN];
int first=0;
int last=0;
int keyPressed = 0;

char releaseBuffer[BUFFER_LEN];
int r_last = 0;

int isKeyPress(uint8_t code){
    return (code & 0x80) == 0;
}

void keyboard_handler(uint64_t * RSP){
    uint64_t code = readKey();
    if (code == F1_SCANCODE){
        saveREGS(RSP);
    }
    if (isKeyPress(code)){
        code = code < KEYMAP_LEN ? keyboard_map[code] : 0;
        if (code) {
            if (last > BUFFER_LEN){
                cleanBuffer();
            }
            buffer[last++] = code;
            keyPressed = 1;
            alter_from_state(AWAITING_INPUT, READY);
        }
    }
    else{
        //es un key release
        code = code & 0x7F;
        code = code < KEYMAP_LEN ? keyboard_map[code] : 0;
        if (code) {
            if (r_last > BUFFER_LEN){
                r_last=0;
            }
            releaseBuffer[r_last++] = code;
        }
    }

}

char getKey(){
    if (keyPressed==0){
        return 0;
    }
    char toReturn = buffer[first++];
    if (first == last){
        cleanBuffer();
    }
    return toReturn;
}

void getAllKeys(char * c){
    int usedSpace = last - first;
    for(int i = 0; i < usedSpace; i++){
        c[i] = buffer[first+i];
    }
    c[usedSpace]=0;
    cleanBuffer();
}

void cleanBuffer(){
    first = 0;
    last = 0;
    keyPressed = 0;
}

void getReleasedKeys(char * c){
    for(int i = 0; i < r_last; i++){
        c[i] = releaseBuffer[i];
    }
    c[r_last]=0;
    r_last = 0;
}
