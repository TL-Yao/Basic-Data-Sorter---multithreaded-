#include "sorter_thread.h"
int main(int argc, char** argv){
    char* a = (char*)malloc(10);
    char* b = (char*)malloc(10);
    char* c = (char*)malloc(10);
    a = "abc";
    c = "def";
    char** abc = (char**)malloc(3*sizeof(char*));
    abc[0] = a;
    abc[1] = c;
    int i;
    for(i = 0; i < 2; i++){
        printf("%s\n", *abc);
        abc++;
    }

    return 0;
}