#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define WRITER_TURNS 10
#define READER_TURNS 10
#define READERS_COUNT 5
#define WRITERS_COUNT 5

#define BUFFER_SIZE 5


pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

long GetRandomTime(int maximumMiliseconds) {
    return (maximumMiliseconds * 1000.0 * (rand() / (RAND_MAX+1.0)));
}

// Writer thread function
int Writer(void* data) {
    int i;
    int threadId = *(int*)data;
    int writeCount = 0;
    
    for (i = 0; i < WRITER_TURNS; i++) {
        int result = pthread_mutex_lock(&mutex);
        if (result != 0) {
            fprintf(stderr, "Error occured during locking the mutex.\n");
            exit (-1);
        } else {
		    printf("(W) Writer %d started writing...", threadId);
		    fflush(stdout);
		    usleep(GetRandomTime(800));

		    // Write

		    printf("(W) Writer %d finished writing\n", threadId);
		    fflush(stdout);

		    writeCount++;

		    // inform critic that writer wrote
		    pthread_cond_broadcast(&cond);
	            
		    // Release ownership of the mutex object.
	            result = pthread_mutex_unlock(&mutex);
	            if (result != 0) {
	                fprintf(stderr, "Error occured during unlocking the mutex.\n");
	                exit (-1);
	            }
		    // Think, think, think, think
		    usleep(GetRandomTime(1000));
        }
    }

	printf("(W) Writer %d has written %d pages in total.\n", threadId, writeCount);

    free(data);
    return 0;
}

int Critic(void* data){
	int criticCount = 0;

	while(1){
		pthread_cond_wait(&cond, &mutex);
		printf("Critic\n");
		criticCount++;
		if(criticCount == WRITER_TURNS * WRITERS_COUNT -1) break;
	}

	free(data);
	return 0;
}

// Reader thread function
int Reader(void* data) {
    int i;
    int threadId = *(int*)data;
    int readCount = 0;
    
    for (i = 0; i < READER_TURNS; i++) {
        int result = 0;//= pthread_mutex_lock(&mutex);
        if (result != 0) {
            fprintf(stderr, "Error occured during locking the mutex.\n");
            exit (-1);
        } else {
		    // Read
		    printf("(R) Reader %d started reading...", threadId);
		    fflush(stdout);
	        
		    // Read

		    usleep(GetRandomTime(200));
		    printf("(R) Reader %d finished reading\n", threadId);
		    fflush(stdout);

		    readCount++;           

	    	// Release ownership of the mutex object.
            // result = pthread_mutex_unlock(&mutex);
            // if (result != 0) {
            //     fprintf(stderr, "Error occured during unlocking the mutex.\n");
            //     exit (-1);
            // }
	    
            usleep(GetRandomTime(800));
        }
    }

    printf("(R) Reader %d has read %d pages in total.\n", threadId, readCount);

    // Clean up the resources
    free(data);
    return 0;
}

int main(int argc, char* argv[])
{
    srand(100005);
    
    pthread_t writerThreads[WRITERS_COUNT];
    pthread_t readerThreads[READERS_COUNT];

    int i,rc;

    pthread_t criticThread;
    // create critic thread
    rc = pthread_create(
        &criticThread,  // thread identifier
        NULL,           // thread attributes
        (void*) Critic, // thread function
        NULL);  // thread function argument

    if (rc != 0) 
    {
    	fprintf(stderr,"Couldn't create the writer threads");
        exit (-1);
    }

    // Create the Writer threads
    for(i =0; i < WRITERS_COUNT; i++){
    	usleep(GetRandomTime(1000));

		int* threadId=malloc(sizeof(int));
		*threadId=i;

    	rc = pthread_create(
            &writerThreads[i],  // thread identifier
            NULL,           // thread attributes
            (void*) Writer, // thread function
            (void*) threadId);  // thread function argument

	    if (rc != 0) 
	    {
	    	fprintf(stderr,"Couldn't create the writer threads");
	        exit (-1);
	    }
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

    // Wait for the Writers
    for (i = 0; i < WRITERS_COUNT; i++)
    	pthread_join(writerThreads[i],NULL);

   	pthread_join(criticThread, NULL);

    return (0);
}
