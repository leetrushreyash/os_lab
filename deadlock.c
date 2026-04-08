#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define MAXPROCESSES 10     // total number of process 10
#define MAXRESOURCETYPES 4    // four types of resource r0 , r1 , r2 , r3
#define MAXREQUESTS 10       // maximum number of request a process can do 

int TotalResources[MAXRESOURCETYPES] = {20, 20, 20, 20};   // initial resources 
int Max[MAXPROCESSES][MAXRESOURCETYPES];
int Allocation[MAXPROCESSES][MAXRESOURCETYPES];
int Need[MAXPROCESSES][MAXRESOURCETYPES];
int Request[MAXPROCESSES][MAXRESOURCETYPES];
int Available[MAXRESOURCETYPES];
int finish_arr[MAXPROCESSES];   
int completed[MAXPROCESSES];    // visited 
int activeProcesses;

pthread_cond_t resourceWait[MAXPROCESSES];   // if process i request me make the thread sleep using this 
pthread_cond_t handlerWait;  // central allocator wait
int requestProcessesQueue[MAXPROCESSES+1]; // process will go to this queue after requesting store pid
int tail;  // a pointer 
pthread_mutex_t requestLock;   // for mut-ex in shared resources 

void displayState(void) { 
    int i, j, pno; 
    
    printf("Proc\t       MAX              Allocation              Need\n\t"); 
    for (j=0; j<MAXRESOURCETYPES; j++) 
        printf("  R%d", j+1); 
    printf("    "); 
    
    for (j=0; j<MAXRESOURCETYPES; j++) 
        printf("  R%d", j+1); 
    printf("    "); 
    
    for (j=0; j<MAXRESOURCETYPES; j++) 
        printf("  R%d", j+1); 
    printf("\n"); 
    
    for (i=0; i<MAXPROCESSES; i++) { 
        printf("P%d\t", i+1); 
        
        for (j=0; j<MAXRESOURCETYPES; j++) 
            printf("%4d", Max[i][j]); 
        printf("    "); 
        
        for (j=0; j<MAXRESOURCETYPES; j++) 
            printf("%4d", Allocation[i][j]); 
        printf("    "); 
        
        for (j=0; j<MAXRESOURCETYPES; j++) 
            printf("%4d", Need[i][j]); 
        printf("\n"); 
    } 
    
    printf("\nAvailable: "); 
    for (j=0; j<MAXRESOURCETYPES; j++) 
        printf("%4d", Available[j]); 
    
    printf("\nRequests waiting : %d\n", tail); 
    for (i=0; i<tail; i++) { 
        pno = requestProcessesQueue[i]; 
        printf("Process #%d : ", pno+1); 
        for (j=0; j<MAXRESOURCETYPES; j++) 
            printf(" %d", Request[pno][j]); 
        printf("\n"); 
    } 
    printf("Completed :"); 
    for (i=0; i<MAXPROCESSES; i++) 
        if (completed[i] == 1) 
            printf(" %d", i+1); 
    printf("\n\n"); 
}

void requestResources(int pno) { 
    int j; 
    for (j=0; j<MAXRESOURCETYPES; j++)  
        if ((Max[pno][j] - Allocation[pno][j]) > 0) 
            Request[pno][j] = rand() % (Max[pno][j] - Allocation[pno][j] + 1); 
        else Request[pno][j] = 0; 
    
    printf("Request by P#%d : ", pno + 1); 
    for (j=0; j<MAXRESOURCETYPES; j++) 
        printf("%d ", Request[pno][j]); 
    printf("\n"); 
    
    requestProcessesQueue[tail++] = pno; 
    pthread_cond_signal(&handlerWait); 
    pthread_cond_wait(&resourceWait[pno], &requestLock); 
}

void releaseAllResources(int pno) { 
    int j; 
    printf("Process #%d releasing all resources\n", pno + 1); 
    for (j=0; j<MAXRESOURCETYPES; j++) { 
        Available[j] += Allocation[pno][j]; 
        Allocation[pno][j] = 0; 
        Max[pno][j] = 0; 
        Need[pno][j] = 0; 
    } 
    pthread_cond_signal(&handlerWait); 
}

void releaseSomeResources(int pno) { 
    int j, r; 
    printf("Process #%d releasing some resources\n", pno + 1); 
    for (j=0; j<MAXRESOURCETYPES; j++) 
        if (Allocation[pno][j] != 0) { 
            r = (rand() % (Allocation[pno][j]+1)); 
            Allocation[pno][j] -= r; 
            Available[j] += r; 
        } 
    pthread_cond_signal(&handlerWait);
}

int isSafe(void) { 
    int i, j; 
    int work[MAXRESOURCETYPES]; 
    int finish[MAXPROCESSES] = {0}; 
    int safeseq[MAXPROCESSES] = {0}; 
    int count = 0; 
    int flag; 
    
    printf("Safety algorithm\n"); 
    displayState(); 
    
    for (j=0; j<MAXRESOURCETYPES; j++)  
        work[j] = Available[j]; 
    
    for (i=0; i<MAXPROCESSES; i++)  
        for (j=0; j<MAXRESOURCETYPES; j++)  
            Need[i][j] = Max[i][j] - Allocation[i][j]; 
    
    for (i=0; i<MAXPROCESSES; i++) { 
        flag = 1; 
        for (j=0; j<MAXRESOURCETYPES; j++)  
            if (Max[i][j] != 0) { 
                flag = 0; 
                break; 
            } 
        if (flag == 1) { 
            safeseq[count++] = i; 
            finish[i] = 1; 
        } 
    } 
    
    while (1) { 
        for (i=0; i<MAXPROCESSES; i++) { 
            if (finish[i] == 1) continue; 
            for (j=0; j<MAXRESOURCETYPES; j++) 
                if (Need[i][j] > work[j]) break; 
            
            if (j >= MAXRESOURCETYPES) break; 
        } 
        
        if (i < MAXPROCESSES) { 
            finish[i] = 1; 
            safeseq[count++] = i; 
            for (j=0; j<MAXRESOURCETYPES; j++)  
                work[j] += Allocation[i][j]; 
        } 
        else { 
            if (count == MAXPROCESSES) { 
                printf("Safe sequence : "); 
                for (i=0; i<count; i++) 
                    if (completed[i] == 0) 
                        printf(" %d", safeseq[i]+1); 
                printf("\n"); 
                return 1; 
            } 
            else return 0;  
        } 
    } 
}

int processRequest(int pno) { 
    int j; 
    
    printf("process Request #%d\n", pno + 1); 
    for (j=0; j<MAXRESOURCETYPES; j++) 
        printf(" %d", Request[pno][j]); 
    printf("\n"); 
    
    printf("\nAvailable: "); 
    for (j=0; j<MAXRESOURCETYPES; j++) 
        printf("%4d", Available[j]); 
    printf("\n"); 
    
    for (j=0; j<MAXRESOURCETYPES; j++) { 
        if ((Allocation[pno][j] + Request[pno][j]) > Max[pno][j]) break; 
    } 
    
    if (j < MAXRESOURCETYPES) { 
        printf("Error: invalid request by process : %d\n", pno + 1); 
        return 0; 
    } 
    
    for (j=0; j<MAXRESOURCETYPES; j++) { 
        if (Request[pno][j] > Available[j]) 
            break; 
    } 
    
    if (j < MAXRESOURCETYPES) { 
        printf("Sufficient resources are not available for process : %d\n", pno + 1); 
        return 0; 
    } 
    
    for (j=0; j<MAXRESOURCETYPES; j++) { 
        Available[j] -= Request[pno][j]; 
        Allocation[pno][j] += Request[pno][j]; 
    } 
    
    if (isSafe()) { 
        printf("Request of process %d is granted\n", pno + 1); 
        for (j=0; j<MAXRESOURCETYPES; j++)  
            Request[pno][j] = 0; 
        pthread_cond_signal(&resourceWait[pno]); 
        return 1; 
    } 
    else { 
        printf("Request of process %d is not granted, UNSAFE\n", pno + 1); 
        for (j=0; j<MAXRESOURCETYPES; j++) { 
            Available[j] += Request[pno][j]; 
            Allocation[pno][j] -= Request[pno][j];  
        } 
        return 0; 
    } 
}

void *deadlockHandler(void *arg) { 
    int i, j, pno, ret; 
    
    while (activeProcesses > 0) { 
        pthread_mutex_lock(&requestLock); 
        
        if (tail > 0) { 
            for (i=0; i<tail; i++) { 
                pno = requestProcessesQueue[i]; 
                ret = processRequest(pno); 
                
                if (ret == 1) { 
                    for (j=i; j<tail-1; j++)  
                        requestProcessesQueue[j] = requestProcessesQueue[j+1]; 
                    tail--; 
                    i--; 
                } 
            } 
        } 
        
        pthread_cond_wait(&handlerWait, &requestLock); 
        pthread_mutex_unlock(&requestLock); 
    } 
    return NULL;
}

void *requestGenerator(void *arg) { 
    int myno = *(int *)arg; 
    int i, j; 
    struct timespec t, rem; 
    
    for (i=0; i<MAXREQUESTS; i++) { 
        printf("Process #%d iteration %d\n", myno+1, (i+1)); 
        pthread_mutex_lock(&requestLock); 
        requestResources(myno); 
        pthread_mutex_unlock(&requestLock); 
        
        t.tv_sec = (rand() % 2) + 1; 
        t.tv_nsec = 0; 
        nanosleep(&t, &rem); 
        
        if ((rand() % 10) < 5) { 
            pthread_mutex_lock(&requestLock); 
            releaseSomeResources(myno); 
            pthread_mutex_unlock(&requestLock); 
        } 
        
        t.tv_sec = (rand() % 2) + 1; 
        t.tv_nsec = 0; 
        nanosleep(&t, &rem); 
    } 
    
    pthread_mutex_lock(&requestLock); 
    printf("Process %d has completed its execution\n", myno+1); 
    releaseAllResources(myno); 
    for (j=0; j<MAXRESOURCETYPES; j++) { 
        Max[myno][j] = 0; 
        Allocation[myno][j] = 0; 
    } 
    activeProcesses--; 
    completed[myno] = 1; 
    pthread_mutex_unlock(&requestLock); 
    
    pthread_cond_signal(&handlerWait);
    
    return NULL; 
}

void initialize() { 
    int i, j; 
    for (i=0; i<MAXPROCESSES; i++)  pthread_cond_init(&resourceWait[i], NULL);   // intialize all the condition variable
        
    pthread_cond_init(&handlerWait, NULL); // initialize condition for handler
    pthread_mutex_init(&requestLock, NULL);  // initialize mutex for shared resource 
    
    srand(time(NULL)); 
    
    for (i=0; i<MAXPROCESSES; i++)  { 
        for (j=0; j<MAXRESOURCETYPES; j++) { 
            Max[i][j] = rand() % (TotalResources[j] / 2 + 1); // assign max requirement
            Allocation[i][j] = 0;  // initialize allocation
            Request[i][j] = 0;  // initialize request 
        } 
        completed[i] = 0;  // mark all process not completed 
    } 
    
    for (j=0; j<MAXRESOURCETYPES; j++)  
        Available[j] = TotalResources[j];    // currently available resoruces 
    
    tail = 0; // tail of the queue
    activeProcesses = MAXPROCESSES;  // total active process
}

int main() { 
    int i; 
    pthread_t ptids[MAXPROCESSES];    // 10 threads for process 
    pthread_t htid;    // one central process for handling 
    int tnos[MAXPROCESSES] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // id for each process
    int htno = 0;  // handler 
    
    initialize(); 
    
    pthread_create(&htid, NULL, deadlockHandler, &htno); // create central thread it is going to execute deadlock handling
    
    for (i=0; i<MAXPROCESSES; i++)  
        pthread_create(&ptids[i], NULL, requestGenerator, &tnos[i]);  // make thread associate id and 
    
    printf("All threads created\n"); 
    
    for (i=0; i<MAXPROCESSES; i++)  
        pthread_join(ptids[i], NULL); 
    
    pthread_mutex_lock(&requestLock);
    pthread_cond_signal(&handlerWait); // wakeup the central thread 
    pthread_mutex_unlock(&requestLock);
    pthread_join(htid, NULL); 
    
    displayState(); 
    return 0; 
}