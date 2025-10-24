#ifndef CONVERTER_H
#define CONVERTER_H

int TXTtoCSV(const char *in, const char *out);
int TXTtoTSV(const char *in, const char *out);
int BMPtoTXT(const char *in, const char *out);
int CSVtoJSON(const char *in, const char *out);
int JSONtoCSV(const char *in, const char *out);


#endif