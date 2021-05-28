/* WRITTEN BY: Daniel Zeng
 * DESC:
 *      This program uses two threads to produce and consume data. Implements the fix for producer/consumer 
 *      problem. Ran by executing ./consumerProducer
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

using namespace std;

#define MAX_THREAD 2
#define MAX_BUFF_SIZE 5

pthread_mutex_t bufLock;
sem_t empty, full;

int buffer[MAX_BUFF_SIZE];
//int buffer[MAX_BUFF_SIZE]= {1,3,5};
//int buffer[MAX_BUFF_SIZE]= {1,3,5,9,10};

void* producer(void*){

    int in = 0;
    for(;;){
        sem_wait(&full);
        pthread_mutex_lock(&bufLock);
        int rand_num = rand() % 10 + 1; //Produce random number 1-10

        buffer[in] = rand_num;       //Add into buffer
        in = (in + 1) % MAX_BUFF_SIZE;

        pthread_mutex_unlock(&bufLock);
        sem_post(&empty);
        //cout << "Produced: " << rand_num << endl;
    }
}

void* consumer(void*){
    int consumed_num;
    int out=0;
    for(;;){
        sem_wait(&empty);
        pthread_mutex_lock(&bufLock);
        consumed_num = buffer[out]; //Remove from buffer and consume
        out = (out + 1) % MAX_BUFF_SIZE;
        
        pthread_mutex_unlock(&bufLock);
        sem_post(&full);
        cout << "Consumed: " << consumed_num << endl;
        sleep(1);
    }
}

int main(void){
    
    srand(time(NULL));

    pthread_t thread_id[MAX_THREAD];
    pthread_mutex_init(&bufLock, NULL);
    sem_init(&full, 0, MAX_BUFF_SIZE);
    sem_init(&empty, 0, 0);

    for(int i =0; i< MAX_THREAD; i++){
        if(i%2 == 0){
            pthread_create(&thread_id[i], NULL, &producer, NULL);
        }
        else{
            pthread_create(&thread_id[i], NULL, &consumer, NULL);
        }
    }

    for(int i =0; i< MAX_THREAD; i++)
        pthread_join(thread_id[i], NULL);

    cout << "test done" << endl;
    pthread_mutex_destroy(&bufLock);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}
