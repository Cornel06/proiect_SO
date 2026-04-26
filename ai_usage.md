# proiect_SO

Here i will write my AI usage report for the Phase 1 of the cityManager project.

--Tool used:
    Google Gemini

--Where did AI help me:
    Besides the given task to use AI as a tool to help us implement the filter functions, I also asked Gemini to explain to me the usage of different libraries/functions, to make me understand system calls and directory/file management as well as help pointing out bugs or logic issues in my code. 

--How did I use AI for parseCondition() and matchCondition():
    Prompt: I am implementing a project that stores reports on different issues. Using the command:
    ./cityManager --role yourRole --user yourName --function functionInputs
    you can do one of these actions:
        -addReport
        -removeReport
        -listAllReports
        -viewCertainReport
        -filter
        
    The input struct looks like this:
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

    The reports struct like this:
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

    Now, the filter function filters and displays all reports matching the condition. It accepts one (or optionally more) condition(s). In case of more conditions, they are given as distinct arguments separated by spaces. The command prints all reports that satisfy all of them (conditions are implicitly joined by AND). A condition is a single string of the form:
        field:operator:value
    Supported fields: severity, category, inspector, timestamp. Supported operators: ==, !=, <, <=, >, >=.

    Now here is the way i read the filters parameters:

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

    What i need is:
    Generate a function called int parseCondition(const char* input, char* field, char* op, char* value); which splits a field:operator:value string into its three parts.
    After that generate the function int mathCondition(report* r, const cahr* field, const char* op, const char* value) which returns 1 if the record satisfies the condition, 0 otherwise.
    Please explain how everything actually works.

--AI responses:
    Besides giving me the actual code I used and commenting most lines, Gemini gave me the safer option, comparing sscanf with looping and using strtok and explained what each symbol in sscanf meant.

--Issues that AI helped me solve:

--Learning Experience:
