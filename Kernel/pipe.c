#include <pipe.h>

#define MAX_PIPES 20
#define PIPE_SIZE 1024

typedef struct pipe_record{
	unsigned int pipe_id;

	int read_sem_id;
	int write_sem_id;

	unsigned int write_pos;
	unsigned int read_pos;
	uint8_t * pipe;
	unsigned int amount;

	uint8_t eof;
}pipe_record;

static pipe_record pipe_info[MAX_PIPES];
static unsigned int num_pipes = 0;

int find_pipe(unsigned int pipe_id){
	for(int i=0; i<MAX_PIPES; i++){
		if(pipe_info[i].pipe_id == pipe_id){
			return i;
		}
	}
	return INVALID_PIPE_ID;
}

int find_available_pipe_id(){
	if(num_pipes == MAX_PIPES)
		return ERROR_NO_MORE_SPACE;

	uint8_t found = false;
	int pipe_id = 10;

	while(!found){
		found = true;
		for(int i=0; i<MAX_PIPES; i++){
			if(pipe_info[i].pipe_id == pipe_id){
				found = false;
				pipe_id++;
				break;
			}
		}
	}
	return pipe_id;
}

int create_pipe_available(){
	int id = find_available_pipe_id();

	if(id == ERROR_NO_MORE_SPACE){
		return ERROR_NO_MORE_SPACE;
	}
	
	if( create_pipe(id) != SUCCESS){
		return ERROR_NO_MORE_SPACE;
	}
	

	return id;
}

int create_pipe(unsigned int pipe_id){
	if(pipe_id == 0)
		return INVALID_PIPE_ID;
	if(num_pipes == MAX_PIPES)
		return ERROR_NO_MORE_SPACE;

	int freePos = -1;
	for(int i=0; i<MAX_PIPES; i++){
		if(freePos == -1 && pipe_info[i].pipe_id == 0){
			freePos = i;
		}
		if(pipe_info[i].pipe_id == pipe_id){
			return INVALID_PIPE_ID;
		}
	}

	
	int sem_id1 = open_sem_available(0);
	int sem_id2 = open_sem_available(PIPE_SIZE);
	if(sem_id1 == INVALID_SEM_ID || sem_id2 == INVALID_SEM_ID){
		close_sem(sem_id1);
		close_sem(sem_id2);
		return ERROR_NO_MORE_SPACE;
	}
	
	pipe_info[freePos].pipe = memAlloc(PIPE_SIZE);
	if(pipe_info[freePos].pipe == NULL){
		close_sem(sem_id1);
		close_sem(sem_id2);
		return ERROR_NO_MORE_SPACE;
	}

	pipe_info[freePos].pipe_id = pipe_id;
	pipe_info[freePos].read_sem_id  = sem_id1;
	pipe_info[freePos].write_sem_id  = sem_id2;
	pipe_info[freePos].write_pos = 0;
	pipe_info[freePos].read_pos = 0;
	pipe_info[freePos].amount = 0;
	pipe_info[freePos].eof = 0;
	
	num_pipes++;

	return SUCCESS;
}


void destroy_pipe(unsigned int pipe_id){
	int pos = find_pipe(pipe_id);
	if(pos == INVALID_PIPE_ID)
		return;
	close_sem(pipe_info[pos].write_sem_id);	
	close_sem(pipe_info[pos].read_sem_id);
	memFree(pipe_info[pos].pipe);

	pipe_info[pos].read_sem_id  = 0;
	pipe_info[pos].write_sem_id  = 0;
	pipe_info[pos].pipe_id = 0;
	pipe_info[pos].write_pos = 0;
	pipe_info[pos].read_pos = 0;
	pipe_info[pos].amount = 0;
	pipe_info[pos].pipe = 0;

	num_pipes--;
}




void signal_eof(unsigned int pipe_id){
	int pos = find_pipe(pipe_id);
	if(pos == INVALID_PIPE_ID)
		return;

	pipe_info[pos].eof = 1;
}

int write_to_pipe(unsigned int pipe_id, const char * src, unsigned int count){
	int pos = find_pipe(pipe_id);
	if(pos == INVALID_PIPE_ID)
		return INVALID_PIPE_ID;
	
	for(int i=0; i<count; i++){
		wait_sem(pipe_info[pos].write_sem_id);

		pipe_info[pos].pipe[pipe_info[pos].write_pos] = src[i];
		pipe_info[pos].write_pos = (pipe_info[pos].write_pos + 1) % PIPE_SIZE;
		pipe_info[pos].amount++;

		post_sem(pipe_info[pos].read_sem_id);
	}
	return count;
}

int read_from_pipe(unsigned int pipe_id, char * dest, unsigned int count){
	int pos = find_pipe(pipe_id);
	if(pos == INVALID_PIPE_ID)
		return INVALID_PIPE_ID;

	
	if(pipe_info[pos].eof && pipe_info[pos].amount == 0){
		return EOF;
	}
	int i=0;	
	for(; i<count && !(pipe_info[pos].eof && pipe_info[pos].amount == 0); i++){
		wait_sem(pipe_info[pos].read_sem_id);

		dest[i] = pipe_info[pos].pipe[pipe_info[pos].read_pos];
		pipe_info[pos].read_pos = (pipe_info[pos].read_pos + 1) % PIPE_SIZE;
		pipe_info[pos].amount--;
		
		post_sem(pipe_info[pos].write_sem_id);
	}
	return i;
}

