/*
Criação de threads POSIX em UNIX.

Compilar com gcc -Wall thread-create.c -o thread-create -lpthread

Carlos Maziero, DINF/UFPR 2020
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 16

void *threadBody (void *id)
{
   long tid = (long) id ;

   printf ("t%02ld: Olá!\n", tid) ;                                        //logo quando a thread eh criada, da ola
   sleep (3) ;   
   printf ("t%02ld: Tchau!\n", tid) ;                                      //apos esperar 3 sec, da tchau
   
   pthread_exit (NULL) ;
}

int main (int argc, char *argv[])
{
   pthread_t thread [NUM_THREADS] ;                                        //cria um vetor com 15 threads neste caso
   long i, status ;
   
   for (i=0; i<NUM_THREADS; i++)                                           //para cada uma das 15 threads faz:
   {
      printf ("Main: criando thread %02ld\n", i) ;                         //imprime a criacao
      
      status = pthread_create (&thread[i], NULL, threadBody, (void *) i) ; //cria a thread, executando o threadBody no processo
      
      if (status)                                                          // caso status != 0, a thread nao foi criada, entao da erro
      {
         perror ("pthread_create") ;
         exit (1) ;
      }
   }
   pthread_exit (NULL) ;
}

/*
Em um sistema nao deterministico, nao ha como saber qual das threads é executada em qual ordem,
pois todas elas estão competindo pelo processador com os outros processos do sistema, assim é 
possivel que uma thread criada antes seja executada apos uma thread criada posteriormente.

*/