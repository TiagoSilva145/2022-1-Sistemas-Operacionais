//Autores:
//Tiago Silva 2023644
//Vitor Triches 2040042

/*
Criação de processos em UNIX.

Compilar com gcc -Wall fork.c -o fork

Carlos Maziero, DINF/UFPR 2020
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main ()
{
   int retval ;
   
   printf ("Ola, sou o processo %5d\n", getpid()) ;

   retval = fork() ;                                  //cria o filho
   
   printf ("[retval: %5d] sou %5d, filho de %5d\n",   //imprime o pid dele e de seu pai
           retval, getpid(), getppid()) ;

   if ( retval < 0 )                                  //se < 0 entao retval nao funcionou
   {
      perror ("Erro") ;
      exit (1) ;
   }
   else
      if ( retval > 0 )                               //se > 0 entao eh o pai
         wait (0) ;                                   //espera o filho terminar

      else                                            //se = 0 entao eh o filho
         sleep (5) ;                                  //aguarda 5 segundos

   printf ("Tchau de %5d!\n", getpid()) ;             //imprime o seu pid

   exit (0) ;                                         //encerra o programa
}