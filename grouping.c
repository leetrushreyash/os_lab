#include <stdio.h>
#include <stdlib.h>

#define ADDR_PER_BLOCK 4    // we set that single block store 

// now we create a block that stores multiple free block addresses

struct groupnode{
    int free_blocks[ADDR_PER_BLOCK -1] ;
    int num_free ;
    struct groupnode* next_group ;
};

struct groupnode* head = NULL ;

void free_block(int block_addr){
    if(!head || head->num_free == ADDR_PER_BLOCK-1){
        struct groupnode* newnode = (struct groupnode*) malloc(sizeof(struct groupnode)) ;
        newnode->num_free = 0 ;
        newnode->next_group = head ;
        head = newnode ;
    }
    head->free_blocks[head->num_free] = block_addr ;
    head->num_free++ ;
}

void display(){
    struct groupnode* temp = head ;
    printf("\n--- Grouping Free Space List ---\n");
    while(temp){
        printf("group block [ ");
        for(int i = 0 ; i < temp->num_free ; i++){
             printf("%d ", temp->free_blocks[i]);
        }
        printf(" ]-> ") ;
        temp = temp->next_group ;
    }
     printf("NULL\n");
}

void allocate(){
    struct groupnode* temp = head ;
    while(temp && temp->num_free == 0){
        temp = temp->next_group ;  
    }
    if(temp) printf("free block no is %d\n" , temp->free_blocks[--temp->num_free]) ;
    else printf("no free blocks available\n") ;
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
                free_block(block);
                break;

            case 2:
                allocate();
                break;

            case 3:
                display();
                break;

            case 4:
                return 0;
        }
    }
}
