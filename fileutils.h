#ifndef FILEUTILS_H
#define FILEUTILS_H

void writeFile(char *fileName);
unsigned int strpspn(const char *x, const char *y);
char* remove_extension(const char *filename);
int get_file_type(const char *filename);

#define FILE_TYPE_TXT 0
#define FILE_TYPE_BMP 1
#define FILE_TYPE_CSV 2
#define FILE_TYPE_JSON 3
#define FILE_TYPE_UNSUPPORTED 4

#endif