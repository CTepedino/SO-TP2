#include <test_util.h>

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 4  // TODO: Change as required
#define HIGHEST 8 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++){
    execve(&endless_loop, "endless_loop_print" , 1, (char**) argv, LOWEST);
    pids[i] = getPid();
  }

  bussy_wait(WAIT);
  print("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    setPriority(pids[i], prio[i]);

  bussy_wait(WAIT);
  print("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    blockProcess(pids[i]);

  print("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    setPriority(pids[i], MEDIUM);

  print("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    unblockProcess(pids[i]);

  bussy_wait(WAIT);
  print("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    kill(pids[i]);
}