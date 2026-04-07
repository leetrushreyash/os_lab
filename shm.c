#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(){
    // creating or getting a shared memory
    int shmid ; // defines the id of the shared memory 
    shmid = shmget(1234 , 1024 , 0666|IPC_CREAT);   // key is 1234 , size of shared memory is 1024 bytes with rw permission also create
    // now attach this shared memory to pointer 
    void* ptr = shmat(shmid , NULL , 0) ;
    char* data  = (char*) shmat(shmid , NULL , 0) ;
    sprintf(data , "hello from shreyash") ;
    printf("Data: %s\n", data);
    shmdt(data);   // detach data from here 
    shmctl(shmid , IPC_RMID , NULL) ;
}