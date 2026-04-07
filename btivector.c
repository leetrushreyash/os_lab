#include <stdio.h>

#define SIZE 10

int main() {
    int bit[SIZE] = {0}; // all free

    int choice, block;

    while (1) {
        printf("\n1. Allocate\n2. Free\n3. Display\n4. Exit\n");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter block to allocate: ");
                scanf("%d", &block);

                if (bit[block] == 0) {
                    bit[block] = 1;
                    printf("Block allocated\n");
                } else {
                    printf("Already allocated\n");
                }
                break;

            case 2:
                printf("Enter block to free: ");
                scanf("%d", &block);

                if (bit[block] == 1) {
                    bit[block] = 0;
                    printf("Block freed\n");
                } else {
                    printf("Already free\n");
                }
                break;

            case 3:
                printf("Bit Vector: ");
                for (int i = 0; i < SIZE; i++)
                    printf("%d ", bit[i]);
                printf("\n");
                break;

            case 4:
                return 0;
        }
    }
}