#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// FCFS scheduling
void fcfs() {
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    int arrival[n], burst[n], tat[n], pid[n], ct[n], wt[n];

    printf("Enter arrival time and burst time for each process (AT BT):\n");
    for (int i = 0; i < n; i++) {
        scanf("%d %d", &arrival[i], &burst[i]);
        pid[i] = i + 1;
    }

    // Sort by arrival time (simple bubble sort)
    for (int i = n - 1; i > 0; i--) {
        for (int j = 0; j < i; j++) {
            if (arrival[j] > arrival[j + 1]) {
                int tmp;

                tmp = arrival[j];
                arrival[j] = arrival[j + 1];
                arrival[j + 1] = tmp;

                tmp = burst[j];
                burst[j] = burst[j + 1];
                burst[j + 1] = tmp;

                tmp = pid[j];
                pid[j] = pid[j + 1];
                pid[j + 1] = tmp;
            }
        }
    }

    int time = arrival[0];
    float avgt = 0.0f, avgwt = 0.0f;

    printf("PID\tAT\tCT\tTAT\tWT\n");
    for (int i = 0; i < n; i++) {
        if (time < arrival[i]) {
            time = arrival[i]; // CPU idle until next process arrives
        }
        ct[i] = time + burst[i];
        time = ct[i];
        tat[i] = ct[i] - arrival[i];
        wt[i] = tat[i] - burst[i];

        avgt += tat[i];
        avgwt += wt[i];

        printf("%d\t%d\t%d\t%d\t%d\n", pid[i], arrival[i], ct[i], tat[i], wt[i]);
    }

    printf("Average TAT = %.2f\n", avgt / n);
    printf("Average WT  = %.2f\n", avgwt / n);
}

// Non-preemptive SJF scheduling
void sjf() {
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    int at[n], burst[n], tat[n], pid[n], ct[n], wt[n], done[n];
    float avgt = 0.0f, avgwt = 0.0f;

    printf("Enter arrival time and burst time for each process (AT BT):\n");
    for (int i = 0; i < n; i++) {
        scanf("%d %d", &at[i], &burst[i]);
        pid[i] = i + 1;
        done[i] = 0;
    }

    int completed = 0;
    int time = 0;

    printf("PID\tAT\tCT\tTAT\tWT\n");
    while (completed < n) {
        int idx = -1;
        int minBurst = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!done[i] && at[i] <= time && burst[i] < minBurst) {
                minBurst = burst[i];
                idx = i;
            }
        }

        if (idx == -1) {
            time++; // CPU idle
            continue;
        }

        ct[idx] = time + burst[idx];
        time = ct[idx];
        tat[idx] = ct[idx] - at[idx];
        wt[idx] = tat[idx] - burst[idx];

        avgt += tat[idx];
        avgwt += wt[idx];

        printf("%d\t%d\t%d\t%d\t%d\n", pid[idx], at[idx], ct[idx], tat[idx], wt[idx]);

        done[idx] = 1;
        completed++;
    }

    printf("Average TAT = %.2f\n", avgt / n);
    printf("Average WT  = %.2f\n", avgwt / n);
}

// Non-preemptive Priority scheduling (lower number = higher priority)
void priority() {
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    int at[n], burst[n], tat[n], pid[n], ct[n], wt[n], pr[n], done[n];
    float avgt = 0.0f, avgwt = 0.0f;

    printf("Enter arrival time, burst time and priority for each process (AT BT PR):\n");
    for (int i = 0; i < n; i++) {
        scanf("%d %d %d", &at[i], &burst[i], &pr[i]);
        pid[i] = i + 1;
        done[i] = 0;
    }

    int completed = 0;
    int time = 0;

    printf("PID\tAT\tPR\tCT\tTAT\tWT\n");
    while (completed < n) {
        int idx = -1;
        int minPr = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!done[i] && at[i] <= time && pr[i] < minPr) {
                minPr = pr[i];
                idx = i;
            }
        }

        if (idx == -1) {
            time++; // CPU idle
            continue;
        }

        ct[idx] = time + burst[idx];
        time = ct[idx];
        tat[idx] = ct[idx] - at[idx];
        wt[idx] = tat[idx] - burst[idx];

        avgt += tat[idx];
        avgwt += wt[idx];

        printf("%d\t%d\t%d\t%d\t%d\t%d\n", pid[idx], at[idx], pr[idx], ct[idx], tat[idx], wt[idx]);

        done[idx] = 1;
        completed++;
    }

    printf("Average TAT = %.2f\n", avgt / n);
    printf("Average WT  = %.2f\n", avgwt / n);
}

// Round Robin scheduling (queue-based, with arrival time)
void roundrobin() {
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    int at[n], burst[n], rem[n], tat[n], pid[n], ct[n], wt[n];
    float avgt = 0.0f, avgwt = 0.0f;

    printf("Enter arrival time and burst time for each process (AT BT):\n");
    for (int i = 0; i < n; i++) {
        scanf("%d %d", &at[i], &burst[i]);
        rem[i] = burst[i];
        pid[i] = i + 1;
    }

    int tq;
    printf("Enter time quantum: ");
    scanf("%d", &tq);

    // simple circular queue of process indices
    int q[n];
    int inQ[n];
    for (int i = 0; i < n; i++) inQ[i] = 0;
    int front = 0, rear = 0, count = 0;

    int time = 0;
    int completed = 0;

    // start from earliest arrival
    int firstAt = INT_MAX;
    for (int i = 0; i < n; i++) {
        if (at[i] < firstAt) firstAt = at[i];
    }
    time = firstAt;

    // enqueue all processes that have arrived at the start
    for (int i = 0; i < n; i++) {
        if (at[i] <= time && rem[i] > 0 && !inQ[i]) {
            q[rear] = i;
            rear = (rear + 1) % n;
            inQ[i] = 1;
            count++;
        }
    }

    printf("PID\tAT\tCT\tTAT\tWT\n");

    while (completed < n) {
        // if queue is empty, jump to next arriving process
        if (count == 0) {
            int nextAt = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (rem[i] > 0 && at[i] > time && at[i] < nextAt) {
                    nextAt = at[i];
                }
            }
            if (nextAt == INT_MAX) {
                break; // no more work
            }
            time = nextAt;
            for (int i = 0; i < n; i++) {
                if (at[i] <= time && rem[i] > 0 && !inQ[i]) {
                    q[rear] = i;
                    rear = (rear + 1) % n;
                    inQ[i] = 1;
                    count++;
                }
            }
            continue;
        }

        // take next process from queue
        int idx = q[front];
        front = (front + 1) % n;
        count--;
        inQ[idx] = 0;

        int exec = rem[idx] > tq ? tq : rem[idx];
        time += exec;
        rem[idx] -= exec;

        // add any newly arrived processes up to current time
        for (int i = 0; i < n; i++) {
            if (rem[i] > 0 && at[i] <= time && !inQ[i]) {
                q[rear] = i;
                rear = (rear + 1) % n;
                inQ[i] = 1;
                count++;
            }
        }

        if (rem[idx] == 0) {
            completed++;
            ct[idx] = time;
            tat[idx] = ct[idx] - at[idx];
            wt[idx] = tat[idx] - burst[idx];

            avgt += tat[idx];
            avgwt += wt[idx];

            printf("%d\t%d\t%d\t%d\t%d\n", pid[idx], at[idx], ct[idx], tat[idx], wt[idx]);
        } else {
            // still has time left, put it at end of queue again
            if (!inQ[idx]) {
                q[rear] = idx;
                rear = (rear + 1) % n;
                inQ[idx] = 1;
                count++;
            }
        }
    }

    printf("Average TAT = %.2f\n", avgt / n);
    printf("Average WT  = %.2f\n", avgwt / n);
}

int main() {
    printf("Select the scheduling algorithm you want to run:\n");
    printf("1. FCFS\n2. SJF (non-preemptive)\n3. Priority (non-preemptive)\n4. Round Robin\n5. Exit\n");

    int choice;
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            fcfs();
            break;
        case 2:
            sjf();
            break;
        case 3:
            priority();
            break;
        case 4:
            roundrobin();
            break;
        default:
            printf("Bye bye sweetheart!\n");
            break;
    }

    return 0;
}