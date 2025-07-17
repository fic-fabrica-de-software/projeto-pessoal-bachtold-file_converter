#include <stdio.h>
#include <stdlib.h>
#include "fileutils.h";

int main()
{
    printf("Do you want read[1], write[2] or  alter[3] the file csv? :)");
    int answ;
    scanf("%i", &answ);
    
    switch (answ){
        case 1:
            char *fileName = writeFileName(fileName);
            readFile(fileName);
            free(fileName);
            break;
        case 2: 
            char *fileName = writeFileName(fileName);
            writeFile(fileName);
            break;
        case 3:
            char *fileName = writeFileName(fileName);
            alterFile(fileName);
            break;
        default:
            printf("invalid command");
            break;
        
    }
    
    return 0;
}

void cleanBuffer(){
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

