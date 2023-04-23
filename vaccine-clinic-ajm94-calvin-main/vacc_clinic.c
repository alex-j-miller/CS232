/*
 * Alex Miller
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NUM_VIALS 30
#define SHOTS_PER_VIAL 6
#define NUM_CLIENTS (NUM_VIALS * SHOTS_PER_VIAL)
#define NUM_NURSES 10
#define NUM_STATIONS NUM_NURSES
#define NUM_REGISTRATIONS_SIMULTANEOUSLY 4

#define NUM_THREADS (NUM_CLIENTS + NUM_NURSES)

/* global variables */
pthread_t ts[NUM_THREADS];

int num_vials_left = NUM_VIALS;

long int stations[NUM_STATIONS];
long int desks[NUM_REGISTRATIONS_SIMULTANEOUSLY];
int full = 0;
int empty = NUM_STATIONS;
int desk_full = 0;
int desk_empty = NUM_REGISTRATIONS_SIMULTANEOUSLY;

int nurse_index = 0;
int consumer_index = 0;

int mutex = 1;
int nurse_ready = 0;
int id_offset = 100;

int ready[NUM_NURSES];

int client_id = 0;
int nurse_id = 0;

/* global functions */

int get_rand_in_range(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

char *time_str;
char *curr_time_s() {
    time_t t;
    time(&t);
    time_str = ctime(&t);
    // replace ending newline with end-of-string.
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}

// lower and upper are in seconds.
void walk(int lower, int upper) {
    usleep(get_rand_in_range(lower*1000000, upper*1000000));
}

// arg is the nurses station number.
void *nurse(void *arg) {
    //long int id = (long int)arg;
    while (mutex <= 0); // wait
    mutex--;

    long int id = nurse_id;
    nurse_id++;

    mutex++; // signal

    fprintf(stderr, "%s: nurse %ld started\n", curr_time_s(), id);
    ready[id] = 0;

    while (1) {
        // walk a random amount of time between 1 and 3 seconds to get a vial of vaccine
        fprintf(stderr, "%s: nurse %ld is walking to get a vial\n", curr_time_s(), id);
        walk(1, 3);

        if (num_vials_left == 0) {
            fprintf(stderr, "%s: nurse %ld is done\n", curr_time_s(), id);
            pthread_exit(NULL); // leave if there are no vials left
        }
        num_vials_left--; // take a vial

        // walk back (again between 1 and 3 seconds)
        fprintf(stderr, "%s: nurse %ld is walking back\n", curr_time_s(), id);
        walk(1, 3);
        

        for(int i = 0; i < 6; i++) { // repeat for all six vials
            
            // indicate to the queue of clients waiting for a station assignment that you are ready for the next client.
            while(empty <= 0); // wait 
            empty--;

            while(mutex <= 0); // wait
            mutex--;

            stations[nurse_index] = id;
            nurse_index = (nurse_index + 1) % NUM_STATIONS; 
            fprintf(stderr, "%s: nurse %ld claims a station\n", curr_time_s(), id);

            mutex++; // signal
            full++; // signal

            // wait for the client to indicate that they are ready to be vaccinated
            while(ready[id] != 1);

            // give the client a vaccination (which takes 5 seconds to administer)
            fprintf(stderr, "%s: nurse %ld is giving the client a vaccination\n", curr_time_s(), id);
            usleep(5000000);

            while(mutex <= 0); // wait
            mutex--;

            ready[id] = 0;

            mutex++; // signal
        }
    }
}

void *client(void *arg) {
    //long int id = (long int)arg;
    long int curr_station;

    while (mutex <= 0); // wait
    mutex--;

    long int id = client_id;
    client_id++;

    mutex++; // signal

    // walk a random amount between 3 and 10 seconds to the registration desk
    fprintf(stderr, "%s: client %ld has arrived and is walking to register\n", curr_time_s(), id);
    walk(3, 10);

    // wait for an opening at the registration desk.
    while(mutex <= 0); // wait
    mutex--;

    while(desk_full >= NUM_REGISTRATIONS_SIMULTANEOUSLY); // wait
    desk_full--;
    desk_empty++; // signal

    mutex++; // signal
    
    
    // take a random amount of time between 3 and 10 seconds to register.
    fprintf(stderr, "%s: client %ld is registering\n", curr_time_s(), id);
    walk(3, 10);

    // walk a random amount of time between 3 and 10 seconds to get to the station-assignment queue.
    fprintf(stderr, "%s: client %ld is walking to the station-assignment queue\n", curr_time_s(), id);
    walk(3, 10);

    // wait for station assignment
    while(full <= 0); // wait
    full--;

    while(mutex <= 0); // wait
    mutex--;

    curr_station = stations[consumer_index];
    consumer_index = (consumer_index + 1) % NUM_STATIONS;
    
    fprintf(stderr, "%s: client %ld was assigned nurse %ld\n", curr_time_s(), id, curr_station);

    mutex++; // signal
    empty++; // signal

    // walk a random amount of time between 1 and 2 seconds to get to the assigned nurse's station.
    walk(1, 2);

    // indicate to the nurse that the client is ready for the vaccination
    while (mutex <= 0); // wait
    mutex--;

    fprintf(stderr, "%s: client %ld is ready for the vaccination\n", curr_time_s(), id);
    ready[curr_station] = 1; 
    
    mutex++; // signal

    // wait for the nurse to complete the vaccination
    while(ready[curr_station] != 0);

    fprintf(stderr, "%s: client %ld leaves the clinic!\n", curr_time_s(), id);
    pthread_exit(NULL);
}

int main() {
    srand(time(0));

    for(int i = 0; i < NUM_NURSES; i++) {
        pthread_create(&ts[i], NULL, nurse, NULL);
    }

    for(int i = NUM_NURSES; i < NUM_CLIENTS + NUM_NURSES; i++) {
        pthread_create(&ts[i], NULL, client, NULL);
        walk(3, 10);
    }
    
    pthread_exit(0);
}
