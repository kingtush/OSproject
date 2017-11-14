//David DiPalermo, Mark Tushemereirwe, Tyler Matthews, and Gregory Wood

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
int numRead = 0;
int numWrite = 0;
int totalRead = 0;
int totalWrite = 0;
int shared = 0;

sem_t readMutex;
sem_t writeMutex;
sem_t readTry;
sem_t resource;

void *read(void *arg){ 
  while(totalRead < numRead){ //reader thread
    sleep(1);    

    sem_wait(&readTry);
    sem_wait(&readMutex);

    readCount = readCount + 1;
    totalRead = totalRead + 1;

    if(readCount == 1){
       sem_wait(&resource);
       printf("Reader is holding the resource\n");
    }

    sem_post(&readMutex);
    sem_post(&readTry);

    printf("The value in shared resource is %d\n", shared);
    //critical section

    sem_wait(&readMutex);
    readCount = readCount - 1;
    
    if(readCount == 0){
        sem_post(&resource);
        printf("Reader has released the resource\n");
    }
    sem_post(&readMutex);
  }
}


void *write(void *arg){  
while(totalWrite < numWrite){ //wait till all writers done
   sleep(2); //so no racing condition

   sem_wait(&writeMutex);
   writeCount = writeCount + 1;
   totalWrite = totalWrite + 1;
   
   if(writeCount == 1){
      sem_wait(&readTry); //lock reader out
      printf("A Writer is trying to access the resource\n");
   }
   
   sem_post(&writeMutex);
   
   //critical secttion
   
   sem_wait(&resource);
   
   shared = shared + 1;

   printf("Writing\n");
   
   sem_post(&resource);

   printf("Writer has released the resource\n");

   //exit
   sem_wait(&writeMutex);
   writeCount = writeCount - 1;

   if(writeCount == 0){
      sem_post(&readTry);
      printf("Writer is complete\n");
   }

   sem_post(&writeMutex);
   }
}


main(int argc, char **argv){

printf("Enter how many readers you want: ");
scanf("%d", &numRead);

printf("Enter how many writers you want: ");
scanf("%d", &numWrite);

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
sem_destroy(&readMutex); //destroy semaphores
sem_destroy(&writeMutex); 
sem_destroy(&readTry);
sem_destroy(&resource);

}
