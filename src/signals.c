/*
 * signals.c - Signal handling routines
 *
 * This file implements signal handlers to manage:
 *   - SIGCHLD: Reaping terminated or stopped child processes.
 *   - SIGINT: Forwarding interrupt signals (Ctrl-C) to the foreground job.
 *   - SIGTSTP: Stopping the foreground job (Ctrl-Z).
 *   - SIGQUIT: Clean termination of the shell.
 *
 * Complete the implementation of each handler.
 */

#include "shell.h"

void sigchld_handler(int sig) {
  /* TODO: Reap zombie processes and update the job list accordingly */
}

void sigint_handler(int sig) {
  /* TODO: Forward the SIGINT signal to the foreground job */
}

void sigtstp_handler(int sig) {
  /* TODO: Handle SIGTSTP by stopping the foreground job */
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) {
  printf("Terminating after receipt of SIGQUIT signal\n");
  exit(1);
}
