#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

pthread_t thread1, thread2, thread3;


void *thread_printer(void* ptr);



int main(int argc, char** argv){

    printf("testing away\n");

    int thret1, thret2, thret3;
    
    char *message1 = "Thread1";
    char *message2 = "Thread2";
    char *message3 = "Thread3";

    thret1 = pthread_create(&thread1, NULL, thread_printer, (void*)message1);
    thret2 = pthread_create(&thread2, NULL, thread_printer, (void*)message2);
    thret3 = pthread_create(&thread3, NULL, thread_printer, (void*)message3);

    // in between
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    return 0;
}

void *thread_printer(void* ptr){

    char *messages;
    messages = (char*) ptr;
    printf("%s \n", messages);
    return NULL;
}








