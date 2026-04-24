#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
MAIN:
./cm --role {inspector/manager} --user {name} --{add, removeReport, list, view, updateThreshold, filter} {args}

if new district, create directory
each directory:
    reports.dat {saves: reportId, userName, cords, issueCategory, severity, timestamp, description}
    district.cfg {minimum lvl that triggers an alert}
    loggedDistrict {record every action in that district, timestamps, role, username}

add:    input: districtId
        usage: append new report
        who: both roles

remove: input: districtId, reportId

list:   input: districtId

view:   input: districtId, reportId

update: input: districtId, value

filter: input: districtId, condition
*/

typedef struct args{
    char role[10];
    char name[50];
    char op[20];
    int districtId;
    int reportId;
    int value;
    //char condition[][]; //matrice bidimensionala, trebuie sa initializez fiecare rand cu \0
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
    //input.condition[0] = '\0'; //!!!!!!!!!!!!!!!!!!!!!!!!!!!
    input.conditionCtr = 0;
}

int outOfBounds(int i, int argc){
    return i >= argc;
}

void arguments(int argc, char* argv){
    init();
    int roleExists = 0;
    int userExists = 0;
    int functionExists = 0;
    
    for(int i = 1; i < argc; i++){
        if((strcmp(argv[i], "--role") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.role, argv[i + 1]);
            roleExists = 1;
        }
        if((strcmp(argv[i], "--user") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.name, argv[i + 1]);
            userExists = 1;
        }
        if((strcmp(argv[i], "--add") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.op, "add");
            strcpy(input.districtId, argv[i + 1]);
            int functionExists = 1;
        }
        if((strcmp(argv[i], "--removeReport") == 0) && !outOfBounds(i + 1, argc) && !outOfBounds(i + 2, argc)){
            strcpy(input.op, "remove");
            strcpy(input.districtId, argv[i + 1]);
            strcpy(input.reportId, argv[i + 2]);
            int functionExists = 1;
        }
        if((strcmp(argv[i], "--list") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.op, "list");
            strcpy(input.districtId, argv[i + 1]);
            int functionExists = 1;
        }
        if((strcmp(argv[i], "--view") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.op, "view");
            strcpy(input.districtId, argv[i + 1]);
            strcpy(input.reportId, argv[i + 2]);
            int functionExists = 1;
        }
        if((strcmp(argv[i], "--updateThreshold") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.op, "update");
            strcpy(input.districtId, argv[i + 1]);
            strcpy(input.value, argv[i + 2]);
            int functionExists = 1;
        }
        if((strcmp(argv[i], "--filter") == 0) && !outOfBounds(i + 1, argc)){
            //moartea caprioarei, trebui sa gasesc args num si sa scriu in bimatrix fiecare arg
            int functionExists = 1;
        }

        if(roleExists + userExists + functionExists != 3){
            fprintf(stderr, "Failed to read, wrong args");
            exit(1);
        }
    }
}

int main(int argc, char* argv[]){
    if(argc < 5){
        fprintf(stderr, "Not enough args\n");
        exit(1);
    }


    return 0;
}