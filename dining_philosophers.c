#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

pthread_mutex_t chopstick[NUM_PHILOSOPHERS];

void think(int id) {
    printf("[Philosopher %d] Thinking...\n", id);
    sleep(1 + rand() % 2);
}

void eat(int id) {
    printf("[Philosopher %d] Eating...\n", id);
    sleep(1 + rand() % 2);
}

void* philosopher(void* arg) {
    int id = (int)(long)arg;

    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    while (1) {
        think(id);

        // Deadlock-free strategy: even philosophers pick up right first, then left;
        // odd philosophers pick up left first, then right.
        if (id % 2 == 0) {
            pthread_mutex_lock(&chopstick[right]);
            pthread_mutex_lock(&chopstick[left]);
        } else {
            pthread_mutex_lock(&chopstick[left]);
            pthread_mutex_lock(&chopstick[right]);
        }

        eat(id);

        pthread_mutex_unlock(&chopstick[left]);
        pthread_mutex_unlock(&chopstick[right]);
    }
    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&chopstick[i], NULL);
    }

    for (long i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, (void*)i);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&chopstick[i]);
    }

    return 0;
}
