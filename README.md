gcc -o converter.exe interface.c converter.c fileutils.c $(pkg-config --cflags --libs gtk+-3.0)
