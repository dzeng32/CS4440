#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

using namespace std;

sem_t fatherSem;
pthread_mutex_t motherMutex;
int totalNumChildren = 13;

void *mothersTasks(void* parameter) {
    int currentDay = 1;
    string *temp = static_cast<std::string *>(parameter);
    string days = *temp;
    int maxDays = stoi(days);

    while (currentDay <= maxDays) {
        
        //Lock mutex
        pthread_mutex_lock(&motherMutex);
        cout << "-----This is day #" << currentDay << " of a day in the life of Mother Hubbard.-----" << endl;
        int index = 1;
        while (index < totalNumChildren) {
            //All the mother's tasks
            cout << "Child #" << index << " is being woken up." << endl;
            usleep(100);
            cout << "Child #" << index << " is being fed breakfast." << endl;
            usleep(100);
            cout << "Child #" << index << " is being taken to school." << endl;
            usleep(100);
            cout << "Child #" << index << " is being fed dinner." << endl;
            usleep(100);
            //Signal dad to start the task for this child
            sem_post(&fatherSem);
            index++;
        }

        if (totalNumChildren == index) { 
            cout << "Mother is going to take a nap." << endl;
        }
        
        currentDay++;
    }

    pthread_exit(NULL);
}

void *fathersTasks(void* parameter) {
    int currentDay = 1;
    string *temp = static_cast<std::string *>(parameter);
    string days = *temp;
    int maxDays = stoi(days);

    while (currentDay <= maxDays) {
        int index = 1;
        while (index < totalNumChildren) {
            sem_wait(&fatherSem);
            //All the father's tasks
            cout << "Child #" << index << " is being given a bath." << endl;
            usleep(100);
            cout << "Child #" << index << " is being put to bed." << endl;
            usleep(100);
            index++;
        }
        if (totalNumChildren == index) {
            //Signal mom to start the tasks for the next day
            pthread_mutex_unlock(&motherMutex);
            cout << "Father is going to sleep and waking up Mother to take care of the children." << endl;
        }
        currentDay++;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    //Initialize the semaphore and the mutex
    sem_init(&fatherSem, 0, 0);
    pthread_mutex_init(&motherMutex, NULL);

    if (argc != 2) {
        cout << "Incorrect number of arguments give" << endl;
    }
    pthread_t mother, father;
    //Make argument into a string to pass into the mother and father function's parameter
    string temp = argv[1];

    //Make each thread (father and mother) run their respective tasks
    pthread_create(&mother, NULL, &mothersTasks, static_cast<void*>(&temp));
    pthread_create(&father, NULL, &fathersTasks, static_cast<void*>(&temp));

    //Join both threads
    pthread_join(father, NULL);
    pthread_join(mother, NULL);
    
    //Destroy the semaphore and the mutex
    sem_destroy(&fatherSem);
    pthread_mutex_destroy(&motherMutex);
}