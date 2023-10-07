#include <videoDriver.h>


struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

typedef struct textPosition{
    uint32_t x;
    uint32_t y;
} textPosition;

textPosition tp = {.x = 0, .y = 0};


void putPixel(uint32_t hexColor, uint32_t x, uint32_t y) {
    if (x<VBE_mode_info->width && y < VBE_mode_info->height){
        uint8_t *screen = (uint8_t * )((uint64_t)(VBE_mode_info->framebuffer));
        uint32_t offset = VBE_mode_info->pitch * y + x * 3;
        screen[offset] = hexColor & 0xFF;
        screen[offset + 1] = (hexColor >> 8) & 0xFF;
        screen[offset + 2] = (hexColor >> 16) & 0xFF;
    }
}

void putChar(uint32_t hexColorChar, uint32_t hexColorBG, char c){
    if (c == '\b'){
        putBackSpace();
    }
    else if (c == '\t'){
        for(int i=0; i<4; i++) {
            putChar(hexColorChar, hexColorBG, ' ');
        }
    }
    else if (c == '\n'){
        putNewLine();
    }
    else if (c != ESC){
        if (tp.x + CHAR_WIDTH > VBE_mode_info->width) {
            tp.x = 0;
            tp.y += CHAR_HEIGHT;
        }
        if (tp.y + CHAR_HEIGHT > VBE_mode_info->height) {
            scrollUpwards();
            tp.x = 0;
            tp.y -= CHAR_HEIGHT;
        }
        unsigned char *cPixelMap = pixel_map(c);
        for (int y = 0; y < CHAR_HEIGHT; y++) {
            for (int x = 0; x < CHAR_WIDTH; x++) {
                if (cPixelMap[y] & (0b10000000 >> x)) {
                    putPixel(hexColorChar, tp.x + x, tp.y + y);
                } else {
                    putPixel(hexColorBG, tp.x + x, tp.y + y);
                }
            }
        }
        tp.x += CHAR_WIDTH;
    }
}

void putBackSpace(){
    if (tp.x==0 && tp.y==0){
        return;
    }
    if (tp.x == 0){
        tp.x = VBE_mode_info->width - CHAR_WIDTH;
        tp.y = tp.y - CHAR_HEIGHT;
    }
    else{
        tp.x = tp.x - CHAR_WIDTH;
    }
    for (int x=0; x<CHAR_WIDTH; x++){
        for(int y=0; y<CHAR_HEIGHT;y++){
            putPixel(0,x+tp.x,y+tp.y);
        }
    }
}


void putNewLine(){
    tp.x=0;
    if (tp.y + CHAR_HEIGHT > VBE_mode_info->height){
        scrollUpwards();
        return;
    }
    tp.y += CHAR_HEIGHT;
}

static void copyPixel(uint32_t srcX,uint32_t srcY,uint32_t dstX,uint32_t dstY){
    uint8_t *screen = (uint8_t * )((uint64_t)(VBE_mode_info->framebuffer));
    uint32_t offsetSrc = VBE_mode_info->pitch * srcY + srcX * 3;
    uint32_t offsetDst = VBE_mode_info->pitch * dstY + dstX * 3;

    screen[offsetDst] = screen[offsetSrc];
    screen[offsetDst+1] = screen[offsetSrc+1];
    screen[offsetDst+2] = screen[offsetSrc+2];
}

void scrollUpwards(){
    for (int y = CHAR_HEIGHT; y<VBE_mode_info->height; y++){
        for(int x = 0; x<VBE_mode_info->width;x++){
            copyPixel(x,y, x, y-CHAR_HEIGHT);
        }
    }
    for (int y = VBE_mode_info->height-CHAR_HEIGHT; y<VBE_mode_info->height;y++){
        for(int x = 0; x<VBE_mode_info->width;x++){
            putPixel(0, x, y);
        }
    }
}

int getWidth(){
    return VBE_mode_info->width;
}

int getHeight(){
    return VBE_mode_info->height;
}

void setTextPosition(uint32_t x, uint32_t y){
    tp.x = x < VBE_mode_info->width ? x : VBE_mode_info->width;
    tp.y = y < VBE_mode_info->height ? y : VBE_mode_info->height;
}

