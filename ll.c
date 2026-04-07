#include<stdlib.h>
#include<stdio.h>

struct node{
    int block ;
    struct node* next ;
} ;

struct node* head = NULL ;

void freeBlock(int block_no){
   struct node* newnode = (struct node*) malloc(sizeof(struct node)) ;
   newnode->block = block_no ;
   newnode->next = head ;
   head = newnode ;
}

void allocate_block(){
    if(!head) {
        printf("no block free\n") ;
        return ;
    }
    struct node* temp = head ;
    printf("we got a free block: %d\n" , temp->block  ) ;
    head = head->next ;
    free(temp) ;
}

void display(){
    struct node* temp = head ;
    printf("Free blocks: ") ;
    while(temp){
        printf("%d ", temp->block);
        temp = temp->next;
    }
    printf("\n") ;
}

int main() {
    int choice, block;

    while (1) {
        printf("\n1. Free block\n2. Allocate block\n3. Display\n4. Exit\n");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter block: ");
                scanf("%d", &block);
                freeBlock(block);
                break;

            case 2:
                allocate_block();
                break;

            case 3:
                display();
                break;

            case 4:
                return 0;
        }
    }
}
