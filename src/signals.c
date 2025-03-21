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

void sigchld_handler(int sig) {  //�����ѽ�������ͣ���ӽ��� 
    int olderrno = errno;  //���浱ǰ״̬ 
    int status;  //�洢�ӽ��̵��˳�״̬ 
    pid_t pid; 
    
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {  //ѭ����������
        job_t *job = get_job_by_pid(jobs, pid);
        if (!job)
            continue;
            
        if (WIFEXITED(status))  //�����˳���ɾ����ҵ
            delete_job(jobs, pid); 
    	else if (WIFSIGNALED(status)) {  //���ź���ֹ�������Ϣ����ɾ����ҵ
            printf("Job [%d] (%d) terminated by signal %d\n", job->jid, job->pid, WTERMSIG(status));
            delete_job(jobs, pid);
        } 
		else if (WIFSTOPPED(status)) {  //��ֹͣ�������Ϣ��������״̬
            job->state = ST;
            printf("Job [%d] (%d) stopped by signal %d\n", job->jid, job->pid, WSTOPSIG(status));
        }
    }
    errno = olderrno;  //�ָ�ԭʼ״̬ 
}

void sigint_handler(int sig) {  //��ֹǰ̨��ҵ 
    pid_t pid = fg_pid(jobs);
    if (pid > 0)
        kill(-pid, SIGINT); //���͵�������
}

void sigtstp_handler(int sig) {  //��ͣǰ̨��ҵ 
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
