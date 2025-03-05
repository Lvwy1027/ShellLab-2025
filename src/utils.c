/*
 * utils.c - Utility functions
 *
 * This file implements helper routines for error reporting, usage messages,
 * and a wrapper for signal handling.
 */

#include "shell.h"

void usage(void) {
  printf("Usage: shell [-hvp]\n");
  printf("   -h   Print this help message\n");
  printf("   -v   Enable verbose mode\n");
  printf("   -p   Do not print a command prompt\n");
  exit(1);
}

void unix_error(const char *msg) {
  fprintf(stdout, "%s: %s\n", msg, strerror(errno));
  exit(1);
}

void app_error(const char *msg) {
  fprintf(stdout, "%s\n", msg);
  exit(1);
}

handler_t *Signal(int signum, handler_t *handler) {
  struct sigaction action, old_action;

  action.sa_handler = handler;
  sigemptyset(&action.sa_mask); /* Block the signal during its handler */
  action.sa_flags = SA_RESTART; /* Restart system calls if possible */

  if (sigaction(signum, &action, &old_action) < 0)
    unix_error("Signal error");
  return old_action.sa_handler;
}
