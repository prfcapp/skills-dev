#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

pthread_t thread1, thread2, thread3;


sem_t sem1;

struct node_stack{
    struct node* head;
    int size_limit;
    int size;

};

struct node{
    struct node* next;
    int value;
};

void *thread_printer(void* ptr);
void *thread_producer(void* args);
void *thread_consumer(void* args);


struct node_stack* stack_loader(){
    
    struct node_stack* stack = malloc(sizeof(struct node_stack));
    
    stack->head = malloc(sizeof(struct node));
    stack->size = 20;
    stack->size_limit = 40;

    struct node* head;
    head = stack->head;
    //todo error check
    
    head->value = 20;

    struct node* curs1;
    curs1 = head;

    for(int i = 1; i<20; i++){
        curs1->next = malloc(sizeof(struct node));
        //todo error check
        curs1->next->value = 20-i;
        curs1 = curs1->next;
    }

    return stack;
}

void stack_printer(struct node_stack* stack){

    struct node* head = stack->head;
    printf("Limit is size %d\n", stack->size_limit);
    printf("Stack is size %d\n", stack->size);
    if(head==NULL){
        return;
    }


    while(1){
        if(head==NULL){
            return;
        }else{
            printf("id -> %d\n",head->value);
            head=head->next;
        }
    }
}

void remove_node(struct node_stack* stack){

    if(stack == NULL || stack->head == NULL){
        printf("error removing node...\n");
        return;
    }

    if(stack->size==1){
        printf("value %d removed from stack.\n", stack->head->value);
        free(stack->head);
        stack->head = NULL;
        stack->size -= 1;
        return;
    }

    //rebase
    struct node* curs = NULL;
    curs = stack->head;
    printf("value %d removed from stack.\n", stack->head->value);
    stack->head = stack->head->next;
    free(curs);
    stack->size -= 1;
    

    return;
}

void add_node(struct node_stack* stack, int value){
    if(stack==NULL){
        printf("error - null passed to stack_printer\n");
        return;
    }

    if(stack->size >= stack->size_limit){
        printf("error - stack size has hit limit. Disregarding add_node with value %d\n",value);
        return;
    }
    
    struct node* new_node = malloc(sizeof(struct node));
    new_node->value = value;
    new_node->next = stack->head;
    stack->size += 1;
    stack->head = new_node;
    return;
}

void *thread_consumer(void* args){

    struct node_stack* stack = (struct node_stack*)args;
    while(1){

        if(stack == NULL || stack->head == NULL){
            printf("error removing node...\n");
            return NULL;
        }

        sem_wait(&sem1);
        if(stack->size==1){
            printf("value %d removed from stack.\n", stack->head->value);
            free(stack->head);
            stack->head = NULL;
            stack->size -= 1;
            return NULL;
        }

        //rebase
        struct node* curs = NULL;
        curs = stack->head;
        printf("value %d removed from stack.\n", stack->head->value);
        stack->head = stack->head->next;
        free(curs);
        stack->size -= 1;
        sem_post(&sem1);

        if(stack->size <= 0){
            break;
        }
        if(stack->size >= 40){
            break;
        }
        stack_printer(stack);
        sleep(1);
    }

    return NULL;
}


void *thread_producer(void* args){
    
    struct node_stack* stack = (struct node_stack*)args;
    
    while(1){
        if(stack==NULL){
            printf("error - null passed to stack_printer\n");
            return NULL;
        }

        int value = rand() % 20;
        if(stack->size >= stack->size_limit){
            printf("error - stack size has hit limit. Disregarding add_node with value %d\n",value);
            return NULL;
        }


        struct node* new_node = malloc(sizeof(struct node));


        new_node->value = value;
        
        printf("Adding %d to stack\n",new_node->value);
        sem_wait(&sem1);
        
        new_node->next = stack->head;
        stack->size += 1;
        stack->head = new_node;

        sem_post(&sem1);

        if(stack->size>=40){
            break;
        }
        if(stack->size<=0){
            break;
        }
        stack_printer(stack);
        sleep(1);

    }

    return NULL;
}




int main(int argc, char** argv){

    printf("testing away\n");

    srand(time(NULL));

    int thret1, thret2, thret3;
    sem_init(&sem1,0, 1);

    char *message1 = "Thread1";
    char *message2 = "Thread2";
    char *message3 = "Thread3";

    //thret1 = pthread_create(&thread1, NULL, thread_printer, (void*)message1);
    //thret2 = pthread_create(&thread2, NULL, thread_printer, (void*)message2);
    thret3 = pthread_create(&thread3, NULL, thread_printer, (void*)message3);

    // in between
    //pthread_join(thread1, NULL);
    //pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    // stack init and print
    struct node_stack* stack = stack_loader(); 
    stack_printer(stack);

    int vals = 0;
    /*
    while(1){
        add_node(stack, vals);
        vals++;
        if(vals>20){
            break;
        }
    }
    */


    thret1 = pthread_create(&thread1, NULL, thread_consumer, (void*)stack);
    thret2 = pthread_create(&thread2, NULL, thread_consumer, (void*)stack);
    thret3 = pthread_create(&thread3, NULL, thread_producer, (void*)stack);
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    stack_printer(stack);

    sem_destroy(&sem1);

    return 0;
}

void *thread_printer(void* ptr){

    char *messages;
    messages = (char*) ptr;
    printf("%s \n", messages);
    return NULL;
}








