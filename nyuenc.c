/*
*https://man7.org/linux/man-pages/man3/fopen.3.html
*https://man7.org/linux/man-pages/man3/fclose.3.html
*https://www.tutorialspoint.com/cprogramming/c_file_io.htm
*https://man7.org/linux/man-pages/man3/fwrite.3p.html
*https://en.wikipedia.org/wiki/Pthreads
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_TASKS 262144
char** tasks;
char** completed;
int* size;
int* completed_size;
int num_threads;

void initMem(){
    int i = 0;
    tasks = malloc(MAX_TASKS*sizeof(char*));
    while(i < MAX_TASKS) tasks[i++] = NULL; 
    completed = malloc(MAX_TASKS*sizeof(char*));
    i = 0;
    while(i < MAX_TASKS) completed[i++] = NULL; 
    size = malloc(MAX_TASKS*sizeof(int));
    completed_size = malloc(MAX_TASKS*sizeof(int));
}

void createTasks(int argc, char* argv[]){
    int arg = 2, iTask = 0;
    while(++arg < argc) {
        char* fileName = argv[arg];
        if(fileName == NULL) continue;
        FILE* fd = fopen(fileName, "r");
        if(fd == NULL) {
            fprintf(stderr, "File not found");
            continue;
        }
        while(1) { 
            char *chunk = malloc(4096);
            int ret = fread(chunk, 1, 4096, fd);
            if(feof(fd) == true) {
                if(ret > 0) {
                    size[iTask] = ret;
                    tasks[iTask++] = chunk;
                }
                else free(chunk);
                break;
            }
            // printf("Chunk found %d : %d\n", ret, iTask);
            size[iTask] = ret;
            tasks[iTask++] = chunk;
        }
        fclose(fd);
    }
    return;
}

void doSeqEnc(int argc, char* argv[]) {
    int ch = -1, prev = -1;
    unsigned char count = 0;
    int arg = 0;
    while(++arg < argc) {
        char* fileName = argv[arg];
        if(fileName == NULL) continue;
        FILE* fd  = fopen(fileName, "r");
        if(fd == NULL) {
            fprintf(stderr, "File not found");
            continue;
        }
        ch = fgetc(fd);
        while(ch != EOF){ 
            if(prev != ch) {
                if(prev != -1) {
                    fwrite(&prev, 1, 1, stdout);
                    fwrite(&count, 1, 1, stdout);
                }
                prev = ch;
                count = 0;
            }
            count++;
            ch = fgetc(fd);
        }
        fclose(fd);
    }
    fwrite(&prev, 1, 1, stdout);
    fwrite(&count, 1, 1, stdout);
    fflush(stdout);
    return;
}

void *parallelTask(void *idx){
    char* index = (char *)idx;
    int start = (int)(*index);
    while(start < MAX_TASKS && tasks[start] != NULL) {
        char* st = tasks[start];
        unsigned char count = 0;
        int i = 0, j = 0, ch = -1, prev = -1;;
        char* comp = malloc(2*4096);
        while(i < size[start]) {
            ch = st[i++];
            if(prev != -1 && prev != ch){
                comp[j++] = prev;
                comp[j++] = count;
                count = 0;
            }
            prev = ch;
            count++;
        }
        comp[j++] = prev;
        comp[j++] = count;
        completed[start] = comp;
        completed_size[start] = j;
        // printf("parallelising the encoding %d %s\n", start, comp);
        start += num_threads;
    }
    pthread_exit(NULL);
}

void cleanMem(){
    int i = 0;
    while(tasks[i] != NULL) free(tasks[i++]);
    i = 0;
    while(completed[i] != NULL) free(completed[i++]);
    free(tasks);
    free(completed);
    free(size);
    free(completed_size);
    return;
}

void encode(int argc, char* argv[]){
    if(argc > 2 && strcmp(argv[1], "-j") == 0 ) {
        doParallelEnc(argc, argv);
    } else {
        doSeqEnc(argc, argv);
    }
}

int main(int argc, char* argv[]){
    if(argc == 0) return 0;
    encode(argc, argv);
    return 0;
}

