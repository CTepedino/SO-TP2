#include <stdio.h>

uint64_t fRead(char * buffer, uint64_t length){
    int fd = getCurrentProcess()->fds.input;
    if (fd == STDIN){
        return read(buffer, length);
    } else if (fd > DEFAULT_FD_COUNT){
        return readPipe(fd, buffer, length);
    }
    return 0;
}

uint64_t fWrite(uint64_t color, const char * string, uint64_t count){
    int fd = getCurrentProcess()->fds.output;
    if (fd==STDOUT){
        return write(color, string, count);
    } else if (fd > DEFAULT_FD_COUNT){
        return writePipe(fd, string, count);
    }
    return 0;
}


uint64_t read(char *buffer, uint64_t length){
    for(int i = 0; i< length ; i++){
        buffer[i] = getAscii();
        if (buffer[i]==EOF){
            return i+1;
        }
    }
    return length;
}

uint64_t write(uint64_t color, const char * string, uint64_t count){
    uint32_t charColor;
    switch(color){
        case RED:
            charColor = 0xFF0000;
            break;
        case GREEN:
            charColor = 0x378805;
            break;
        default:
            charColor = 0xFFFFFF;
            break;
    }
    for(int i=0;i<count;i++){
        putChar(charColor, 0, string[i]);
    }
    return count;
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
