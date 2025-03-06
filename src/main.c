/*
 * main.c - Entry point for the shell
 *
 * This file sets up the shell environment by parsing command-line options,
 * installing signal handlers, initializing the job list, and starting the shell
 * loop.
 *
 * Author: [Your Name]
 * Date: [Date]
 */

#include "shell.h"
#include <getopt.h>

char prompt[] = "tsh> "; /* Shell prompt */
int verbose = 0;

/* Global job list */
job_t jobs[MAXJOBS];

int main(int argc, char **argv) {
  int opt;
  char *script_file = NULL;
  int emit_prompt = 1; /* Default: emit prompt */

  /* Redirect stderr to stdout (so that driver will get all output
   * on the pipe connected to stdout) */
  dup2(1, 2);

  /* Process command-line options */
  while ((opt = getopt(argc, argv, "hvpc:")) != -1) {
    switch (opt) {
    case 'h':
      usage();
      break;
    case 'v':
      verbose = 1;
      break;
    case 'p':
      emit_prompt = 0; /* Suppress prompt for testing */
      break;
    case 'c':
      script_file = optarg; /* Batch mode: execute script file */
      emit_prompt = 0;
      break;
    default:
      usage();
    }
  }

  /* Install signal handlers */

  /* These are the ones you will need to implement */
  Signal(SIGINT, sigint_handler);   /* ctrl-c */
  Signal(SIGTSTP, sigtstp_handler); /* ctrl-z */
  Signal(SIGCHLD, sigchld_handler); /* Terminated or stopped child */

  /* This one provides a clean way to kill the shell */
  Signal(SIGQUIT, sigquit_handler);

  /* Initialize the job list */
  init_jobs(jobs);

  /* If a script file is provided, execute it in batch mode */
  if (script_file) {
    eval_script(script_file);
  } else {
    /* Run the interactive shell loop */
    shell_loop(emit_prompt);
  }

  return 0;
}
