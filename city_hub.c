#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void writeMsg(int fd, char buff[]){
    write(fd, buff, sizeof(buff) - 1);
}

void startMonitor(){
    int pipefd[2];

    if(pipe(pipefd) == -1){
        writeMsg(STDERR_FILENO, "ERROR | Failed to create pipe!\n");
        exit(1);
    }

    pid_t monitorPid = fork();
    
    if(monitorPid < 0){
        writeMsg(STDERR_FILENO, "ERROR | Failed to execute monitor_reports!\n");
        exit(1);
    } else if(monitorPid == 0){
        
    } else{

    }
    
}

void calculateScores(){
    
}

int main(void){
    char command[128];
    char buffer[128];
    writeMsg(STDOUT_FILENO, "----City Hub----\nAvailable commands: start_monitor, exit\n");
    
    while(1){
        writeMsg(STDOUT_FILENO, "city_hub> ");

        if(fgets(command, sizeof(command), stdin) == NULL){
            break;
        }

        command[strcspn(command, "\n")] == '\0';
        if(strcmp(command, "start_monitor") == 0){
            pid_t hub_mon = fork();
            
            if(hub_mon < 0){
                writeMsg(STDERR_FILENO, "ERROR | Failed to create hub_mon!\n"); 
                exit(1);
            } else if(hub_mon == 0){
                startMonitor();
            } else{
                int len = snprintf(buffer, sizeof(buffer), "Started hub_mon with PID: %d\n", hub_mon);
                write(STDOUT_FILENO, buffer, len);
            }
        } else if(strcmp(command, "exit") == 0){
            writeMsg(STDOUT_FILENO, "Exiting city_hub!\n");
            break;
        } else if(strlen(command) > 0){
            int len = snprintf(buffer, sizeof(buffer), "ERROR | Unknown command: %s\n", command);
            write(STDOUT_FILENO, buffer, len);
        }
    }


    return 0;
}