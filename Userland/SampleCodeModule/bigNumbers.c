#include <bigNumbers.h>

char numbers_bitmap[] = {
        XXXX,
        X__X,
        X__X,
        X__X,
        X__X,
        X__X,
        XXXX,

        ___X,
        _XXX,
        ___X,
        ___X,
        ___X,
        ___X,
        ___X,

        XXXX,
        ___X,
        ___X,
        XXXX,
        X___,
        X___,
        XXXX,

        XXXX,
        ___X,
        ___X,
        XXXX,
        ___X,
        ___X,
        XXXX,

        X__X,
        X__X,
        X__X,
        XXXX,
        ___X,
        ___X,
        ___X,

        XXXX,
        X___,
        X___,
        XXXX,
        ___X,
        ___X,
        XXXX,

        XXXX,
        X___,
        X___,
        XXXX,
        X__X,
        X__X,
        XXXX,

        XXXX,
        ___X,
        ___X,
        _XXX,
        ___X,
        ___X,
        ___X,

        XXXX,
        X__X,
        X__X,
        XXXX,
        X__X,
        X__X,
        XXXX,

        XXXX,
        X__X,
        X__X,
        XXXX,
        ___X,
        ___X,
        XXXX,
};

void drawBigNumber(uint8_t number, uint8_t size, uint32_t x, uint32_t y){
    number = number > 9 ? number : number%10;
    for(int i = 0 ; i< CHAR_HEIGHT;i++){
        for (int j = 0; j < CHAR_WIDTH; j++) {
            if (numbers_bitmap[number*CHAR_HEIGHT+i] & (0b1000 >> j)) {
                drawRectangle(x + j *size, y + i * size ,size,size);
            } else {
                clearRectangle(x + j*size, y+ i * size ,size,size);
            }
        }
    }
}