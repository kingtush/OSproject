//David DiPalermo
//Mark Tushemereirwe

#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

int readCount = 0;
int writeCount = 0;

sem_t readMutex;
sem_t writeMutex;
sem_t readTry;
sem_t resource;

//int producer_loc = 0;
//int consumer_loc = 0;

//char next_char;
//char buffer[15];

//char newChar;
//FILE *fp;

void *read(void *arg){  //producer
  //while(fscanf(fp, "%c", &newChar) != EOF){ //producer thread
    sem_wait(&readTry);
    sem_wait(&readMutex);
    readCount = readCount + 1;
    //buffer[producer_loc] = newChar; // put new char in buffer
    //producer_loc = (producer_loc + 1) % buffer_size;
    if(readCount == 1){
       sem_wait(&resource);
    }
    sem_post(&readMutex);
    sem_post(&readTry);
    //}

    //critical section

    sem_wait(&readMutex);
    readCount = readCount - 1;
    //buffer[producer_loc] = '*';
    if(readCount == 0){
        sem_post(&resource);
    }
    sem_post(&readMutex);
    //sem_post(&full);
}


void *write(void *arg){  //consumer
//while(buffer[consumer_loc] != '*'){ //wait till *
   //sleep(1); //so no racing condition
   //sem_wait(&full);
   sem_wait(&writeMutex);
   writeCount = writeCount + 1;
   
   if(writeCount == 1){
      sem_wait(&tryRead);
   }
   //next_char = buffer[consumer_loc];
   //consumer_loc = (consumer_loc + 1) % buffer_size;
   
   sem_post(&writeMutex);
   //sem_post(&empty);
   
   //critical secttion
   sem_wait(&resource);
   //do write
   sem_post(&resource);

   //exit
   sem_wait(&writeMutex);
   writeCount = writeCount - 1;
   if(writeCount == 0){
      sem_post(&readTry);
   }
   sem_post(&writeMutex);
   
   //printf("%c", next_char); //prints next character
   //fflush(stdout);
   }
}


main(int argc, char **argv){

//fp = fopen("mytest.dat", "r"); // open mydata.dat

pthread_t tid1[1]; // id for thread 1
pthread_t tid2[1]; // id for thread 2
pthread_attr_t attr[1]; // attribute pointer array

pthread_attr_init(&attr[0]); // schedule thread independantly
pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM); //end thread schedule

sem_init(&readMutex, 0, 1); //initalize semaphores
sem_init(&writeMutex, 0, 1);
sem_init(&readTry, 0, 1);
sem_init(&resource, 0 ,1);

pthread_create(&tid1[0], &attr[0], read, NULL); //create producer thread
pthread_create(&tid2[0], &attr[0], write, NULL); //create consumer thread

pthread_join(tid1[0], NULL); //wait for producer thread to finish
pthread_join(tid2[0], NULL); //wait for consumer thread to finish

pthread_exit(NULL); //terminate threads
sem_destroy(&mutex); //destroy semaphores
sem_destroy(&full); 
sem_destroy(&empty);

close(fp); //closes file
}
