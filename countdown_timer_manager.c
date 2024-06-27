#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define DEFAULT_COUNTDOWN_TIME 60 // Default countdown time in seconds
#define SHM_KEY 1234

int countdown_time = DEFAULT_COUNTDOWN_TIME;
int running = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int *shared_time;

void *countdown_thread(void *arg) 
{
    while (running) 
    {
        pthread_mutex_lock(&mutex);
        if (*shared_time > 0) 
	{
            (*shared_time)--;
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

void *pipe_thread(void *arg) 
{
    int pipe_fd = *((int *)arg);
    int command;
    int new_time;

    while (running) 
    {
        if (read(pipe_fd, &command, sizeof(int)) > 0) 
	{
            pthread_mutex_lock(&mutex);

            switch (command) 
	    {
                case 0: // Set new countdown time
                    if (read(pipe_fd, &new_time, sizeof(int)) > 0 && new_time > 0) 
		    {
                        countdown_time = new_time;
                        *shared_time = new_time;
                    }
                    break;
                case 1: // Stop the timer
                    running = 0;
                    break;
                case 2: // Reset the timer
                    *shared_time = countdown_time;
                    break;
                default:
                    printf("Invalid command\n");
                    break;
            }

            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

void print_timer() 
{
    while (running) 
    {
        pthread_mutex_lock(&mutex);
        int minutes = *shared_time / 60;
        int seconds = *shared_time % 60;
        pthread_mutex_unlock(&mutex);

        printf("\r%02d:%02d", minutes, seconds);
        fflush(stdout);
        sleep(1);
    }
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <pipe_read_fd>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipe_fd = atoi(argv[1]);

    pthread_t countdown_tid, pipe_tid;
    int shmid;

    // Create shared memory segment
    shmid = shmget(SHM_KEY, sizeof(int), 0666 | IPC_CREAT);
    if (shmid == -1) 
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory
    shared_time = (int *)shmat(shmid, NULL, 0);
    if (shared_time == (void *)-1) 
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory
    *shared_time = countdown_time;

    // Start the countdown thread
    if (pthread_create(&countdown_tid, NULL, countdown_thread, NULL) != 0) 
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // Start the pipe thread
    if (pthread_create(&pipe_tid, NULL, pipe_thread, &pipe_fd) != 0) 
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // Print the timer
    print_timer();

    // Wait for threads to finish
    pthread_join(countdown_tid, NULL);
    pthread_join(pipe_tid, NULL);

    // Detach from shared memory
    if (shmdt(shared_time) == -1) 
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // Delete shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1) 
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}

