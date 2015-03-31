#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define WRITER_TURNS 10
#define READER_TURNS 10
#define READERS_COUNT 5
#define WRITERS_COUNT 5

#define OBJECTS_COUNT 5

int objects[OBJECTS_COUNT];
pthread_mutex_t mutexArray[OBJECTS_COUNT];

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
    	int objectId = rand() % OBJECTS_COUNT; /* randomly choose which object will be accessed */

    	printf("(W) Writer %d trying to write to object %d\n", threadId, objectId);
		fflush(stdout);

        int result = pthread_mutex_trylock(&mutexArray[objectId]);
        if (result == 0) {
		    usleep(GetRandomTime(800));

		    objects[objectId] = rand() % 666;

		    printf("(W) Writer %d wrote to object %d value = %d\n", threadId, objectId, objects[objectId]);
		    fflush(stdout);

		    writeCount++;

		    // Release ownership of the mutex object.
            result = pthread_mutex_unlock(&mutexArray[objectId]);
            if (result != 0) {
                fprintf(stderr, "Error occured during unlocking the mutex.\n");
                exit (-1);
            }

		    usleep(GetRandomTime(1000));
        }
    }

	printf("(W) Writer %d has written %d pages in total.\n", threadId, writeCount);

    free(data);
    return 0;
}

// Reader thread function
int Reader(void* data) {
    int i;
    int threadId = *(int*)data;
    int readCount = 0;
    
    for (i = 0; i < READER_TURNS; i++) {
    	int objectId = rand() % OBJECTS_COUNT; /* randomly choose which object will be accessed */

    	printf("(R) Reader %d trying to read from object %d\n", threadId, objectId);
        fflush(stdout);

        int result = pthread_mutex_trylock(&mutexArray[objectId]);
		if (result == 0) {
	        usleep(GetRandomTime(200));
		    printf("(R) Reader %d read from object %d value %d\n", threadId, objectId, objects[objectId]);
		    fflush(stdout);

		    readCount++;           

	    	// Release ownership of the mutex object.
            result = pthread_mutex_unlock(&mutexArray[objectId]);
            if (result != 0) {
                fprintf(stderr, "Error occured during unlocking the mutex.\n");
                exit (-1);
            }
	    
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

    int i,rc;

    // initialize objects
    for(i = 0; i < OBJECTS_COUNT; i++){
    	objects[i] = 0;
    	pthread_mutex_init(&mutexArray[i], NULL);
    	// mutexArray[i] = PTHREAD_MUTEX_INITIALIZER;
    }


    pthread_t writerThreads[WRITERS_COUNT];
    pthread_t readerThreads[READERS_COUNT];


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

    return (0);
}
