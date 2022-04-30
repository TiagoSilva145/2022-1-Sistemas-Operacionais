//Autores:
//Tiago Silva 2023644
//Vitor Triches 2040042

/*
Criação de processos em UNIX, com impressão de valores de variável.

Compilar com gcc -Wall fork-print.c -o fork-print

Carlos Maziero, DINF/UFPR 2020
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main ()
{
   int retval, x ;                                          

   x = 0 ;
   
   retval = fork () ;                                    //cria o filho

   printf ("No processo %5d x vale %d\n", getpid(), x) ; //imprime x = 0 para ambos
   
   if ( retval < 0 )                                     //se < 0 entao retval nao funcionou
   {
      perror ("Erro") ;
      exit (1) ;
   }
   else
      if ( retval > 0 )                                  //se > 0 entao eh o pai
      {
         x = 0 ;
         wait (0) ;                                      //espera o filho terminar
      }
      else                                               //senao eh o filho
      {
         x++ ;                                           //incrementa x
         sleep (5) ;                                     //espera 5 segundos
      }

   printf ("No processo %5d x vale %d\n", getpid(), x) ; //imprime x novamente

   exit (0) ;
}