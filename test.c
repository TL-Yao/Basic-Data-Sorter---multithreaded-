#include "sorter_thread.h"
#define SIZE 500
pthread_t* tid[SIZE];
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
    int i = 0;
    int err = 0;
    int dirnum = 0;
    char** dirpath = (char**)malloc(500*sizeof(char*));
    struct sort_para** paraarr = (struct sort_para**)malloc(500*sizeof(struct sort_para*));

    struct sort_para* para;
    para = (struct sort_para*) arg;
	char* colname = para -> colname; 
    char* path = para -> tmppath;
    //pthread_t* tid = para -> tid;
    pthread_t* temparr = (pthread_t*)calloc(100,sizeof(pthread_t));
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
         
        /*skip forward and back folder*/
        if(strcmp(dir_ptr->d_name, ".") == 0 ||
		   strcmp(dir_ptr->d_name, "..") == 0 ||
		   dir_ptr->d_name[0] == '.'){
            continue;
        }
        
        if(isDirectory(temppath)){
            dirpath[dirnum] = malloc(strlen(temppath)+1);
            dirpath[dirnum] = strcpy(dirpath[dirnum], temppath);
            paraarr[dirnum] = (struct sort_para*)malloc(strlen(colname) + strlen(temppath) + 1);
            paraarr[dirnum] -> colname = colname;
            paraarr[dirnum] -> tmppath = temppath;
            dirnum++;
        }
    }
    pthread_mutex_lock(&lock);
        count++;
        printf("%d the tid is %d,  %s\n", count, pthread_self(), path);                
    pthread_mutex_unlock(&lock);  
    for(i = 0; i < dirnum; i++){
        err = pthread_create(&temparr[i], NULL, (void *)&count_process, (void*)paraarr[i]);
        if(err != 0){
            printf("Failed to create new thread.\n");
        }    
    }


    for(i = 0; i < dirnum; i++){
        pthread_join(temparr[i], NULL);
    }

}

int main(int argc, char** argv){
   /* if(pthread_mutex_init(&lock, NULL) != 0){
        printf("error\n");
    } */
    struct sort_para* para = (struct sort_para*) malloc (sizeof(struct sort_para*));
    char* colname = argv[1];
    char* dirname = argv[2];
    pthread_t* tid = (pthread_t*) calloc(sizeof(pthread_t), SIZE);

    para -> colname = colname;
    para -> tmppath = dirname;
   // para -> tid = tid;

    pthread_t tmptid;
    int error = 0;
    error = pthread_create(&tmptid, NULL, (void*)&count_process, (void*)para);
    if(error != 0){
        printf("Failed to create thread.\n");
        pthread_exit(0);
    }
    pthread_join(tmptid, NULL);
    pthread_mutex_destroy(&lock);
    return 0;
}