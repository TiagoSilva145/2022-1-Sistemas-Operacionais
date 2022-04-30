/*
Criação de threads POSIX em UNIX, com impressão de variáveis.

Compilar com gcc -Wall thread-print.c -o thread-print -lpthread

Carlos Maziero, DINF/UFPR 2020
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 16

int x = 0 ;                                              //x comeca em 0

void *threadBody (void *id)
{
   long tid = (long) id ;
   
   x++ ;                                                 //incrementa x
   printf ("t%02ld: Olá!   (x=%02d)\n", tid, x) ;        //imprime x logo apos incrementar
   sleep (3) ;                                           //espera 3 segundos
   x++ ;
   printf ("t%02ld: Tchau! (x=%02d)\n", tid, x) ;        //incrementa x e imprime novamente

   pthread_exit (NULL) ;
}

int main (int argc, char *argv[])
{
   pthread_t thread [NUM_THREADS] ;
   long i, status ;
   
   for (i=0; i<NUM_THREADS; i++)
   {
      printf ("Main: criando thread %02ld\n", i) ;                         
      
      status = pthread_create (&thread[i], NULL, threadBody, (void *) i) ; //cria a thread
      
      if (status)                                                          //caso != 0, deu errado
      {
         perror ("pthread_create") ;
         exit (1) ;
      }
   }
   pthread_exit (NULL) ;
}

/*
neste caso x evolui de maneira nao sequencial, pois o comando de incrementar x dentro da thread
e o comando de imprimir, embora sejam um apos o outro, podem nao serem executados um apos o outro, 
pelo fato de todas as threads estarem competindo pelo processador
*/