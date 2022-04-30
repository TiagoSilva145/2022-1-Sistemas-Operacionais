/*
Criação de threads POSIX em UNIX, com operação join().

Compilar com gcc -Wall thread-join.c -o thread-join -lpthread

Carlos Maziero, DINF/UFPR 2020
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 16

void *threadBody(void *id)
{
   long tid = (long) id ;

   printf ("t%02ld: Olá!\n", tid) ;
   sleep (3) ;   
   printf ("t%02ld: Tchau!\n", tid) ;
   
   pthread_exit (NULL) ;
}

int main (int argc, char *argv[])
{
   pthread_t thread [NUM_THREADS] ;                               //vetor para as 16 threads
   pthread_attr_t attr ;                                          //variavel para alterar atributos da thread
   long i, status ;
   
   pthread_attr_init (&attr) ;
   pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE) ; //faz com que as threads criadas com attr sejam joinable

   for(i=0; i<NUM_THREADS; i++)
   {
      printf ("Main: criando thread %02ld\n", i) ;
      
      status = pthread_create (&thread[i], &attr, threadBody, (void *) i) ;   //cria a thread, neste caso com attr fazendo com qeu elas seja joinable
      if (status)
      {
         perror ("pthread_create") ;
         exit (1) ;
      }
   }
   
   for (i=0; i<NUM_THREADS; i++)
   {
      printf ("Main: aguardando thread %02ld\n", i);
      status = pthread_join (thread[i], NULL) ;                   //join faz com que a main espere a thread terminar, ou que ja tenha terminado, para continuar sua execucao
      if (status)                                                 //caso status seja diferente de 0 entao o joit falhou
      {
         perror ("pthread_join") ;
         exit (1) ;
      }
   }
   
   pthread_attr_destroy (&attr) ;
   
   pthread_exit (NULL) ;
}

/*
attr eh uma variavel para alterar os atributos padrao no momento da criacao da thread

pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE) faz com que o attr permita que a 
thread seja joinable


pthread_join (thread[i], NULL) suspende quem chama, neste caso a funcao main, ate que a thread[i] 
seja finalizada, retornando 0 caso obtenha sucesso
*/