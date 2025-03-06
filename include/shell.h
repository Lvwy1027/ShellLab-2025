#ifndef SHELL_H
#define SHELL_H

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Maximum constants */
#define MAXLINE 1024     /* Maximum command line length */
#define MAXARGS 128      /* Maximum number of arguments */
#define MAXJOBS 16       /* Maximum number of jobs */
#define MAXJID (1 << 16) /* Maximum job ID */

/* Job states */
#define UNDEF 0 /* Undefined */
#define FG 1    /* Foreground job */
#define BG 2    /* Background job */
#define ST 3    /* Stopped job */

/* Job structure used for job control */
typedef struct job {
  pid_t pid;             /* Job process ID */
  int jid;               /* Job ID */
  int state;             /* UNDEF, FG, BG, or ST */
  char cmdline[MAXLINE]; /* Command line */
} job_t;

/* Global job list */
extern job_t jobs[MAXJOBS];
extern int next_jid;

/* Global variables */
extern char prompt[]; /* Shell prompt string */
extern int verbose;   /* Verbose flag for debugging output */

/*
 * Command structure returned by the parser.
 * Supports:
 *   - argv: array of command and arguments.
 *   - argc: number of arguments.
 *   - bg: flag indicating background execution.
 *   - infile: input redirection file (if any).
 *   - outfile: output redirection file (if any).
 *   - append: flag for output redirection mode (append vs. truncate).
 *   - next: pointer to next command in a pipeline.
 *
 * Future implementations should add support for command substitution
 * and environment variable expansion.
 */
typedef struct command {
  char *argv[MAXARGS];  /* Command and arguments */
  int argc;             /* Number of arguments */
  char *infile;         /* Input redirection file */
  char *outfile;        /* Output redirection file */
  int append;           /* Append mode flag for output redirection */
  struct command *next; /* For pipeline: pointer to next command */
} command_t;

/* Main shell functions */
void shell_loop(int emit_prompt);
void eval(const char *cmdline);
void eval_script(const char *filename);

/* Parsing functions */
int parse_command_line(const char *cmdline, command_t **cmd, int *bg);
void free_command(command_t *cmd);

/* Built-in command functions */
int builtin_cmd(command_t *cmd);
void do_bgfg(command_t *cmd);
void do_cd(command_t *cmd);
void do_kill(command_t *cmd);

/* Signal Handlers */
void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sigquit_handler(int sig);

/* Job Control Functions */
/* Clear the entries in a job struct */
void clear_job(job_t *job);
/* Initialize the job list */
void init_jobs(job_t *jobs);
/* Return the largest allocated job ID */
int max_jid(job_t *jobs);
/* Add a job to the job list */
int add_job(job_t *jobs, pid_t pid, int state, const char *cmdline);
/* Delete a job whose PID=pid from the job list */
int delete_job(job_t *jobs, pid_t pid);
/* Return PID of current foreground job, 0 if no such job */
pid_t fg_pid(job_t *jobs);
/* Find a job (by PID) on the job list */
job_t *get_job_by_pid(job_t *jobs, pid_t pid);
/* Find a job (by JID) on the job list */
job_t *get_job_by_jid(job_t *jobs, int jid);
/* Map process ID to job ID */
int pid_to_jid(pid_t pid);
/* Print the job list */
void list_jobs(job_t *jobs);

/* Utility functions */
void unix_error(const char *msg);
void app_error(const char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
void usage(void);

#endif /* SHELL_H */
