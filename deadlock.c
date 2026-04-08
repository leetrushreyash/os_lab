#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define MAXP 10
#define MAXR 4
#define MAXREQ 10

// System Arrays
int TotalRes[MAXR] = {20, 20, 20, 20};
int Max[MAXP][MAXR], Alloc[MAXP][MAXR], Need[MAXP][MAXR];
int Req[MAXP][MAXR], Avail[MAXR];

// State flags
int pending[MAXP], completed[MAXP];
int activeProcs = MAXP;

// Thread Synchronization
pthread_cond_t resWait[MAXP], handlerWait;
pthread_mutex_t lock;

// Banker's Algorithm Safety Check
int isSafe() {
    int work[MAXR], finish[MAXP];
    for (int i = 0; i < MAXR; i++) work[i] = Avail[i];
    for (int i = 0; i < MAXP; i++) finish[i] = completed[i];

    while (1) {
        int found = 0;
        for (int i = 0; i < MAXP; i++) {
            if (!finish[i]) {
                int j;
                for (j = 0; j < MAXR; j++) 
                    if (Need[i][j] > work[j]) break;
                
                if (j == MAXR) { // If all needs can be met
                    for (j = 0; j < MAXR; j++) work[j] += Alloc[i][j];
                    finish[i] = 1;
                    found = 1;
                }
            }
        }
        if (!found) break; // Stop if we can't find any safe processes
    }
    
    for (int i = 0; i < MAXP; i++) 
        if (!finish[i]) return 0; // Unsafe!
    return 1; // Safe!
}

// OS tries to grant resources
int tryGrant(int p) {
    // 1. Can we even afford it right now?
    for (int j = 0; j < MAXR; j++) if (Req[p][j] > Avail[j]) return 0;

    // 2. Pretend to allocate
    for (int j = 0; j < MAXR; j++) {
        Avail[j] -= Req[p][j];
        Alloc[p][j] += Req[p][j];
        Need[p][j] -= Req[p][j];
    }
    
    // 3. Check Safety
    if (isSafe()) {
        for (int j = 0; j < MAXR; j++) Req[p][j] = 0;
        pthread_cond_signal(&resWait[p]); // Wake up the process
        return 1; 
    } else { 
        // 4. Rollback if unsafe
        for (int j = 0; j < MAXR; j++) {
            Avail[j] += Req[p][j];
            Alloc[p][j] -= Req[p][j];
            Need[p][j] += Req[p][j];
        }
        return 0;
    }
}

// OS Central Allocator Thread
void *handler(void *arg) {
    while (activeProcs > 0) {
        pthread_mutex_lock(&lock);
        
        // Loop over processes. If they are pending, try to grant.
        for (int i = 0; i < MAXP; i++) {
            if (pending[i]) {
                if (tryGrant(i)) pending[i] = 0; // Un-mark pending if granted
            }
        }
        
        pthread_cond_wait(&handlerWait, &lock); // Sleep until a process wakes me
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

// User Process Thread
void *process(void *arg) {
    int p = *(int *)arg;

    for (int iter = 0; iter < MAXREQ; iter++) {
        pthread_mutex_lock(&lock);
        // Ask for random amount within our Needs
        for (int j = 0; j < MAXR; j++) 
            if (Need[p][j] > 0) Req[p][j] = rand() % (Need[p][j] + 1);
        
        pending[p] = 1;
        pthread_cond_signal(&handlerWait); // Wake up OS
        pthread_cond_wait(&resWait[p], &lock); // Wait for approval
        pthread_mutex_unlock(&lock);

        sleep((rand() % 2) + 1); // Work

        // Randomly release some resources
        pthread_mutex_lock(&lock);
        if (rand() % 2 == 0) {
            for (int j = 0; j < MAXR; j++) {
                int r = rand() % (Alloc[p][j] + 1);
                Alloc[p][j] -= r;
                Avail[j] += r;
                Need[p][j] += r; // We gave it back, so we might need it later
            }
            pthread_cond_signal(&handlerWait); // Tell OS we released stuff
        }
        pthread_mutex_unlock(&lock);
        
        sleep((rand() % 2) + 1); // Work
    }

    // Finish entirely
    pthread_mutex_lock(&lock);
    for (int j = 0; j < MAXR; j++) {
        Avail[j] += Alloc[p][j]; // Give everything back
        Alloc[p][j] = 0;
    }
    completed[p] = 1;
    activeProcs--;
    printf("Process %d Finished successfully\n", p);
    pthread_cond_signal(&handlerWait); // Tell OS to check waiting threads or shut down
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t t_handler, t_proc[MAXP];
    int pids[MAXP];
    srand(time(NULL));

    // Initialize locks
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&handlerWait, NULL);
    
    // Setup arrays
    for (int j = 0; j < MAXR; j++) Avail[j] = TotalRes[j];

    for (int i = 0; i < MAXP; i++) {
        pthread_cond_init(&resWait[i], NULL);
        pids[i] = i;
        for (int j = 0; j < MAXR; j++) { // Give random initial Max Needs
            Max[i][j] = Need[i][j] = rand() % (TotalRes[j] / 2 + 1);
        }
    }

    // Start Threads
    pthread_create(&t_handler, NULL, handler, NULL);
    for (int i = 0; i < MAXP; i++) pthread_create(&t_proc[i], NULL, process, &pids[i]);

    // Wait for all processes to finish
    for (int i = 0; i < MAXP; i++) pthread_join(t_proc[i], NULL);
    
    // Shut down the handler
    pthread_mutex_lock(&lock);
    pthread_cond_signal(&handlerWait); 
    pthread_mutex_unlock(&lock);
    pthread_join(t_handler, NULL);

    printf("\nAll processes completed without deadlock!\n");
    return 0;
}
