#include <test_util.h>

#define SEM_ID 42
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}


void my_process_inc(int argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3)
    return;

  if ((n = satoi(argv[0])) <= 0)
    return;
  if ((inc = satoi(argv[1])) == 0)
    return;
  if ((use_sem = satoi(argv[2])) < 0)
    return;

  if (use_sem)
    if (semOpen(SEM_ID, 1) < 0) {
      print("test_sync: ERROR opening semaphore\n");
      return;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      semWait(SEM_ID);
    slowInc(&global, inc);
    if (use_sem)
      semPost(SEM_ID);
  }

 /* if (use_sem)
    semClose(SEM_ID);*/

  return;
}

void test_sync(int argc, char ** argv) { //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2)
    return;
  int use_sem = satoi(argv[1]);
  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};
  int fds[2] = {STDIN, STDOUT};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = execve(&my_process_inc, "my_process_inc", 3, argvDec, 0, fds);
    pids[i + TOTAL_PAIR_PROCESSES] = execve(&my_process_inc, "my_process_inc", 3, argvInc,0, fds);

  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    waitForChildren(pids[i]);
    waitForChildren(pids[i + TOTAL_PAIR_PROCESSES]);
  }
  if (use_sem) {
      semClose(SEM_ID);
  }

  print("Final value: ");
  printInt(global);
  print("\n");
}