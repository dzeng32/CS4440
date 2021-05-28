/*
 * WRITTEN BY: Jason Tejada
 * DESC:
 *      This program simulates an airport terminal where we have
 *      P passengers, B baggage handlers, S security screeners, and
 *      F flight attendants. Passengers go through all their tasks.
 *      Once all threads have finished, the plan takes off.
 *      Example usage: ./airline 100 4 3 2
 */
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <vector>

using namespace std;

sem_t bag_sem, security_sem, flight_sem;

// Mutex locks used for changing shared data and printing out data
pthread_mutex_t passengerPrint = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t baggageMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t screenMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t attendantMutex = PTHREAD_MUTEX_INITIALIZER;

// The queues we will keep track of
vector<int> baggageLine;
vector<int> securityLine;
vector<int> flightLine;


// Passengers arrives at airport, goes to baggage line.
void *passenger(void * param) {
    int num = (intptr_t) param;

    pthread_mutex_lock(&passengerPrint);
    cout << "Passenger#" << num << " enters the terminal...\n";
    baggageLine.push_back(num);
    cout << "Passenger#" << num << " is waiting at baggage processing for a handler...\n";
    pthread_mutex_unlock(&passengerPrint);
    usleep(100);

    sem_post(&bag_sem);
    pthread_exit(NULL);
}

// Passenger gets to baggage, after they get their luggage claimed they must get screened by security
void *baggageHandler(void * param) {
    // 'handle baggage' for passengers, remove them from the line and add them to the next one (security).
    sem_wait(&bag_sem); // Synchronization
    pthread_mutex_lock(&baggageMutex); // Protecting crtical resources
    while(baggageLine.size() != 0) {
        cout << "Passenger#" << baggageLine[baggageLine.size() - 1] << " baggage handled...\n";
        securityLine.push_back(baggageLine[baggageLine.size() - 1]);
        cout << "Passenger#" << baggageLine[baggageLine.size() - 1] << " waiting to be screened by a screener...\n";
        baggageLine.pop_back();
        usleep(1);
        sem_post(&security_sem);
    }
    pthread_mutex_unlock(&baggageMutex);
    pthread_exit(NULL);    
}

// Passegner gets to security, after they are screened they get seated by a flight attendant.
void *securityScreener(void * param) {
    // 'Screen' passengers in line, remove them from line. Add them to next line (flight).
    sem_wait(&security_sem); // Synchronization
    pthread_mutex_lock(&screenMutex); // Protecting critical resources
    while(securityLine.size() != 0) {
        cout << "Passenger#" << securityLine[securityLine.size() - 1] << " screened by security...\n"; 
        flightLine.push_back(securityLine[securityLine.size() - 1]);
        cout << "Passenger#" << securityLine[securityLine.size() - 1] << " is waiting to be board the plane by an attendant...\n";
        securityLine.pop_back();
        usleep(1);
        sem_post(&flight_sem);
    } 
    pthread_mutex_unlock(&screenMutex);

    pthread_exit(NULL);
}

// Passenger waits to be seated, after this they are done
void *flightAttendant(void * param) {
    // 'Seat' passengers in line, remove them from the line. 
    sem_wait(&flight_sem);
    pthread_mutex_lock(&attendantMutex); // Protecting critical resource
    while(flightLine.size() != 0) {
        cout << "Passenger#" << flightLine[flightLine.size() - 1] << " has been seated and relaxes...\n"; 
        flightLine.pop_back();
        usleep(1);
    }
    pthread_mutex_unlock(&attendantMutex);

    pthread_exit(NULL);    
}

int main(int argc, char *argv[]) {
    if(argc != 5) {
        cout << "INVALID ARGS: ./airline3 PASSENGERS BAGGERS SCREENERS ATTENDANTS\n"; 
    }
    cout << "-----------------------------------------OPENING TERMINAL-----------------------------------------" << endl;
    // Converting command line args to integers
    int p = atoi(argv[1]);
    int b = atoi(argv[2]);
    int s = atoi(argv[3]);
    int f = atoi(argv[4]);

    // Initializing threads
    pthread_t passengers[p];
    pthread_t bag_handlers[b];
    pthread_t security_screeners[s];
    pthread_t flight_attendants[f];

    // Initializing semaphores
    sem_init(&bag_sem, 0, 0);
    sem_init(&security_sem, 0, 0);
    sem_init(&flight_sem, 0, 0);

    // Creating threads...
    for(int i = 0; i < b; i++) {
        pthread_create(&bag_handlers[i], NULL, &baggageHandler, NULL); 
    }  

    for(int i = 0; i < s; i++) {
        pthread_create(&security_screeners[i], NULL, &securityScreener, NULL); 
    }

    for(int i = 0; i < s; i++) {
        pthread_create(&flight_attendants[i], NULL, &flightAttendant, NULL); 
    }

    for(int i = 0; i < p; i++) {
        pthread_create(&passengers[i], NULL, &passenger, (void*)(intptr_t)i); 
    }

    // Joining threads...
    for(int j = 0; j < b; j++) {
        pthread_join(bag_handlers[j], NULL); 
    }

    for(int j = 0; j < s; j++) {
        pthread_join(security_screeners[j], NULL); 
    }

    for(int j = 0; j < f; j++) {
        pthread_join(flight_attendants[j], NULL); 
    }

    for(int j = 0; j < p; j++) {
        pthread_join(passengers[j], NULL); 
    }

    // All threads are done, take off
    cout << "--------------------------------------------TAKE OFF---------------------------------------------" << endl;
}
