#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#define SIZE 9
pthread_t tid[SIZE+1];
int count = 0;
pthread_mutex_t lock;
pthread_t temp = 0;

void printsth(){
    int err = 0;
    err = pthread_create(&temp, NULL, (void*)&printsth, NULL);
    if(err != 0){
        exit(0);
    }
    pthread_mutex_lock(&lock);
    if(count > SIZE){
        return;
    }
    tid[count] = temp; 
    printf("count: %d\n", count);    
    printf("%dth temp: %d\n", count, tid[count]);  
    count++; 
    pthread_mutex_unlock(&lock);
    
}

int main(int argc, char** argv){
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("error\n");
    } 
    printsth();
    int i = 0;
    while(i < SIZE+1){
        pthread_join(tid[i], NULL);
        i++;
    }

    pthread_mutex_destroy(&lock);
    return 0;
}