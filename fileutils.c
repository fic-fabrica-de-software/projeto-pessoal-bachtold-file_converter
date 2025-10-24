#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileutils.h"
#include <ctype.h>

void writeFile(char *fileName){
    FILE *file = fopen(fileName, "w");
    
    if(file == NULL){
        printf("open file error");
        return;
    }
    
    char x[500];
    printf("Write: \n");
    fgets(x, sizeof(x), stdin);
    fprintf(file, "%s", x);

    fclose(file);
}

unsigned int strpspn(const char *x, const char *y){
    unsigned int a, b;

    for(a = 0; x[a] != '\0'; a++){
        for(b = 0; y[b] != '\0'; b++){
            if(x[a] == y[b]){
                return a;
            }
        }
    }
    return a;
}

char* remove_extension(const char *filename) {
    char *result = malloc(strlen(filename) + 1);
    if (result == NULL) {
        return NULL;
    }
    strcpy(result, filename);
    char *last_dot = strrchr(result, '.');
    char *last_slash = strrchr(result, '/');
    
    if (last_dot != NULL && (last_slash == NULL || last_dot > last_slash)) {
        *last_dot = '\0';
    }
    
    return result;
}

int get_file_type(const char *filename) {
    if (filename == NULL) {
        return FILE_TYPE_UNSUPPORTED;
    }
    
    const char *extension = strrchr(filename, '.');
    if (extension == NULL) {
        return FILE_TYPE_UNSUPPORTED;
    }
    
    extension++; 

    char lower_ext[16];
    int i;
    for (i = 0; extension[i] && i < 15; i++) {
        lower_ext[i] = tolower(extension[i]);
    }
    lower_ext[i] = '\0';
    
    if (strcmp(lower_ext, "txt") == 0) {
        return FILE_TYPE_TXT;
    } else if (strcmp(lower_ext, "bmp") == 0) {
        return FILE_TYPE_BMP;
    } else if (strcmp(lower_ext, "csv") == 0) {
        return FILE_TYPE_CSV;
    } else if (strcmp(lower_ext, "json") == 0) {
        return FILE_TYPE_JSON;
    }  
    
    return FILE_TYPE_UNSUPPORTED;
}