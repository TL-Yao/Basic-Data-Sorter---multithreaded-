#include "sorter_thread.h"
#define SIZE 255
pthread_t tid[SIZE+1];
int count = 0;
pthread_mutex_t lock;
pthread_t temp = 0;

char* path_contact(const char* str1,const char* str2){ 
    char* result;  
    result=(char*)malloc(strlen(str1)+strlen(str2)+ 3);
    if(!result){
        printf("fail to allocate memory space\n");  
        exit(1);  
    }  

	strcpy(result,str1);
    strcat(result,"/");   
	strcat(result,str2);  
    return result;  
} 
 
int isDirectory(char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
 }

void count_process(void* arg){ //new

    int err = 0;
    int tid_index = 0;
    struct sort_para* para;
    para = (struct sort_para*) arg;
	char* colname = para -> colname; 
    char* path = para -> tmppath;
    DIR *dir;  
    dir = opendir(path); 
    
    struct dirent *dir_ptr;
    
    if(dir == NULL){
		printf("Wrong Path\n");
		//fflush(stdout); 
        exit(1);
    }
    while (dir_ptr = readdir(dir)){
        char* temppath;
        temppath = path_contact(path, dir_ptr->d_name);
        struct stat st;
        stat(temppath, &st);
         
        /*skip forward and back folder*/
        if(strcmp(dir_ptr->d_name, ".") == 0 ||
		   strcmp(dir_ptr->d_name, "..") == 0 ||
		   dir_ptr->d_name[0] == '.'){
            continue;
        }

        if(isDirectory(temppath)){
            para -> tmppath = temppath;
            err = pthread_create(&temp, NULL, (void *)&count_process, (void*)para);
            if(err != 0){
                printf("Failed to create new thread.\n");
            }
            pthread_mutex_lock(&lock);

            tid[count] = temp;
            count++;
            printf("%d the tid is %d\n", count, temp);
            printf("count: %d path: %s\n", count, temppath);
            
            pthread_mutex_unlock(&lock);
            
        }
    }
    pthread_join(temp, NULL);
}

int main(int argc, char** argv){
   /* if(pthread_mutex_init(&lock, NULL) != 0){
        printf("error\n");
    } */
    struct sort_para* para = (struct sort_para*) malloc (sizeof(struct sort_para*));
    char* colname = argv[1];
    char* dirname = argv[2];
    
    para -> colname = colname;
    para -> tmppath = dirname;

    //printf("%s\n", dirname);
    count_process((void*)para);

    int i = 0;
    while(tid[i]){
        pthread_join(tid[i], NULL);
        i++;
    }

    pthread_mutex_destroy(&lock);
    return 0;
}