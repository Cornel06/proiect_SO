#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct args{
    char role[10];
    char name[50];
    char op[20];
    int districtId;
    int reportId;
    int value;
    char condition[10][128];
    int conditionCtr;
}args;

args input;

void init(){
    input.role[0] = '\0';
    input.name[0] = '\0';
    input.op[0] = '\0';
    input.districtId = -1;
    input.reportId = -1;
    input.value = -1;
    for(int i = 0; i < 10; i++){
        input.condition[i][0] = '\0';
    }
    input.conditionCtr = 0;
}

int outOfBounds(int i, int argc){
    return i >= argc;
}

void readArguments(int argc, char* argv[]){
    init();
    int roleExists = 0;
    int userExists = 0;
    int functionExists = 0;
    
    for(int i = 1; i < argc; i++){
        if((strcmp(argv[i], "--role") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.role, argv[i + 1]);
            roleExists = 1;
            i++;
        }
        if((strcmp(argv[i], "--user") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.name, argv[i + 1]);
            userExists = 1;
            i++;
        }
        if((strcmp(argv[i], "--add") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.op, "add");
            strcpy(input.districtId, argv[i + 1]);
            functionExists = 1;
            i++;
        }
        if((strcmp(argv[i], "--removeReport") == 0) && !outOfBounds(i + 1, argc) && !outOfBounds(i + 2, argc)){
            strcpy(input.op, "remove");
            strcpy(input.districtId, argv[i + 1]);
            strcpy(input.reportId, argv[i + 2]);
            functionExists = 1;
            i +=2;
        }
        if((strcmp(argv[i], "--list") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.op, "list");
            strcpy(input.districtId, argv[i + 1]);
            functionExists = 1;
            i++;
        }
        if((strcmp(argv[i], "--view") == 0) && !outOfBounds(i + 1, argc) && !outOfBounds(i + 2, argc)){
            strcpy(input.op, "view");
            strcpy(input.districtId, argv[i + 1]);
            strcpy(input.reportId, argv[i + 2]);
            functionExists = 1;
            i +=2;
        }
        if((strcmp(argv[i], "--updateThreshold") == 0) && !outOfBounds(i + 1, argc) && !outOfBounds(i + 2, argc)){
            strcpy(input.op, "update");
            strcpy(input.districtId, argv[i + 1]);
            strcpy(input.value, argv[i + 2]);
            functionExists = 1;
            i++;
        }
        if((strcmp(argv[i], "--filter") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.op, "filter");
            strcpy(input.districtId, argv[i + 1]);
            
            i += 2;
            
            while(!outOfBounds(i, argc) && strncmp(argv[i], "--", 2) != 0){
                strcpy(input.condition[input.conditionCtr], argv[i]);
                i++;
                input.conditionCtr++;
            }
            i--;
            functionExists = 1;
        }
    }
    if(roleExists + userExists + functionExists != 3){
            fprintf(stderr, "Failed to read, wrong args");
            exit(1);
    }
}

void addRep(){

}

void removeRep(){

}

void listRep(){

}

void viewRep(){

}

void updateRep(){

}

void filterRep(){

}

int main(int argc, char* argv[]){
    if(argc < 5){
        fprintf(stderr, "Not enough args\n");
        exit(1);
    }

    readArguments(argc, argv);

    if(strcmp(input.op, "add") == 0){
        addRep();
    } else if(strcmp(input.op, "remove") == 0){
        removeRep();
    } else if(strcmp(input.op, "list") == 0){
        listRep();
    } else if(strcmp(input.op, "view") == 0){
        viewRep();
    } else if(strcmp(input.op, "update") == 0){
        updateRep();
    } else if(strcmp(input.op, "filter") == 0){
        filterRep();
    }
    
    return 0;
}