#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define WRITER_TURNS 10
#define READER_TURNS 10
#define READERS_COUNT 5

// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t *readersSem, *writerSem;
int readersCount = 0;

void post_semaphore(sem_t* semaphore){
	if (sem_post(semaphore) != 0) {
        fprintf(stderr, "Error occured during unlocking semaphore.\n");
        exit (-1);
    } 
}

void wait_semaphore(sem_t* semaphore){
	if (sem_wait(semaphore) != 0) {
        fprintf(stderr, "Error occured during locking semaphore.\n");
        exit (-1);
    } 
}

long GetRandomTime(int maximumMiliseconds) {
    return (maximumMiliseconds * 1000.0 * (rand() / (RAND_MAX+1.0)));
}

// Writer thread function
int Writer(void* data) {
    int i;
    
    for (i = 0; i < WRITER_TURNS; i++) {
        wait_semaphore(writerSem);

	    // Write
	    printf("(W) Writer started writing...");
	    fflush(stdout);
	    usleep(GetRandomTime(800));
	    printf("finished\n");
            
        post_semaphore(writerSem);
	    // Think, think, think, think
	    usleep(GetRandomTime(1000)); 
    }

    return 0;
}

// Reader thread function
int Reader(void* data) {
    int i;
    int threadId = *(int*)data;
    int readCount = 0;
    
    for (i = 0; i < READER_TURNS; i++) {
        
        wait_semaphore(readersSem);

    	readersCount++;

    	if(readersCount == 1){
    		wait_semaphore(writerSem);
    	}

    	post_semaphore(readersSem);

	    // Read
	    printf("(R) Reader %d started reading...", threadId);
	    fflush(stdout);
            // Read, read, read
	    usleep(GetRandomTime(200));
	    printf("finished\n");

	    readCount++;            

        wait_semaphore(readersSem);
        readersCount--;
        if(readersCount == 0){
        	post_semaphore(writerSem);
        }
        post_semaphore(readersSem);
    
        usleep(GetRandomTime(800));
    }

    printf("(R) Reader %d has read %d pages in total.\n", threadId, readCount);

    // Clean up the resources
    free(data);

    return 0;
}

int main(int argc, char* argv[])
{
    srand(100005);
    
    pthread_t writerThread;
    pthread_t readerThreads[READERS_COUNT];

    // TODO: error checking
    writerSem = sem_open("writer_sem", O_CREAT, S_IRWXU, 1);
    readersSem = sem_open("readers_sem", O_CREAT, S_IRWXU, 1);

    int i,rc;

    // Create the Writer thread
    rc = pthread_create(
            &writerThread,  // thread identifier
            NULL,           // thread attributes
            (void*) Writer, // thread function
            (void*) NULL);  // thread function argument

    if (rc != 0) 
    {
    	fprintf(stderr,"Couldn't create the writer thread");
        exit (-1);
    }

    // Create the Reader threads
    for (i = 0; i < READERS_COUNT; i++) {
	// Reader initialization - takes random amount of time
	usleep(GetRandomTime(1000));
	int* threadId=malloc(sizeof(int));
	*threadId=i;
	rc = pthread_create(
                &readerThreads[i], // thread identifier
                NULL,              // thread attributes
                (void*) Reader,    // thread function
                (void*) threadId); // thread function argument - in this example, it should be cleaned up by the Reader threa

        if (rc != 0)
        {
    	    fprintf(stderr,"Couldn't create the reader threads");
            exit (-1);
        }
    }

    // At this point, the readers and writers should perform their operations

    // Wait for the Readers
    for (i = 0; i < READERS_COUNT; i++) 
        pthread_join(readerThreads[i],NULL);

    // Wait for the Writer
    pthread_join(writerThread,NULL);

    sem_close(writerSem);
    sem_close(readersSem);

    return (0);
}
