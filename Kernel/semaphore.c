// From: https://github.com/MSGronda/tp2-so

#include <semaphore.h>

#define MAX_SEMAPHORES 50
#define MAX_WAITING_PROCESS 20

typedef struct sem_record{
	uint64_t sem_id;
	unsigned int sem_value;
	unsigned int lock;

	ProcessQueueADT queue;
}sem_record;

static sem_record sem_info[MAX_SEMAPHORES] = {{0}};

static unsigned int active_sem = 0;

int find_sem(unsigned int sem_id){
	for(int i=0; i<MAX_SEMAPHORES; i++){
		if(sem_info[i].sem_id == sem_id){
			return i;
		}
	}
	return INVALID_SEM_ID;
}

int find_available_sem_id(){
	if(active_sem == MAX_SEMAPHORES)
		return ERROR_NO_MORE_SPACE;

	uint8_t found = 0;
	int sem_id = 10;

	while(!found){
		found = 1;
		for(int i=0; i<MAX_SEMAPHORES; i++){
			if(sem_info[i].sem_id == sem_id){
				found = 0;
				sem_id++;
				break;
			}
		}
	}
	return sem_id;
}

void lock(unsigned int * lock){
	while(_xchg(lock, 1) != 0);
}


void unlock(unsigned int * lock){
	_xchg(lock,0);
}


int open_sem_available(unsigned int value){
	int id = find_available_sem_id();

	if(id == ERROR_NO_MORE_SPACE){
		return ERROR_NO_MORE_SPACE;
	}

	if( open_sem(id, value) != SUCCESS){
		return ERROR_NO_MORE_SPACE;
	}
	return id;
}

int open_sem(unsigned int sem_id, unsigned int value){
	if(sem_id == 0)
		return INVALID_SEM_ID;
	if(active_sem == MAX_SEMAPHORES)
		return ERROR_NO_MORE_SPACE;

	int freePos = -1;
	for(int i=0; i<MAX_SEMAPHORES; i++){
		if(freePos == -1 && sem_info[i].sem_id == 0){
			freePos = i;
		}
		if(sem_info[i].sem_id == sem_id){
			return INVALID_SEM_ID;
		}
	}

	sem_info[freePos].queue = initializeProcessQueue();

	sem_info[freePos].sem_id = sem_id;
	sem_info[freePos].sem_value = value;

	active_sem++;

	return SUCCESS;
}

int close_sem(unsigned int sem_id){
	int pos = find_sem(sem_id);
	if(pos == INVALID_SEM_ID)
		return INVALID_SEM_ID;

	lock(&(sem_info[pos].lock));
	sem_info[pos].sem_id = 0;
	sem_info[pos].sem_value = 0;

	freeQueue(sem_info[pos].queue);

	unlock(&(sem_info[pos].lock));

	return SUCCESS;
}

int wait_sem(unsigned int sem_id){
	int pos = find_sem(sem_id);
	if(pos == INVALID_SEM_ID)
		return INVALID_SEM_ID;

	lock(&(sem_info[pos].lock));
	if(sem_info[pos].sem_value > 0){
		sem_info[pos].sem_value--;
	}
	else{
		int pid = getCurrentPid();
		blockProcess(pid);
		enqueue(sem_info[pos].queue, findProcessInQueue(sem_info[pos].queue, pid));

		unlock(&(sem_info[pos].lock));
		yield();
		return SUCCESS;
	}


	unlock(&(sem_info[pos].lock));

	return SUCCESS;
}

int post_sem(unsigned int sem_id){
	int pos = find_sem(sem_id);
	if(pos == INVALID_SEM_ID){
		return INVALID_SEM_ID;
	}

	lock(&(sem_info[pos].lock));
	if(!isEmpty(sem_info[pos].queue)){	

		Process *blocked_process = dequeue(sem_info[pos].queue);
		unblockProcess(blocked_process->pid);
	}
	else{
		sem_info[pos].sem_value++;
	}

	unlock(&(sem_info[pos].lock));
	return SUCCESS;
}