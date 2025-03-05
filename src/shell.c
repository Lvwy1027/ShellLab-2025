/*
 * shell.c - Core shell loop and command evaluation
 *
 * This file contains the main shell loop and the evaluation routine.
 * The eval function is responsible for parsing the command line,
 * checking for built-in commands, and executing external commands.
 */

#include "shell.h"

void shell_loop(int emit_prompt) {
  char cmdline[MAXLINE];

  while (1) {
    /* Print the command prompt */
    if (emit_prompt) {
      printf("%s", prompt);
      fflush(stdout);
    }

    /* Read a command line */
    if (fgets(cmdline, MAXLINE, stdin) == NULL) {
      if (ferror(stdin)) {
        app_error("fgets error");
      }
      if (feof(stdin)) { /* End-of-file (Ctrl-D) */
        fflush(stdout);
        exit(0);
      }
      continue;
    }

    /* Evaluate the command line */
    eval(cmdline);
    fflush(stdout);
    fflush(stdout);
  }
}

void eval(const char *cmdline) {
  command_t *cmd = NULL;

  int bg = 0;

  /* Parse the command line into a command_t structure */
  if (parse_command_line(cmdline, &cmd, &bg) != 0) {
    /* Handle parsing error (if any) */
    return;
  }

  /* If no command was entered, return */
  if (cmd == NULL || cmd->argv[0] == NULL)
    return;

  /* Execute built-in commands if applicable */
  if (builtin_cmd(cmd))
    return;

  /* Execute external command(s), including handling of:
     - Process creation with proper process group management
     - I/O redirection (< and >)
     - Single pipeline (|) if present
     - Command substitution and environment variable expansion

     TODO: Implement execution of external commands.
  */

  /* Free any allocated memory for the command structure, if needed */
  /* TODO: Free command_t structure resources if dynamically allocated */
}
