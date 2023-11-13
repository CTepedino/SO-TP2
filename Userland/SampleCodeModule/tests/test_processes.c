#include <test_util.h>

enum State { RUNNING,
             BLOCKED,
             KILLED };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;

void test_processes(int argc, char **argv) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {0};
  int fds[2] = {STDIN, STDOUT};

  if (argc != 1){
    print("Usage: testprocesses <max_processes>\n");
    killCurrent();
  }

  if ((max_processes = satoi(argv[0])) <= 0)
    killCurrent();

  print("Argument sent: ");
  printInt(max_processes);  

  p_rq p_rqs[max_processes];

  while (1) {

    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      execve(endless_loop_print, "endless_loop_print", 1, (char**) argvAux, 0, fds);
      p_rqs[rq].pid = getPid();

      if (p_rqs[rq].pid == 0) {
        print("test_processes: ERROR creating process\n");
        killCurrent();
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              kill(p_rqs[rq].pid);
              p_rqs[rq].state = KILLED;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING) {
              blockProcess(p_rqs[rq].pid);
              p_rqs[rq].state = BLOCKED;
            }
            break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          unblockProcess(p_rqs[rq].pid);
          p_rqs[rq].state = RUNNING;
        }
    }
  }
  killCurrent();
}