//Autores:
//Tiago Silva 2023644
//Vitor Triches 2040042

/*
Criação de processos em UNIX, com execução de outro binário

Compilar com gcc -Wall fork-execve.c -o fork-execve

Carlos Maziero, DINF/UFPR 2020
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char *argv[], char *envp[])
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
      {
         execve ("/bin/date", argv, envp) ;           //substitui o filho pelo processo "/bin/date"
         perror ("Erro") ;                            //caso execve nao tenha funcionado da erro
      }

   printf ("Tchau de %5d!\n", getpid()) ;             //encerra o programa

   exit (0) ;
}