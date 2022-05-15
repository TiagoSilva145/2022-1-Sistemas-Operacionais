//Autores: Tiago Gonçalves da Silva - 2023644
//         Vitor Triches - 2040042

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include "queue.h"

#define NUM_PROD 3
#define NUM_CONS 2
#define VAGAS 5

sem_t vagas, itens; // semáforos para controle de vagas e itens
sem_t buffer;       // mutex para acessar ao buffer

int num_itens, num_vagas; // contadores de vagas e itens

typedef struct node
{
    struct node *prev;
    struct node *next;
    int info;
} Node;

Node *inicio_fila;
#define TAM 5

// corpo de produtor
void *prodBody(void *id)
{
    long tid = (long)id;
    // printf("P%02ld: Olá! \n",tid);

    while (1)
    {
        sleep(random() % 3);         // pausa entre 0 e n segundos (inteiro)
        sem_wait(&vagas);
        sem_wait(&buffer);

        Node *n;
        n = malloc(sizeof(Node));
        n->info = rand() % 100;
        queue_append((queue_t **)&inicio_fila, (queue_t *)n);

        sem_post(&buffer);
        sem_post(&itens);
        printf("P%d: Inseriu %d no buffer\n", tid, n->info);
    }
}

// corpo do consumidor
void *consBody(void *id)
{
    long tid = (long)id;
    //printf("C%02ld: Olá! \n", tid);
    while (1)
    {
        sem_wait(&itens);
        sem_wait(&buffer);
        int i = inicio_fila->info;
        queue_remove((queue_t**)&inicio_fila, (queue_t*)inicio_fila);
        sleep(random() % 3);
        printf("        C%d: Consumiu %d do buffer\n", tid, i);
    }
}

void cria_fila()
{
    for (int i = 0; i < TAM; i++)
    {
        Node *n;
        n = malloc(sizeof(Node));
        n->info = rand() % 100;
        queue_append((queue_t **)&inicio_fila, (queue_t *)n);
    }
}

// programa principal
int main(int argc, char *argv[])
{
    pthread_t produtor[NUM_PROD];
    pthread_t consumidor[NUM_CONS];
    long i;

    num_itens = 0;
    num_vagas = VAGAS;

    // inicia os semaforos
    sem_init(&buffer, 0, 1);
    sem_init(&vagas, 0, VAGAS);
    sem_init(&itens, 0, 0);

    // cria produtores
    for (i = 0; i < NUM_PROD; i++)
    {
        if (pthread_create(&produtor[i], NULL, prodBody, (void *)i))
        {
            perror("pthread_create");
            exit(1);
        }
    }

    // cria consumidores
    for (i = 0; i < NUM_CONS; i++)
    {
        if (pthread_create(&consumidor[i], NULL, consBody, (void *)i))
        {
            perror("pthread_create");
            exit(1);
        }
    }

    // main_encerra.aqui
    pthread_exit(NULL);
}