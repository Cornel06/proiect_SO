#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

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
            int val = atoi(argv[i + 2]);
            if(val < 1 || val > 3){
                fprintf(stderr, "Value out of bounds\n");
                exit(1);
            }
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
            fprintf(stderr, "Failed to read, wrong args\n");
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

    int f = open(path, O_RDONLY);
    if(f == -1){
        return 0;
    }

    off_t offset = lseek(f, -sizeof(report), SEEK_END);
    if(offset < 0){
        close(f);
        return 0;
    }
    report lastReport;
    read(f, &lastReport, sizeof(report));
    close(f);

    return lastReport.reportId;
}

int checkPermission(const char* path, int requiresWrite) {
    struct stat fileStat;
    
    if (stat(path, &fileStat) == -1) {
        return 1; 
    }

    if (strcmp(input.role, "manager") == 0) {
        if (requiresWrite){
            return (fileStat.st_mode & S_IWUSR) ? 1 : 0;  
        } else{
            return (fileStat.st_mode & S_IRUSR) ? 1 : 0;
        }
    } 
    else if (strcmp(input.role, "inspector") == 0) {
        if (requiresWrite){ 
            return (fileStat.st_mode & S_IWGRP) ? 1 : 0;
        } else{
            return (fileStat.st_mode & S_IRGRP) ? 1 : 0;
        }
    }
    return 0;
}

void permissionBitsSymbols(mode_t mode, char* str){
    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';
    str[3] = (mode & S_IRGRP) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
    str[5] = (mode & S_IXGRP) ? 'x' : '-';
    str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
    str[8] = (mode & S_IXOTH) ? 'x' : '-';
    str[9] = '\0'; 
}

void addRep(){
    char path[128];

    snprintf(path, sizeof(path), "%s", input.districtId);
    struct stat st_dir = {0};
    
    if(stat(path, &st_dir) == -1){
        if(mkdir(path, 0750) != 0){
            fprintf(stderr, "Failed to create directory\n");
            exit(1);
        }
        chmod(path, 0750);
    }

    report newReport;
    float xCords = 0;
    float yCords = 0;
    int reportCount = ctrReports();

    newReport.reportId = reportCount + 1;
    strcpy(newReport.name, input.name);
    
    printf("Coordonates:\nX: ");
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
    if(severity < 1 || severity > 3){
        fprintf(stderr, "Invalid severity\n");
        return;
    }
    newReport.severity = severity;

    char desc[128];
    printf("Description: ");
    scanf(" %[^\n]", desc);
    strcpy(newReport.description, desc);

    time_t timestamp = time(NULL);
    newReport.timestamp = timestamp;

    FILE* f;

    snprintf(path, sizeof(path), "%s/reports.dat", input.districtId);
    
    if(!checkPermission(path, 1)){
        fprintf(stderr, "Access denied!\n");
        exit(1);
    }

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
            chmod(path, 0640);
        } else{
            fprintf(stderr, "Failed district.cfg\n");
            exit(1);
        }
    }

    if(strcmp(input.role, "manager") == 0){
        snprintf(path, sizeof(path), "%s/logged_district", input.districtId);
        if(!checkPermission(path, 1)){
            fprintf(stderr, "Cannot write in log, lack of permission\n");
        } else{
            f = fopen(path, "a");
            if(f != NULL){
                fprintf(f, "%lld || %s || %s || Added Report #%d\n", (long long)timestamp, input.role, input.name, newReport.reportId);
                fclose(f);
            } else{
                fprintf(stderr, "Failed logged_district\n");
                exit(1);
            }
        }
    }

    snprintf(path, sizeof(path), "%s/reports.dat", input.districtId);
    chmod(path, 0664);
    if(strcmp(input.role, "manager") == 0){
        snprintf(path, sizeof(path), "%s/logged_district", input.districtId);
        chmod(path, 0644);
    }
}

void removeRep(){
    if(strcmp(input.role, "manager") != 0){
        fprintf(stderr, "Access denied!\n");
        exit(1);
    }

    char path[128];
    snprintf(path, sizeof(path), "%s/reports.dat", input.districtId);

    if(!checkPermission(path, 1)){
        fprintf(stderr, "Access denied!\n");
        exit(1);
    }

    int f = open(path, O_RDWR);
    if(f == -1){
        fprintf(stderr, "Failed to open file\n");
        exit(1);
    }

    report curr;
    int reportExists = 0;
    off_t readCursor = 0;
    off_t writeCursor = 0;

    while(read(f, &curr, sizeof(report)) == sizeof(report)){
        if(curr.reportId == input.reportId){
            reportExists = 1;
            writeCursor = lseek(f, 0, SEEK_CUR) - sizeof(report);
            readCursor = lseek(f, 0, SEEK_CUR);
            break;
        }
    }

    if(!reportExists){
        fprintf(stderr, "Report not found\n");
        exit(1);
    }

    while(1){
        lseek(f, readCursor, SEEK_SET);
        if(read(f, &curr, sizeof(report)) != sizeof(report)){
            break;
        }

        lseek(f, writeCursor, SEEK_SET);
        write(f, &curr, sizeof(report));

        readCursor += sizeof(report);
        writeCursor += sizeof(report);
    }

    if(ftruncate(f, writeCursor) != 0){
        fprintf(stderr, "Failed to truncate file\n");
    }

    close(f);

    snprintf(path, sizeof(path), "%s/logged_district", input.districtId);
    FILE* log = fopen(path, "a");
    if(log != NULL){
        fprintf(log, "%lld || %s || %s || Removed Report #%d\n", (long long)time(NULL), input.role, input.name, input.reportId);
        fclose(log);
    }

}

void listRep(){
    char path[128];
    snprintf(path, sizeof(path), "%s/reports.dat", input.districtId);
    
    if(!checkPermission(path, 0)){
        fprintf(stderr, "Access deined!\n");
        exit(1);
    }

    struct stat st;
    if(stat(path, &st) == -1){
        printf("No reports\n");
        return;
    }
    
    char permissionBits[10];
    permissionBitsSymbols(st.st_mode, permissionBits);
    printf("--- DISTRIC %s FILE INFO ---\n", input.districtId);
    printf("Permissions:        %s\n", permissionBits);
    printf("File Size:          %lld bytes\n", (long long)st.st_size);
    printf("Last Modified:      %s", ctime(&st.st_mtime));
    printf("----------------------------------------------------------\n");
    
    int f = open(path, O_RDONLY);
    if(f == -1){
        fprintf(stderr, "Failed to open file\n");
        return;
    }

    report currReport;
    int ctr = 0;
    printf("ID | Inspector          | Category   | Sev | Coordinates\n");
    printf("----------------------------------------------------------\n");
    
    while(read(f, &currReport, sizeof(report)) == sizeof(report)){
        ctr++;
        printf("%-2d | %-18s | %-10s | %-3d | X: %.2f, Y: %.2f\n", currReport.reportId, currReport.name, currReport.category, currReport.severity, currReport.xCords, currReport.yCords);
    }

    if(ctr == 0){
        printf("File is empty\n");
    }
    printf("----------------------------------------------------------\n");
    close(f);
}

void viewRep(){
    char path[128];
    snprintf(path, sizeof(path), "%s/reports.dat", input.districtId);

    if(!checkPermission(path, 0)){
        fprintf(stderr, "Acces denied!\n");
        exit(1);
    }

    int f = open(path, O_RDONLY);
    if(f == -1){
        fprintf(stderr, "Could not open reports.dat file\n");
        return;
    }

    report currReport;
    int reportExists = 0;

    while(read(f, &currReport, sizeof(report)) == sizeof(report)){
        if(currReport.reportId == input.reportId){
            reportExists = 1;
            printf("\n---REPORT NO #%d DETAILS---\n", currReport.reportId);
            printf("Inspector:      %s\n", currReport.name);
            printf("Category:       %s\n", currReport.category);
            printf("Severity:       %d\n", currReport.severity);
            printf("Coordonates:X:  %.2f, Y: %.2f\n", currReport.xCords, currReport.yCords);
            printf("Description:    %s\n", currReport.description);
            printf("Date Logged:    %s", ctime(&currReport.timestamp));
            printf("---------------------------\n");
            break;
        }
    }

    if(!reportExists){
        fprintf(stderr, "Could not find the report you are looking for\n");
    }
    close(f);
}

void updateRep(){
    if(strcmp(input.role, "manager") != 0){
        fprintf(stderr, "Access denied!\n");
        exit(1);
    }

    char path[128];
    snprintf(path, sizeof(path), "%s/district.cfg", input.districtId);

    if(!checkPermission(path, 1)){
        fprintf(stderr, "Access denied!\n");
        exit(1);
    }

    struct stat st;
    if(stat(path, &st) == -1){
        fprintf(stderr, "File doesn't exist\n");
        exit(1);
    }

    mode_t permission = st.st_mode & 0777;

    if(permission != 0640){
        fprintf(stderr, "File permissions have been tampered with!\nExpected:   640\nFound:      %03o\n", permission);
        exit(1);
    }

    int f = open(path, O_WRONLY | O_TRUNC);
    if(f == -1){
        fprintf(stderr, "Failed to open file\n");
        exit(1);
    }

    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%d\n", input.value);

    write(f, buffer, len);
    close(f);
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
    
    return 0;
}