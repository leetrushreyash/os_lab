#include <stdio.h>
#include <stdbool.h>

int main() {
    // Number of processes and resources
    int processes = 5; 
    int resources = 3; 

    // Allocation Matrix: how many resources are already given to each process
    int alloc[5][3] = { { 0, 1, 0 }, // P0
                        { 2, 0, 0 }, // P1
                        { 3, 0, 2 }, // P2
                        { 2, 1, 1 }, // P3
                        { 0, 0, 2 } }; // P4

    // Max Matrix: the maximum number of resources each process may need
    int max[5][3] = { { 7, 5, 3 }, // P0
                      { 3, 2, 2 }, // P1
                      { 9, 0, 2 }, // P2
                      { 2, 2, 2 }, // P3
                      { 4, 3, 3 } }; // P4

    // Available Resources: what is currently available in the system
    int avail[3] = { 3, 3, 2 };

    int finish[5] = { 0 }; 
    int safeSeq[5]; 
    int work[3]; 
    int need[5][3];

    // 1. Calculate NEED matrix (Need = Max - Allocation)
    for (int i = 0; i < processes; i++) {
        for (int j = 0; j < resources; j++)
            need[i][j] = max[i][j] - alloc[i][j];
    }

    // 2. Initialize Work array
    for (int i = 0; i < resources; i++) {
        work[i] = avail[i];
    }

    // 3. Find a Safe Sequence
    int count = 0;
    while (count < processes) {
        bool found = false;
        for (int p = 0; p < processes; p++) {
            // Check if process is not finished
            if (finish[p] == 0) {
                int j;
                // Check if its needs can be satisfied with available work
                for (j = 0; j < resources; j++) {
                    if (need[p][j] > work[j]) {
                        break;
                    }
                }

                // If all needs of process 'p' are satisfied
                if (j == resources) {
                    // Process finishes, frees up its allocated resources
                    for (int k = 0; k < resources; k++) {
                        work[k] += alloc[p][k];
                    }

                    safeSeq[count++] = p;
                    finish[p] = 1;
                    found = true;
                }
            }
        }

        // If we loop through all processes and couldn't find any that can finish
        if (found == false) {
            printf("System is not in a safe state. Deadlock detected/possible!\n");
            return 0;
        }
    }

    // 4. If all processes are finished, deadlock is avoided
    printf("System is in a SAFE state.\nSafe sequence is: ");
    for (int i = 0; i < processes; i++) {
        printf("P%d ", safeSeq[i]);
        if(i != processes - 1) printf("-> ");
    }
    printf("\n");

    return 0;
}