
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NB_BUS_X 5
#define NB_BUS_Y 4
#define NB_TRAJETS 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t tunnel_x_y, tunnel_y_x;
int buses_in_tunnel = 0;
int direction = 0;
int waiting_x_y = 0;
int waiting_y_x = 0;

void sleep_random()
{
    usleep((rand() % 500 + 1000) * 1000); // 1 to 1.5 seconds
}

void enter_tunnel(int from_x, int bus_id, int trajet)
{
    pthread_mutex_lock(&mutex);

    if (from_x)
    {
        waiting_x_y++;
        if (direction == 2 || (direction == 0 && waiting_y_x > 0))
        {
            pthread_mutex_unlock(&mutex);
            sem_wait(&tunnel_x_y);
            pthread_mutex_lock(&mutex);
        }
        waiting_x_y--;
        direction = 1;
    }
    else
    {
        waiting_y_x++;
        if (direction == 1 || (direction == 0 && waiting_x_y > 0))
        {
            pthread_mutex_unlock(&mutex);
            sem_wait(&tunnel_y_x);
            pthread_mutex_lock(&mutex);
        }
        waiting_y_x--;
        direction = 2;
    }

    buses_in_tunnel++;
    pthread_mutex_unlock(&mutex);

    printf("Bus %d de %s : %s -> %s (Trajet %d)\n", bus_id,
           (bus_id < NB_BUS_X) ? "X" : "Y", from_x ? "X" : "Y",
           from_x ? "Y" : "X", trajet);

    sleep_random();
}

void exit_tunnel(int from_x)
{
    pthread_mutex_lock(&mutex);
    buses_in_tunnel--;

    if (buses_in_tunnel == 0)
    {
        if (from_x && waiting_y_x > 0)
        {
            direction = 2;
            for (int i = 0; i < waiting_y_x; i++)
                sem_post(&tunnel_y_x);
        }
        else if (!from_x && waiting_x_y > 0)
        {
            direction = 1;
            for (int i = 0; i < waiting_x_y; i++)
                sem_post(&tunnel_x_y);
        }
        else
        {
            direction = 0;
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *bus_thread(void *arg)
{
    int bus_id = *(int *)arg;
    int from_x = bus_id < NB_BUS_X;

    for (int i = 1; i <= NB_TRAJETS; i++)
    {
        // Aller
        enter_tunnel(from_x, bus_id, i);
        exit_tunnel(from_x);

        // Retour
        enter_tunnel(!from_x, bus_id, i);
        exit_tunnel(!from_x);
    }

    free(arg);
    return NULL;
}

int main()
{
    srand(time(NULL));
    pthread_t threads[NB_BUS_X + NB_BUS_Y];

    sem_init(&tunnel_x_y, 0, 0);
    sem_init(&tunnel_y_x, 0, 0);

    // Create bus threads
    for (int i = 0; i < NB_BUS_X + NB_BUS_Y; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, bus_thread, id);
    }

    // Wait for all buses to finish
    for (int i = 0; i < NB_BUS_X + NB_BUS_Y; i++)
    {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&tunnel_x_y);
    sem_destroy(&tunnel_y_x);
    pthread_mutex_destroy(&mutex);

    return 0;
}
