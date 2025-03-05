/*
 * builtins.c - Implementation of built-in shell commands
 *
 * This file implements built-in commands such as:
 *   - quit: Exit the shell.
 *   - cd: Change the current working directory.
 *   - jobs: List background and stopped jobs.
 *   - fg: Bring a background job to the foreground.
 *   - bg: Resume a stopped job in the background.
 *   - kill: Terminate a job.
 *
 * Complete the implementations as specified.
 */

#include "shell.h"

int builtin_cmd(command_t *cmd) {
  /* TODO: Determine if the command is a built-in command.
     If so, execute it and return 1; otherwise, return 0.
  */
  return 0;
}

void do_bgfg(command_t *cmd) {
  /* TODO: Implement the functionality for bg and fg commands.
     - For bg: resume a stopped job in the background.
     - For fg: bring a job to the foreground.
     Ensure proper signal handling and job control.
  */
}

void do_cd(command_t *cmd) {
  /* TODO: Implement the functionality for cd command.
     - Change the current working directory to the specified path.
     - Handle relative and absolute paths.
     - Update the PWD environment variable.
  */
}

void do_kill(command_t *cmd) {
  /* TODO: Implement the functionality for kill command.
     - Terminate a job with the specified job ID.
     - Handle both foreground and background jobs.
     - Send the SIGKILL signal to the job.
  */
}
