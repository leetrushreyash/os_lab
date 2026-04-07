#include <stdio.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h> // Required for sleep()

sem_t mutex, wrt;
int read_count = 0;

void* reader(void* arg) {
    long id = (long)arg; // Cast to long first to avoid warnings
    while(1) {
        // --- ENTRY SECTION ---
        sem_wait(&mutex);
        read_count++;
        if(read_count == 1) {
            sem_wait(&wrt); // First reader locks the writer out
        }
        sem_post(&mutex);

        // --- CRITICAL SECTION (Reading happens here) ---
        // Notice this is OUTSIDE the mutex. Multiple readers can be here!
        printf("Reader %ld is reading... (Active Readers: %d)\n", id, read_count);
        sleep(1); // Read takes 1 second

        // --- EXIT SECTION ---
        sem_wait(&mutex);
        read_count--;
        if(read_count == 0) {
            sem_post(&wrt); // Last reader frees the writer
        }
        sem_post(&mutex);

        // Wait a bit before coming back
        sleep(1); 
    }
    return NULL;
}

void* writer(void* arg) {
    long id = (long)arg;
    while(1) {
        // --- WRITER ENTRY ---
        sem_wait(&wrt); 

        // --- CRITICAL SECTION ---
        printf("Writer %ld is WRITING!\n", id);
        sleep(1); // Write takes 1 second

        // --- WRITER EXIT ---
        sem_post(&wrt);

        // Wait a bit before coming back
        sleep(2); 
    }
    return NULL;
}

int main() {
    pthread_t wr[2], rd[4];

    // Init semaphores
    sem_init(&wrt, 0, 1);
    sem_init(&mutex, 0, 1);

    // Create Readers
    for(long i = 0; i < 4; i++) {
        pthread_create(&rd[i], NULL, reader, (void*)i);
    }

    // Create Writers
    for(long i = 0; i < 2; i++) {
        pthread_create(&wr[i], NULL, writer, (void*)i);
    }

    // Join threads
    for(int i = 0; i < 4; i++) pthread_join(rd[i], NULL);
    for(int i = 0; i < 2; i++) pthread_join(wr[i], NULL);

    sem_destroy(&wrt);
    sem_destroy(&mutex);

    return 0;
}