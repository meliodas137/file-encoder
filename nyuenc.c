/*
*https://man7.org/linux/man-pages/man3/fopen.3.html
*https://man7.org/linux/man-pages/man3/fclose.3.html
*https://www.tutorialspoint.com/cprogramming/c_file_io.htm
*https://man7.org/linux/man-pages/man3/fwrite.3p.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B(x) S_to_binary_(#x)

static inline unsigned long long S_to_binary_(const char *s)
{
        unsigned long long i = 0;
        while (*s) {
                i <<= 1;
                i += *s++ - '0';
        }
        return i;
}

int main(int argc, char* argv[]){
    if(argc == 0) return 0;
    int ch = 0, prev = 0;
    unsigned char count = 0;
    int arg = 0;
    FILE* fd = NULL;
    while(++arg < argc) {
        char* fileName = argv[arg];
        if(fileName == NULL) continue;
        fd = fopen(fileName, "r");
        if(fd == NULL) fprintf(stderr, "File not found");
        else {
            ch = fgetc(fd);
            while(ch != EOF){ 
                if(prev != ch) {
                    if(prev != 0) {
                        fwrite(&prev, 1, 1, stdout);
                        fwrite(&count, 1, 1, stdout);
                    }
                    prev = ch;
                    count = 1;
                }
                else {
                    count++;
                }
                ch = fgetc(fd);
            }
            fclose(fd);
        }
    }
    fwrite(&prev, 1, 1, stdout);
    fwrite(&count, 1, 1, stdout);
    fflush(stdout);
    return 0;
}

