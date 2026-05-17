#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void startMonitor(){
    pid_t hub_mon;

    hub_mon = fork();
    if(hub_mon < 0){
        fprintf(stderr, "Failed to start hub_mon!\n");
        exit(1);
    } else if(hub_mon == 0){
        
    } else{

    }
}

void calculateScores(){
    
}

int main(void){



    return 0;
}