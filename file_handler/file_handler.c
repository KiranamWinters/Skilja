#include "file_handler.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>

char* load_file_into_memory(const char *path){
        struct stat st;
        stat(path,&st);
        FILE* file_ptr = fopen(path,"r");
        if(!file_ptr){printf("Invalid File\n");return NULL;}
        int fd = fileno(file_ptr);
        char* buffer = (char*)mmap(NULL,st.st_size+1,PROT_READ,MAP_PRIVATE,fd,0);
        fclose(file_ptr);
        return buffer;
}
