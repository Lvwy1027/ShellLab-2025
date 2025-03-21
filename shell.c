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
    int bg = 0;  //0前台，1后台 
    
    if (parse_command_line(cmdline, &cmd, &bg) != 0)  //解析命令行，构造 command_t 链表
        return;

   
    if (cmd == NULL || cmd->argv[0] == NULL) {  //若命令为空则直接返回
        if (cmd) 
			free_command(cmd);
        return;
    }

    
    if (builtin_cmd(cmd)) {  //内建命令直接在父进程中处理
        free_command(cmd);
        return;
    }

    pid_t pid = fork();
    
    if (pid == 0) {  //子进程，执行外部命令
    
    	setpgid(0, 0);  //将子进程放到新进程组，便于信号管理	
    	signal(SIGINT, SIG_DFL);
    	signal(SIGTSTP, SIG_DFL);
   		signal(SIGCHLD, SIG_DFL);  //恢复默认信号处理
	
    	if (cmd->infile) {  //处理输入 
        	int fd = open(cmd->infile, O_RDONLY);  //打开输入文件 
        	if (fd < 0) {  //失败了 
            	fprintf(stderr, "Error: could not open input file %s\n", cmd->infile);
            	exit(1);
        	}
	        dup2(fd, STDIN_FILENO);  //把输入复制到标准输入 
	        close(fd);
   		}

	    if (cmd->outfile) {  //处理输出 
	        int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
	        int fd = open(cmd->outfile, flags, 0644);
	        if (fd < 0) {
	            fprintf(stderr, "Error: could not open output file %s\n", cmd->outfile);
	            exit(1);
	        }
	        dup2(fd, STDOUT_FILENO);
	        close(fd);
	    }

        if (execvp(cmd->argv[0], cmd->argv) < 0) {  //调用execvp执行外部命令
            fprintf(stderr, "%s: Command not found\n", cmd->argv[0]);
            exit(1);
        }
    } 
    
	else if (pid > 0) {  //父进程，处理前台/后台作业
        
        add_job(jobs, pid, bg ? BG : FG, cmdline);

        if (!bg) {  // 前台作业
            tcsetpgrp(STDIN_FILENO, pid);  //将终端控制权交给子进程组 
            waitfg(pid);  //等待前台作业完成 
            tcsetpgrp(STDIN_FILENO, getpgrp());  //将终端控制权还给shell 
        } 
		else {  // 后台作业
            job_t *job = get_job_by_pid(jobs, pid);
            printf("[%d] (%d) %s &\n", job->jid, job->pid, job->cmdline);
            fflush(stdout);
        }
    } 

    free_command(cmd);  //释放内存 
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

