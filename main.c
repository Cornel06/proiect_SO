#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct args{
    char role[16];
    char name[64];
    char op[32];
    char districtId[64];
    int reportId;
    int value;
    char condition[16][128];
    int conditionCtr;
}args;

args input;

void init(){
    input.role[0] = '\0';
    input.name[0] = '\0';
    input.op[0] = '\0';
    input.districtId[0] = '\0';
    input.reportId = -1;
    input.value = -1;
    for(int i = 0; i < 16; i++){
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
            if(strcmp(input.role, "manager") != 0 && strcmp(input.role, "inspector") != 0){
                fprintf(stderr, "Invalid role, you have to be either the manager or an inspector\n");
                exit(1);
            }
            roleExists = 1;
            i++;
        } else if((strcmp(argv[i], "--user") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.name, argv[i + 1]);
            userExists = 1;
            i++;
        } else if((strcmp(argv[i], "--add") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.op, "add");
            strcpy(input.districtId, argv[i + 1]);
            functionExists = 1;
            i++;
        } else if((strcmp(argv[i], "--removeReport") == 0) && !outOfBounds(i + 1, argc) && !outOfBounds(i + 2, argc)){
            strcpy(input.op, "remove");
            strcpy(input.districtId, argv[i + 1]);
            input.reportId = atoi(argv[i + 2]);
            functionExists = 1;
            i +=2;
        } else if((strcmp(argv[i], "--list") == 0) && !outOfBounds(i + 1, argc)){
            strcpy(input.op, "list");
            strcpy(input.districtId, argv[i + 1]);
            functionExists = 1;
            i++;
        } else if((strcmp(argv[i], "--view") == 0) && !outOfBounds(i + 1, argc) && !outOfBounds(i + 2, argc)){
            strcpy(input.op, "view");
            strcpy(input.districtId, argv[i + 1]);
            input.reportId = atoi(argv[i + 2]);
            functionExists = 1;
            i +=2;
        } else if((strcmp(argv[i], "--updateThreshold") == 0) && !outOfBounds(i + 1, argc) && !outOfBounds(i + 2, argc)){
            strcpy(input.op, "update");
            strcpy(input.districtId, argv[i + 1]);
            input.value = atoi(argv[i + 2]);
            functionExists = 1;
            i += 2;
        } else if((strcmp(argv[i], "--filter") == 0) && !outOfBounds(i + 1, argc)){
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

typedef struct report{
    int reportId;
    char name[64];
    float xCords;
    float yCords;
    char category[32];
    int severity;
    time_t timestamp;
    char description[128];
}report;

int ctrReports(){
    char path[128];
    snprintf(path, sizeof(path), "%s/reports.dat", input.districtId);

    FILE* f = fopen(path, "rb");
    if(f == NULL){
        return 0;
    }

    fseek(f, 0, SEEK_END);

    long totalBytes = ftell(f);
    fclose(f);

    return (int)(totalBytes/sizeof(report));
}

void addRep(){
    report newReport;
    float xCords = 0;
    float yCords = 0;
    int reportCount = ctrReports();

    newReport.reportId = reportCount + 1;
    strcpy(newReport.name, input.name);
    
    printf("Coordonates?\nX: ");
    scanf("%f", &xCords);
    printf("Y: ");
    scanf("%f", &yCords);
    
    newReport.xCords = xCords;
    newReport.yCords = yCords;
    
    char category[32];

    printf("Category (road/lighting/flooding/other): ");
    scanf("%s", category);

    if(strcmp(category, "road") != 0 && strcmp(category, "lighting") != 0 && strcmp(category, "flooding") != 0){
        strcpy(newReport.category, "other");
    } else{
        strcpy(newReport.category, category);
    }
    int severity = 0;

    printf("Severity (1/2/3): ");
    scanf("%d", &severity);
    if(severity < 1 || severity > 4){
        return;
    }

    char desc[128];
    printf("Description: ");
    scanf("%[^\n]", desc);
    strcpy(newReport.description, desc);

    time_t timestamp = time(NULL);
    newReport.timestamp = timestamp;

    char path[128];

    snprintf(path, sizeof(path), "%s", input.districtId);
    struct stat st_dir = {0};
    
    if(stat(path, &st_dir) == -1){
        if(mkdir(path, 0750) != 0){
            fprintf(stderr, "Failed to create directory\n");
            exit(1);
        }
    }
    
    FILE* f;

    snprintf(path, sizeof(path), "%s/reports.dat", input.districtId);
    f = fopen(path, "ab");
    if(f != NULL){
        fwrite(&newReport, sizeof(report), 1, f);
        fclose(f);
    } else{
        fprintf(stderr, "Failed reports.dat\n");
        exit(1);
    }
    if(reportCount == 0){
        snprintf(path, sizeof(path), "%s/district.cfg", input.districtId);
        f = fopen(path, "w");
        if(f != NULL){
            fprintf(f, "%d", 3);
            fclose(f);
        } else{
            fprintf(stderr, "Failed district.cfg\n");
            exit(1);
        }
    }
    snprintf(path, sizeof(path), "%s/logged_district", input.districtId);
    f = fopen(path, "a");
    if(f != NULL){
        fprintf(f, "%lld || %s || %s", (long long)timestamp, input.role, input.name);
        fclose(f);
    } else{
        fprintf(stderr, "Failed logged_district\n");
        exit(1);
    }
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

void detectAndExecute(){
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
}

int main(int argc, char* argv[]){
    if(argc < 5){
        fprintf(stderr, "Not enough args\n");
        exit(1);
    }

    readArguments(argc, argv);

    detectAndExecute();
    
    printf("%s %s %s %s\n", input.role, input.name, input.op, input.districtId);
    return 0;
}