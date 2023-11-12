#ifdef FREE_LIST

#include <memoryManager.h>

typedef struct Header{
	uint64_t size;
    void * address;
	struct Header * next;
	struct Header * prev;
} Header;

Header * freeList;
Header * takenList;
uint64_t totalMem;
uint64_t usedMem;

void toTakenList(Header * header);
void toFreeList(Header * header);


void initializeMemoryManager(void * initialAddress, uint64_t size){
    freeList = (Header *) initialAddress;
    freeList->size = size-sizeof(Header);
    freeList->address = initialAddress;
    freeList->next = NULL;
    freeList->prev = NULL;
    totalMem = size-sizeof(Header);
}

void * memAlloc(uint64_t size){
    if (size==0){
        return NULL;
    }

    Header * firstFit = freeList;

    while(firstFit != NULL){
        if (firstFit->size >= size+sizeof(Header)){
            void * ptr =(void *) ((uint64_t)firstFit->address + firstFit->size - size);
            firstFit->size -= (size+sizeof(Header));
            if (firstFit->size == 0){
                if (firstFit->prev != NULL){
                    firstFit->prev->next = firstFit->next;
                } else {
                    freeList = firstFit->next;
                }
                if (firstFit->next != NULL){
                    firstFit->next->prev = firstFit->prev;
                }
            }
            Header * taken = (Header *) ((uint64_t)ptr - sizeof(Header));
            taken->address = ptr;
            taken->size = size;
            taken->next = NULL;
            taken->prev = NULL;
            toTakenList(taken);

            usedMem += size + sizeof(Header);
            return ptr;
        }
        firstFit = firstFit->next;
    }
    return NULL;
}

void memFree(void * ptr){
    if (ptr==NULL){
        return;
    }
    Header * toFree = takenList;
    while(toFree != NULL){
        if (toFree->address == ptr){
            usedMem -= (toFree->size + sizeof(Header));
            if (toFree->prev != NULL){
                toFree->prev->next = toFree->next;
            } else {
                takenList = toFree->next;
            }
            if (toFree->next != NULL){
                toFree->next->prev = toFree->prev;
            }
            toFreeList(toFree);
            return;
        }
        toFree = toFree->next;
    }
}

void memoryInfo(){
    printString("Free List Memory Manager\n");
    printString("Total: ");
    printInt(totalMem);
    printString("\nDisponible: ");
    printInt(totalMem - usedMem);
    printString("\nEn uso: ");
    printInt(usedMem);
    printString("\n");
}


void toTakenList(Header * header){

    Header * curr = takenList;
    Header * prev = NULL;
    header->next = NULL;
    header->prev = NULL;

    if (curr == NULL){
        takenList = header;
    } else {
        while(curr != NULL && curr < header){
            prev = curr;
            curr = curr->next;
        }
        if (curr==NULL){
            if (prev != NULL){
                prev->next = header;
            }
            header->prev = prev;
        } else {
            header->prev = prev;
            if (prev != NULL){
                prev->next = header;
            } else {
                takenList = header;
            }
            header->next = curr;
            curr->prev = header;
        }
    }
}

void toFreeList(Header * header){

    Header * curr = freeList;
    Header * prev = NULL;
    header->next = NULL;
    header->prev = NULL;

    if (curr==NULL){
        freeList = header;
    } else {
        while(curr != NULL && curr < header){
            prev = curr;
            curr = curr->next;
        }
        if (curr==NULL){
            if (prev != NULL){
                prev->next = header;
            }
            header->prev = prev;
            return;
        }
        header->prev = prev;
        if (prev!= NULL){
            prev->next = header;
        } else {
            freeList = header;
        }
        header->next = curr;
        curr->prev = header;
        Header * aux = header;
        while (prev != NULL && (Header*)((uint64_t)prev->address+prev->size)==aux){
            prev->size += aux->size + sizeof(Header);
            prev->next = aux->next;
            if (aux->next != NULL){
                aux->next->prev = prev;
            }
            aux = prev;
            prev = prev->prev;
        }
        while(aux->next != NULL && (Header*)((uint64_t)aux->address+aux->size)==aux->next){
            aux->size += aux->next->size + sizeof(Header);
            aux->next = aux->next->next;
            if (aux->next != NULL){
                aux->next->prev = aux;
            }
            aux = aux->next;
        }
    }

}



#endif