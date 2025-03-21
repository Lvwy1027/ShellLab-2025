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

int builtin_cmd(command_t *cmd) {  //读取操作并执行 
    if (strcmp(cmd->argv[0], "quit") == 0)
        exit(0);
    else if (strcmp(cmd->argv[0], "cd") == 0)
        do_cd(cmd);
    else if (strcmp(cmd->argv[0], "kill") == 0)
        do_kill(cmd);
    else if (strcmp(cmd->argv[0], "bg") == 0 || strcmp(cmd->argv[0], "fg") == 0)
        do_bgfg(cmd);
    else if (strcmp(cmd->argv[0], "jobs") == 0)
        list_jobs(jobs);
    else
        return 0;
    return 1;
}

void do_bgfg(command_t *cmd) {
    if (cmd->argc != 2) {  //检验输入合理性 
        fprintf(stderr, "%s command requires PID or %%jobid argument\n", cmd->argv[0]);
        return;
    }

    job_t *job = NULL;
    if (cmd->argv[1][0] == '%') {  // 使用 job id 格式
        int jid;
        if (sscanf(cmd->argv[1], "%%%d", &jid) != 1) {  //判断是否为整数 
            fprintf(stderr, "%s: argument must be a PID or %%jobid\n", cmd->argv[0]);
            return;
    	}
        job = get_job_by_jid(jobs, jid);  //判断job是否存在 
        if (!job) {
            fprintf(stderr, "%%%d: No such job\n", jid);
            return;
        }
    } 
	else {  // 使用纯 PID 格式
        pid_t pid = atoi(cmd->argv[1]);  //字符串转化为整数 
        if (pid <= 0) {  //不是整数 
            fprintf(stderr, "%s: argument must be a PID or %%jobid\n", cmd->argv[0]);
            return;
        }
        job = get_job_by_pid(jobs, pid);
        if (!job) {
            fprintf(stderr, "(%d): No such process\n", pid);
            return;
        }
    }

    if (strcmp(cmd->argv[0], "bg") == 0) {
        if (kill(-job->pid, SIGCONT) < 0)  //恢复运行 
            fprintf(stderr, "bg: %s\n", strerror(errno));
        else {
            job->state = BG;  //设置状态为后台 
            printf("[%d] (%d) %s &\n", job->jid, job->pid, job->cmdline);
        }
    } 
	
	else if (strcmp(cmd->argv[0], "fg") == 0) {
        if (kill(-job->pid, SIGCONT) < 0) 
            fprintf(stderr, "fg: %s\n", strerror(errno));
        else {
            job->state = FG;  //fg命令无需额外输出，直接等待前台作业结束
            waitfg(job->pid);
        }
    }
}

void do_cd(command_t *cmd) {
    if (cmd->argc != 2) {
        fprintf(stderr, "Usage: cd <directory>\n");
        return;
    }
    if (chdir(cmd->argv[1]) < 0)  //切换工作目录 
        fprintf(stderr, "cd: %s\n", strerror(errno));
    else {
        char cwd[MAXLINE];  //初始化，存储工作目录 
        if (getcwd(cwd, sizeof(cwd)) != NULL)  //获取当前路径 
            setenv("PWD", cwd, 1);  //更换环境变量 
		else
            fprintf(stderr, "cd: getcwd error\n");
    }
}

void do_kill(command_t *cmd) {
    if (cmd->argc != 2) {
        fprintf(stderr, "kill command requires PID or %%jobid argument\n");
        return;
    }

    job_t *job = NULL;
    if (cmd->argv[1][0] == '%') {  // 参数为 %jobid 格式
        int jid;
        if (sscanf(cmd->argv[1], "%%%d", &jid) != 1) {
            fprintf(stderr, "kill: argument must be a PID or %%jobid\n");
            return;
        }
        job = get_job_by_jid(jobs, jid);
        if (!job) {
            fprintf(stderr, "%%%d: No such job\n", jid);
            return;
        }
    } 
	else {  // 参数为纯 PID 格式
        pid_t pid = atoi(cmd->argv[1]);
        if (pid <= 0) {
            fprintf(stderr, "kill: argument must be a PID or %%jobid\n");
            return;
        }
        job = get_job_by_pid(jobs, pid);
        if (!job) {
            fprintf(stderr, "(%d): No such process\n", pid);
            return;
        }
    }

    if (kill(-job->pid, SIGKILL) < 0) {
        fprintf(stderr, "(%d): No such process\n", job->pid);
    } 
	else {
        printf("Job [%d] (%d) terminated by signal %d\n", job->jid, job->pid, SIGKILL);
        delete_job(jobs, job->pid);
    }
}

