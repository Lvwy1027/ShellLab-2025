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

void sigchld_handler(int sig) {  //回收已结束或暂停的子进程 
    int olderrno = errno;  //保存当前状态 
    int status;  //存储子进程的退出状态 
    pid_t pid; 
    
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {  //循环回收所有
        job_t *job = get_job_by_pid(jobs, pid);
        if (!job)
            continue;
            
        if (WIFEXITED(status))  //正常退出：删除作业
            delete_job(jobs, pid); 
    	else if (WIFSIGNALED(status)) {  //因信号终止：输出消息，再删除作业
            printf("Job [%d] (%d) terminated by signal %d\n", job->jid, job->pid, WTERMSIG(status));
            delete_job(jobs, pid);
        } 
		else if (WIFSTOPPED(status)) {  //被停止：输出消息，并更新状态
            job->state = ST;
            printf("Job [%d] (%d) stopped by signal %d\n", job->jid, job->pid, WSTOPSIG(status));
        }
    }
    errno = olderrno;  //恢复原始状态 
}

void sigint_handler(int sig) {  //终止前台作业 
    pid_t pid = fg_pid(jobs);
    if (pid > 0)
        kill(-pid, SIGINT); //发送到进程组
}

void sigtstp_handler(int sig) {  //暂停前台作业 
    pid_t pid = fg_pid(jobs);
    if (pid > 0)
        kill(-pid, SIGTSTP); 
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) {
  printf("Terminating after receipt of SIGQUIT signal\n");
  exit(1);
}
