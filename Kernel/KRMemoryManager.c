#include "memoryManager.h"

typedef uint64_t Align;

typedef union header {
    struct {
        union header * ptr;
        uint32_t size;
    } s;
    Align x; //obliga a la alineacion de bloques
} Header;

static Header *base; //lista vacia para iniciar
static Header *freep = NULL; //inicio de una lista libre

static uint64_t limit;

void initializeMemoryManager(void * initialAddress, uint64_t size){
    base = (Header *) initialAddress;
    freep = base;
    limit = size;
    freep->s.size = size/sizeof(Header);
    freep->s.ptr = NULL;
}

void * memAlloc(uint32_t size){
    Header *p, *prevp = freep;
    uint32_t nunits;
    nunits = (size+sizeof(Header)-1)/sizeof(Header)+1;
    for (p=freep ; p!=NULL ; prevp=p, p=p->s.ptr){
        if (p->s.size >= nunits){ //suficiente
            if (p->s.size == nunits) { //exacto
                if (p == freep){
                    freep = p->s.ptr;
                } else {
                    prevp->s.ptr = p->s.ptr;
                }
            } else { //asigna la parte final
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            return (void *) (p+1);
        }
    }
    return NULL;
}

void memFree(void * ptr){
    if (ptr==NULL){
        return;
    }
    Header *bp, *p;
    bp = (Header *)ptr-1;
    if (bp < (Header *) base || bp >= (Header *)base + limit){
        return;
    }
    Header * prevp;
    for(p=freep; p!=NULL && p<bp ;p=p->s.ptr){
        prevp = p;
    }
    if (p!=NULL && ((bp+bp->s.size)==p)){
        bp->s.size += p->s.size;
        bp->s.ptr = p->s.ptr;
    } else {
        bp->s.ptr = p;
    }
    if (p == freep){
        freep = bp;
    } else {
        if (prevp + prevp->s.size == bp){
            prevp->s.size += bp->s.size;
            prevp->s.ptr = bp->s.ptr;
        } else {
            prevp->s.ptr = bp;
        }
    }
}

