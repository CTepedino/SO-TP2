#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <memoryManager.h>
#include <idtLoader.h>
#include <math.h>
#include <scheduler.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

static void * const heapBaseAddress = (void*)0x600000;
#define HEAP_SIZE 0x10000000

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
    void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}

void * getSampleCodeModuleAddress(){
    return sampleCodeModuleAddress;
}

void writeMatrix(uint32_t x, uint32_t y, uint32_t width, uint32_t height,const char matrix[height][width]);

int main(){

    load_idt();
    setTimeFormat();
    
    initializeMemoryManager(heapBaseAddress, HEAP_SIZE);

	initScheduler();

    ((EntryPoint) sampleCodeModuleAddress)();
    return 0;
}
