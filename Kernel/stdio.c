#include <stdio.h>

void read(uint64_t color, char *buffer, uint64_t length){
    if (color==STDIN){
        Process * current = getCurrentProcess();
        if(current->input != STDIN){
            read_from_pipe(current->input, buffer, length);
        } else {
            for(int i=0;i<length;i++){
                do {
                    buffer[i]=getKey();
                } while(buffer[i]==0);
            }
        }
    }
}

void write(uint64_t color, const char * string, uint64_t count){
    uint32_t charColor;
    switch(color){
        case STDOUT:
            charColor = 0xFFFFFF;
            break;
        case STDERR:
            charColor = 0xFF0000;
            break;
        case COLOR:
            charColor = 0x378805;
            break;
        default:
            return;
    }
    Process * current = getCurrentProcess();
    if(current->output != STDOUT){
        write_to_pipe(current->output, string, count);
    } else {
        if(current->foreground){
            for(int i=0;i<count;i++){
                putChar(charColor, 0, string[i]);
            }
        }
    }
}

void screenInfo(uint32_t * width, uint32_t * height){
    *height = getHeight();
    *width  = getWidth();
}


void putRectangle(uint32_t hexColor, uint32_t x, uint32_t y, uint32_t base, uint32_t height){
    uint32_t w = getWidth();
    uint32_t h = getHeight();
    if (x + base > w){
        base = w - x;
    }
    if (y + height > h){
        height = h - y;
    }
    for(int dx=0;dx<base;dx++){
        for(int dy=0;dy<height;dy++){
            putPixel(hexColor,dx+x,dy+y);
        }
    }
}


void putCircle(uint32_t hexColor, uint32_t x, uint32_t y, uint8_t radius){
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            if ((i*i) + (j*j) <= (radius*radius)) {
                putPixel(hexColor,j + x, i + y);
            }
        }
    }
    return;
}
