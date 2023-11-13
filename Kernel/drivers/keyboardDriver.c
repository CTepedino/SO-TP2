#include <keyboardDriver.h>
#include <lib.h>

#define SHIFTED 0x80

static char keyboard_map[] = {
        '\0', ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', '\0', '*', '\0', ' ', '\0'
};

static char keyboard_shift_map[] = {
        '\0', ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
         '+', '\b', ' ', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
         '{', '}', '\n', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
         ';', '"', '\0', '\0', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
         '>', '?', '\0', '*', '\0', ' ', '\0'
};

int semId = 0;


char buffer[BUFFER_LEN];
int first;
int size;
int ctrlPressed;
int shiftPressed;

char releaseBuffer[BUFFER_LEN];
int r_last;

int isKeyPress(uint8_t code){
    return (code & 0x80) == 0;
}

int getBufferIndex(int offset) {
    return (first + offset) % (BUFFER_LEN);
}

void initializeKeyboardHandler(){
    semId = openNewSem(0);
}


void keyboard_handler(uint64_t * RSP){
    uint64_t code = readKey();

    if (code==F1_SCANCODE){
        saveREGS(RSP);
    }

    if (isKeyPress(code)){
        if (code == CTRL){
            ctrlPressed = 1;
        } else if (code == SHIFT){
            shiftPressed = 1;
        } else if (ctrlPressed){
            code = code < KEYMAP_LEN ? keyboard_map[code] : 0;
            if (code=='c'){
                cleanBuffer();
                killForegroundProcess();
            } else if (code=='d' && size < BUFFER_LEN-1){
                writeKey(EOF);
            }
        } else if (size < BUFFER_LEN-1){
            if (shiftPressed){
                code = SHIFTED | code;
            }
            writeKey(code);
        }
    } else {
        code = code & 0x7F;
        if(code==CTRL){
            ctrlPressed=0;
        } else if (code==SHIFT){
            shiftPressed=0;
        } else {
            if (shiftPressed){
                code =  code < KEYMAP_LEN ? keyboard_shift_map[code] : 0;
            } else {
                code = code < KEYMAP_LEN ? keyboard_map[code] : 0;
            }
            if (code) {
                if (r_last > BUFFER_LEN) {
                    r_last = 0;
                }
                releaseBuffer[r_last++] = code;
            }
        }

    }
}

void cleanBuffer(){
    first=0;
    size=0;
}

int8_t getScancode() {
    if (size > 0) {
        int8_t c = buffer[getBufferIndex(0)];
        first = getBufferIndex(1);
        size--;
        return c;
    }
    return 0;
}

int8_t getAscii() {
    waitSem(semId);
    int scanCode = getScancode();
    if (scanCode == EOF)
        return EOF;
    if (SHIFTED & scanCode) {
        scanCode &= 0x7F;
        return keyboard_shift_map[scanCode];
    }
    return keyboard_map[scanCode];
}


void getAllKeys(char * c){
    for(int i = 0; i < size; i++){
        c[i] = buffer[first+i];
    }
    c[size]=0;
    cleanBuffer();
}

void getReleasedKeys(char * c){
    for(int i = 0; i < r_last; i++){
        c[i] = releaseBuffer[i];
    }
    c[r_last]=0;
    r_last = 0;
}

void writeKey(char key){
    if (((key & 0x7F) < sizeof(keyboard_map) &&  keyboard_map[key & 0x7F] != 0) || (int) key == EOF) {
        buffer[getBufferIndex(size)] = key;
        size++;
        postSem(semId);
    }
}




