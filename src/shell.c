/*
 * shell.c - Core shell loop and command evaluation
 *
 * This file contains the main shell loop and the evaluation routine.
 * The eval function is responsible for parsing the command line,
 * checking for built-in commands, and executing external commands.
 */

#include "shell.h"
#include <stdio.h>
#include <fcntl.h>  // For open() and file flags (O_RDONLY, O_WRONLY, etc.)
#include <unistd.h> // For dup2(), close(), etc.

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
    int bg = 0;  //0ǰ̨��1��̨ 
    
    if (parse_command_line(cmdline, &cmd, &bg) != 0)  //���������У����� command_t ����
        return;

   
    if (cmd == NULL || cmd->argv[0] == NULL) {  //������Ϊ����ֱ�ӷ���
        if (cmd) 
			free_command(cmd);
        return;
    }

    
    if (builtin_cmd(cmd)) {  //�ڽ�����ֱ���ڸ������д���
        free_command(cmd);
        return;
    }

    pid_t pid = fork();
    
    if (pid == 0) {  //�ӽ��̣�ִ���ⲿ����
    
    	setpgid(0, 0);  //���ӽ��̷ŵ��½����飬�����źŹ���	
    	signal(SIGINT, SIG_DFL);
    	signal(SIGTSTP, SIG_DFL);
   		signal(SIGCHLD, SIG_DFL);  //�ָ�Ĭ���źŴ���
	
    	if (cmd->infile) {  //�������� 
        	int fd = open(cmd->infile, O_RDONLY);  //�������ļ� 
        	if (fd < 0) {  //ʧ���� 
            	fprintf(stderr, "Error: could not open input file %s\n", cmd->infile);
            	exit(1);
        	}
	        dup2(fd, STDIN_FILENO);  //�����븴�Ƶ���׼���� 
	        close(fd);
   		}

	    if (cmd->outfile) {  //������� 
	        int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
	        int fd = open(cmd->outfile, flags, 0644);
	        if (fd < 0) {
	            fprintf(stderr, "Error: could not open output file %s\n", cmd->outfile);
	            exit(1);
	        }
	        dup2(fd, STDOUT_FILENO);
	        close(fd);
	    }

        if (execvp(cmd->argv[0], cmd->argv) < 0) {  //����execvpִ���ⲿ����
            fprintf(stderr, "%s: Command not found\n", cmd->argv[0]);
            exit(1);
        }
    } 
    
	else if (pid > 0) {  //�����̣�����ǰ̨/��̨��ҵ
        
        add_job(jobs, pid, bg ? BG : FG, cmdline);

        if (!bg) {  // ǰ̨��ҵ
            tcsetpgrp(STDIN_FILENO, pid);  //���ն˿���Ȩ�����ӽ����� 
            waitfg(pid);  //�ȴ�ǰ̨��ҵ��� 
            tcsetpgrp(STDIN_FILENO, getpgrp());  //���ն˿���Ȩ����shell 
        } 
		else {  // ��̨��ҵ
            job_t *job = get_job_by_pid(jobs, pid);
            printf("[%d] (%d) %s &\n", job->jid, job->pid, job->cmdline);
            fflush(stdout);
        }
    } 

    free_command(cmd);  //�ͷ��ڴ� 
}

void eval_script(const char *filename) {
  /* TODO: Implement script file execution */
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return;
    }

    char line[MAXLINE];
    while (fgets(line, sizeof(line), file)) {
        eval(line);
    }

    fclose(file);
}

void waitfg(pid_t pid) {
    while (fg_pid(jobs) == pid) {
        sleep(1);
    }
}

