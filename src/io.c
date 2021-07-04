#include "io.h"

long getFileSize(const char* path)
{
    FILE* fp;
    long fSize;
    
    // open file and die if u cant
    fp = fopen(path, "r");
    if (!fp) perror(path), exit(1);

    // seek the end of the file to find its length, then back to the start
    fseek(fp, 0L, SEEK_END);    // TODO: look @ fseek docs
    fSize = ftell(fp);          // TODO: look @ ftell docs
    rewind(fp);

    // close the file
    fclose(fp);

    return fSize;
}

void readFile(const char* path, char* buffer)
{
    FILE* fp;
    long fSize = getFileSize(path);

    // open file, throw if it doesnt exist
    fp = fopen(path, "r");
    if (!fp) perror(path), exit(1);

    if (!buffer) fclose(fp), fputs("memory alloc fails", stderr), exit(1);

    // copy the file into the buffer 
    if (1!=fread(buffer, fSize, 1, fp)) // TODO: fread?
        fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);

    // close file now that it has been read into a buffer
    fclose(fp);
}
