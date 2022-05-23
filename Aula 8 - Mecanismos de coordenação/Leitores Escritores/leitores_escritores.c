//Vitor Triches 2040042
//Tiago Gonçalves

/*
Sistema leitores-escritores, solução simples com somente um semáforo,
com exclusão mútua mas sem paralelismo entre leitores.

Compilar com gcc -Wall rw1.c -o rw1 -lpthread

Carlos Maziero, DINF/UFPR 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "queue.h"

#define NUM_READERS 5
#define NUM_WRITERS 2

sem_t buffer, m_cont ;		// semáforo de exclusão no buffer
int shared ;		// área compartilhada
queue_t queue;
int leitores = 0;
int media = 0;
int tamanho_fila = 0;

typedef struct node
{
    struct node *prev;
    struct node *next;
    int info;
} Node;

Node* inicio_fila;
Node* fim_fila;

void imprime_fila()
{
    Node* aux = inicio_fila;
    printf("fila: ");
    do
    {
        printf("%2d ", aux->info);
        aux = aux->next;
    } while(aux != inicio_fila);    
    printf("media %2d ", media);
    printf("\n");
}

// espera um tempo aleatório entre 0 e n segundos (float)
void espera (int n)
{
  sleep (random() % n) ;	// pausa entre 0 e n segundos (inteiro)
  usleep (random() % 1000000) ;	// pausa entre 0 e 1 segundo (float)
}

// corpo das tarefas leitoras
void *readerBody (void *id)
{
  long tid = (long) id ;

  while (1)
  {
    // entra na seção crítica
    sem_wait (&m_cont) ;

    leitores++;
    if(leitores == 1){
        sem_wait(&buffer);
    }
    sem_post(&m_cont);

    // faz a leitura
    if(inicio_fila != NULL){
      imprime_fila();
    }
    printf ("R%02ld: read %d\n", tid, shared) ;
    espera (2) ;
    sem_wait(&m_cont);
    leitores--;
    if(leitores == 0){
      sem_post(&buffer);
    }
    // sai da seção crítica
    sem_post (&m_cont) ;
    printf ("R%02ld: out\n", tid) ;
    espera (2) ;
  }
}

// corpo das tarefas escritoras
void *writerBody (void *id)
{
  long tid = (long) id ;
  
  while (1)
  {
    // entra na seção crítica
    sem_wait (&buffer) ;

    // faz a escrita
    Node *n;
    n = malloc(sizeof(Node));
    shared = random () % 1000;
    n->info =  shared;
    tamanho_fila++;
    media = (media + n->info)/tamanho_fila;
    printf("\ntf: %d m: %d\n", tamanho_fila, media);
    queue_append((queue_t**)&inicio_fila, (queue_t*)n);
    //queue_remove((queue_t**)&inicio_fila, (queue_t*)inicio_fila);
    printf ("\t\t\tW%02ld: wrote %d\n", tid, shared) ;
    espera (2) ;

    // sai da seção crítica
    sem_post (&buffer) ;

    printf ("\t\t\tW%02ld: out\n", tid) ;
    espera (2) ;
  }
}

int main (int argc, char *argv[])
{
  pthread_t reader [NUM_READERS] ;
  pthread_t writer [NUM_WRITERS] ;
  long i ;

  shared  = 0 ;

  // inicia semaforos
  sem_init (&buffer, 0, 1) ;
  sem_init (&m_cont, 0, 1) ;

  // cria leitores
  for (i=0; i<NUM_READERS; i++)
    if (pthread_create (&reader[i], NULL, readerBody, (void *) i))
    {
       perror ("pthread_create") ;
       exit (1) ;
    }

  // cria escritores
  for (i=0; i<NUM_WRITERS; i++)
    if (pthread_create (&writer[i], NULL, writerBody, (void *) i))
    {
      perror ("pthread_create") ;
      exit (1) ;
    }

  // main encerra aqui
  pthread_exit (NULL) ;
}