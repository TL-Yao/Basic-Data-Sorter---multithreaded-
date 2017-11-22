#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

pthread_t tid[2];
int count = 0;
pthread_mutex_t lock;

void printsth(){
    pthread_mutex_lock(&lock);
    count++;
    printf("%dth thread start\n", count);
    printf("%dth thread end\n", count);
    pthread_mutex_unlock(&lock);
    
}

int main(int argc, char** argv){
    int i = 0;
    int err = 0;
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("error\n");
    }
    while(i < 2){
        err = pthread_create(&tid[i], NULL, &printsth, NULL);
        if(err != 0){
            printf("error\n");
        }
        i++;
    }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_mutex_destroy(&lock);
    return 0;
}