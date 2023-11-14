#include <test_util.h>

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0  // TODO: Change as required
#define MEDIUM 5  // TODO: Change as required
#define HIGHEST 10 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void endless_loop_print_prio() {
    int64_t pid = getPid();

    while (1) {
        printInt(pid);
        bussy_wait(10000);
    }
}

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  int fds[2] = {STDIN, STDOUT};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++){
    pids[i] = execve(&endless_loop_print_prio, "endless_loop_print" , 1, (char**) argv, LOWEST, fds);
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