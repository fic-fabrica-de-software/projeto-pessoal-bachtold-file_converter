#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fileutils.h"
#include "converter.h"

int TXTtoCSV(const char *in, const char *out) {
    FILE* filein = fopen(in, "r");
    FILE* fileout = fopen(out, "w");
    
    if(filein == NULL || fileout == NULL){
        perror("open file error");
        return 1;
    }

    char c;
    while ((c = fgetc(filein)) != EOF) {
        if(c == ' '){
            fputc(',', fileout);
        }else{
            fputc(c, fileout);
        }
    }

    fclose(filein);
    fclose(fileout);
    return 0;

}

int TXTtoTSV(const char *in, const char *out) {
    FILE* filein = fopen(in, "r");
    FILE* fileout = fopen(out, "w");
    
    if(filein == NULL || fileout == NULL){
        perror("open file error");
        return 1;
    }

    char c;
    while ((c = fgetc(filein)) != EOF) {
        if(c == ' '){
            fputc('\t', fileout);
        }else{
            fputc(c, fileout);
        }
    }

    fclose(filein);
    fclose(fileout);
    return 0;

}

int BMPtoTXT(const char *in, const char *out) {
    FILE* filein = fopen(in, "rb");
    FILE* fileout = fopen(out, "w");

    if(filein == NULL || fileout == NULL){
        perror("open file error");
        return 1;
    }

    unsigned char header[54];
    if (fread(header, 1, 54, filein) != 54) {
        fprintf(stderr, "Error: Not a valid BMP file (header too small)\n");
        fclose(filein);
        fclose(fileout);
        return 1;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "Error: Not a valid BMP file (invalid signature)\n");
        fclose(filein);
        fclose(fileout);
        return 1;
    }

    int width = header[18] | (header[19] << 8) | (header[20] << 16) | (header[21] << 24);
    int height = header[22] | (header[23] << 8) | (header[24] << 16) | (header[25] << 24);
    short bitDepth = header[28] | (header[29] << 8);
    
    int compression = header[30] | (header[31] << 8) | (header[32] << 16) | (header[33] << 24);
    int imageSize = header[34] | (header[35] << 8) | (header[36] << 16) | (header[37] << 24);
    int xPixelsPerM = header[38] | (header[39] << 8) | (header[40] << 16) | (header[41] << 24);
    int yPixelsPerM = header[42] | (header[43] << 8) | (header[44] << 16) | (header[45] << 24);
    int colorsUsed = header[46] | (header[47] << 8) | (header[48] << 16) | (header[49] << 24);
    int importantColors = header[50] | (header[51] << 8) | (header[52] << 16) | (header[53] << 24);

    fprintf(fileout, "BMP File Information:\n");
    fprintf(fileout, "=====================\n");
    fprintf(fileout, "Width: %d pixels\n", width);
    fprintf(fileout, "Height: %d pixels\n", height);
    fprintf(fileout, "Bit Depth: %d bits per pixel\n", bitDepth);
    fprintf(fileout, "Compression: %d (0 = none, 1 = RLE8, 2 = RLE4)\n", compression);
    fprintf(fileout, "Image Size: %d bytes\n", imageSize);
    fprintf(fileout, "Horizontal Resolution: %d pixels per meter\n", xPixelsPerM);
    fprintf(fileout, "Vertical Resolution: %d pixels per meter\n", yPixelsPerM);
    fprintf(fileout, "Colors Used: %d\n", colorsUsed);
    fprintf(fileout, "Important Colors: %d\n", importantColors);
    fprintf(fileout, "\n");

    fseek(filein, 0, SEEK_END);
    long fileSize = ftell(filein);
    rewind(filein);
    
    fprintf(fileout, "File Size: %ld bytes\n", fileSize);
    fprintf(fileout, "Header Size: 54 bytes\n");
    
    int rowSize = ((width * bitDepth + 31) / 32) * 4;
    int expectedDataSize = rowSize * abs(height);
    fprintf(fileout, "Expected Pixel Data Size: %d bytes\n", expectedDataSize);

    fclose(filein);
    fclose(fileout);
    
    printf("BMP file converted successfully to TXT: %s\n", out);
    return 0;
}

int CSVtoJSON(const char *in, const char *out) {
    FILE* filein = fopen(in, "r");
    FILE* fileout = fopen(out, "w");
    
    if(filein == NULL || fileout == NULL) return 1;

    char line[1024];
    char *headers[50];
    int header_count = 0;
    int first_line = 1;
    int record_count = 0;

    fprintf(fileout, "[\n");

    while (fgets(line, sizeof(line), filein)) {
        if (first_line) {
            char *token = strtok(line, ",\n");
            while (token != NULL && header_count < 50) {
                headers[header_count++] = strdup(token);
                token = strtok(NULL, ",\n");
            }
            first_line = 0;
            continue;
        }

        if (record_count > 0) fprintf(fileout, ",\n");
        fprintf(fileout, "  {");

        char *token = strtok(line, ",\n");
        for (int i = 0; i < header_count && token != NULL; i++) {
            if (i > 0) fprintf(fileout, ", ");
            fprintf(fileout, "\"%s\": \"%s\"", headers[i], token);
            token = strtok(NULL, ",\n");
        }
        fprintf(fileout, "}");
        record_count++;
    }

    fprintf(fileout, "\n]");

    for (int i = 0; i < header_count; i++) free(headers[i]);
    fclose(filein);
    fclose(fileout);
    return 0;
}

int JSONtoCSV(const char *in, const char *out) {
    FILE* filein = fopen(in, "r");
    FILE* fileout = fopen(out, "w");
    
    if(filein == NULL || fileout == NULL){
        perror("Error opening file");
        return 1;
    }

    char line[2048];
    char headers[100][50] = {0};
    int header_count = 0;
    int record_count = 0;
    int in_object = 0;

    rewind(filein);
    while (fgets(line, sizeof(line), filein)) {
        char *ptr = line;
        
        while (*ptr) {
            if (*ptr == '{') {
                in_object = 1;
                ptr++;
                continue;
            }
            
            if (*ptr == '}') {
                in_object = 0;
                ptr++;
                continue;
            }
            
            if (in_object) {
                while (*ptr && isspace((unsigned char)*ptr)) ptr++;
                
                if (*ptr == '"') {
                    ptr++; 
                    char key[50] = {0};
                    int key_index = 0;
                    
                    while (*ptr && *ptr != '"' && key_index < 49) {
                        key[key_index++] = *ptr++;
                    }
                    key[key_index] = '\0';
                    
                    if (*ptr == '"') ptr++; 
                    
                    while (*ptr && (*ptr == ':' || isspace((unsigned char)*ptr))) ptr++;

                    if (*ptr == '"') {
                        ptr++;
                        while (*ptr && *ptr != '"') ptr++;
                        if (*ptr == '"') ptr++; 
                    } else {
                        while (*ptr && *ptr != ',' && *ptr != '}' && !isspace((unsigned char)*ptr)) ptr++;
                    }

                    int found = 0;
                    for (int i = 0; i < header_count; i++) {
                        if (strcmp(headers[i], key) == 0) {
                            found = 1;
                            break;
                        }
                    }
                    
                    if (!found && strlen(key) > 0) {
                        strncpy(headers[header_count], key, sizeof(headers[0]) - 1);
                        headers[header_count][sizeof(headers[0]) - 1] = '\0';
                        header_count++;
                    }
                }

                while (*ptr && (*ptr == ',' || isspace((unsigned char)*ptr))) ptr++;
            } else {
                ptr++;
            }
        }
    }
    
    for (int i = 0; i < header_count; i++) {
        if (i > 0) fprintf(fileout, ",");
        if (strchr(headers[i], ',') != NULL || strchr(headers[i], '"') != NULL) {
            fprintf(fileout, "\"%s\"", headers[i]);
        } else {
            fprintf(fileout, "%s", headers[i]);
        }
    }
    fprintf(fileout, "\n");
    
    rewind(filein);
    in_object = 0;
    char values[100][100] = {0}; 
    
    while (fgets(line, sizeof(line), filein)) {
        char *ptr = line;
        
        while (*ptr) {
            if (*ptr == '{') {
                in_object = 1;
                if (record_count > 0) {
                    fprintf(fileout, "\n");
                }
                
                for (int i = 0; i < header_count; i++) {
                    values[i][0] = '\0';
                }
                
                ptr++;
                continue;
            }
            
            if (*ptr == '}') {
                in_object = 0;
                
                for (int i = 0; i < header_count; i++) {
                    if (i > 0) fprintf(fileout, ",");
                    if (strlen(values[i]) > 0) {
                        if (strchr(values[i], ',') != NULL || strchr(values[i], '"') != NULL) {
                            fprintf(fileout, "\"%s\"", values[i]);
                        } else {
                            fprintf(fileout, "%s", values[i]);
                        }
                    }
                }
                
                record_count++;
                ptr++;
                continue;
            }
            
            if (in_object) {
                while (*ptr && isspace((unsigned char)*ptr)) ptr++;
                
                if (*ptr == '"') {
                    ptr++; 
                    char key[50] = {0};
                    int key_index = 0;
                    
                    while (*ptr && *ptr != '"' && key_index < 49) {
                        key[key_index++] = *ptr++;
                    }
                    key[key_index] = '\0';
                    
                    if (*ptr == '"') ptr++; 

                    while (*ptr && (*ptr == ':' || isspace((unsigned char)*ptr))) ptr++;

                    char value[100] = {0};
                    int value_index = 0;
                    
                    if (*ptr == '"') {
                        ptr++; 
                        while (*ptr && *ptr != '"' && value_index < 99) {
                            value[value_index++] = *ptr++;
                        }
                        value[value_index] = '\0';
                        if (*ptr == '"') ptr++; 
                    } else {
                        while (*ptr && *ptr != ',' && *ptr != '}' && !isspace((unsigned char)*ptr) && value_index < 99) {
                            value[value_index++] = *ptr++;
                        }
                        value[value_index] = '\0';
                    }

                    for (int i = 0; i < header_count; i++) {
                        if (strcmp(headers[i], key) == 0) {
                            strncpy(values[i], value, sizeof(values[0]) - 1);
                            values[i][sizeof(values[0]) - 1] = '\0';
                            break;
                        }
                    }
                }

                while (*ptr && (*ptr == ',' || isspace((unsigned char)*ptr))) ptr++;
            } else {
                ptr++;
            }
        }
    }

    fclose(filein);
    fclose(fileout);
    
    if (record_count == 0) {
        printf("Warning: No JSON objects found in file\n");
        return 1;
    }
    
    return 0;
}