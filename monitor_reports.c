#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#define BUFFER_SIZE 64

volatile sig_atomic_t running = 1;

void signal_handler(int signal){
    if(signal == SIGINT){
        char message[] = "CAUGHT SIGINT! Interrupting program!\n";
        write(STDOUT_FILENO, message, sizeof(message) - 1);
        running = 0;
    } else if(signal == SIGUSR1){
        char message[] = "CAUGHT SIGUSR1! New report has been added to the district!\n";
        write(STDOUT_FILENO, message, sizeof(message) - 1);
    }
}

void setSigAction(){
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    
    act.sa_handler = &signal_handler;
    if(sigaction(SIGINT, &act, NULL) == -1 || sigaction(SIGUSR1, &act, NULL) == -1){
        fprintf(stderr, "Failed setting handler\n");
        exit(1);
    }
}

int main(){
    char path[BUFFER_SIZE];
    snprintf(path, sizeof(path), ".monitor_pid");

    int fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if(fd == -1){
        fprintf(stderr, "Couldn't create or open .monitor_pid!\n");
        exit(1);
    }

    char pid[BUFFER_SIZE];
    int bytesWritten = snprintf(pid, sizeof(pid), "%d\n", getpid());
    if(write(fd, pid, bytesWritten) != bytesWritten){
        fprintf(stderr, "Failed to write the PID inside the file!\n");
        close(fd);
        exit(1);
    }

    close(fd);

    setSigAction();

    while(running){
        pause();
    }
    
    if(unlink(path) == -1){
        fprintf(stderr, "Failed to delete .monitor_pid file!\n");
        exit(1);
    }
    return 0;
}