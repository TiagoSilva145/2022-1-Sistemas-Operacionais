//Autores: Tiago Gonçalves da Silva - 2023644
//         Vitor Triches - 2040042

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#include "queue.h"

#define NUM_THREADS  2
#define TAM 10
#define ITERACOES 1000000

sem_t semaforo;

typedef struct node
{
    struct node *prev;
    struct node *next;
    int info;
} Node;

Node* inicio_fila;


#include <signal.h>
#include <sys/time.h>
struct sigaction action;
struct itimerval timer;
int total = 0;
int total_sum = 0;
int itr = 0;

void tratador()
{
    printf("\nTotal de insercoes: %d\n", total);
    total_sum += total;
    total = 0;

    if(itr++ == 10)
        printf("Total por sec: %.2f", (float)total_sum/10);
}

void imprime_fila()
{
    Node* aux = inicio_fila;
    printf("fila: ");
    do
    {
        printf("%2d ", aux->info);
        aux = aux->next;
    } while(aux != inicio_fila);    
    printf("\n");
}

void *threadBody ()
{
    while(1)
    //for(int i = 0; i < ITERACOES; i++)
    {
        sem_wait(&semaforo);
        queue_remove((queue_t**)&inicio_fila, (queue_t*)inicio_fila);
        Node *n;
        n = malloc(sizeof(Node));
        n->info = rand() % 100;
        queue_append((queue_t**)&inicio_fila, (queue_t*)n);
        //imprime_fila();
        sem_post(&semaforo);
        //free(a);
        total++;
    }
}

void cria_fila()
{
    for(int i = 0; i < TAM; i++)
    {
        Node *n;
        n = malloc(sizeof(Node));
        n->info = rand() % 100;
        queue_append((queue_t**)&inicio_fila, (queue_t*)n);
    }
}

int main (int argc, char *argv[])
{
    pthread_t thread [NUM_THREADS] ;
    pthread_attr_t attr ;
    long i, status ;

    // registra a ação para o sinal de timer SIGALRM
	action.sa_handler = tratador;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	if (sigaction(SIGALRM, &action, 0) < 0)
	{
		perror("Erro em sigaction: ");
		exit(1);
	}

	// ajusta valores do temporizador
	timer.it_value.tv_usec = 1;	   // primeiro disparo, em micro-segundos
	timer.it_value.tv_sec = 0;	   // primeiro disparo, em segundos
	timer.it_interval.tv_usec = 0; // disparos subsequentes, em micro-segundos
	timer.it_interval.tv_sec = 1;  // disparos subsequentes, em segundos

	// arma o temporizador ITIMER_REAL (vide man setitimer)
	if (setitimer(ITIMER_REAL, &timer, 0) < 0)
	{
		perror("Erro em setitimer: ");
		exit(1);
	}

    srand(time(NULL));

    sem_init(&semaforo, 0, 1);

    cria_fila();
    imprime_fila();

    pthread_attr_init (&attr) ;
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE) ;

    // create threads
    for(i=0; i<NUM_THREADS; i++)
    {
        status = pthread_create (&thread[i], &attr, threadBody, (void *) i) ;
        if (status)
        {
            perror ("pthread_create") ;
            exit (1) ;
        }
    }

    // wait all threads to finish
    for (i=0; i<NUM_THREADS; i++)
    {
        status = pthread_join (thread[i], NULL) ;
        if (status)
        {
            perror ("pthread_join") ;
            exit (1) ;
        }
    }


    pthread_attr_destroy (&attr) ;
    pthread_exit (NULL) ;
}
