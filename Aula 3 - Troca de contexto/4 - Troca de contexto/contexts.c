//Autor
//Tiago Silva - 2023644

// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.4 -- Janeiro de 2022

// Demonstração das funções POSIX de troca de contexto (ucontext.h).

// operating system check
#if defined(_WIN32) || (!defined(__unix__) && !defined(__unix) && (!defined(__APPLE__) || !defined(__MACH__)))
#warning Este codigo foi planejado para ambientes UNIX (LInux, *BSD, MacOS). A compilacao e execucao em outros ambientes e responsabilidade do usuario.
#endif

#define _XOPEN_SOURCE 600	/* para compilar no MacOS */

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACKSIZE 64*1024	/* tamanho de pilha das threads */

ucontext_t ContextPing, ContextPong, ContextMain ;

/*****************************************************/

void BodyPing (void * arg)
{
   int i ;

   printf ("%s: inicio\n", (char *) arg) ;

   for (i=0; i<4; i++)
   {
      printf ("%s: %d\n", (char *) arg, i) ;
      swapcontext (&ContextPing, &ContextPong) ;
   }
   printf ("%s: fim\n", (char *) arg) ;

   swapcontext (&ContextPing, &ContextMain) ;
}

/*****************************************************/

void BodyPong (void * arg)
{
   int i ;

   printf ("%s: inicio\n", (char *) arg) ;

   for (i=0; i<4; i++)
   {
      printf ("%s: %d\n", (char *) arg, i) ;
      swapcontext (&ContextPong, &ContextPing) ;
   }
   printf ("%s: fim\n", (char *) arg) ;

   swapcontext (&ContextPong, &ContextMain) ;
}

/*****************************************************/

int main (int argc, char *argv[])
{
   char *stack ;

   printf ("main: inicio\n") ;

   getcontext (&ContextPing) ;                     //salva o contexto atual em ping

   stack = malloc (STACKSIZE) ;
   if (stack)
   {
      ContextPing.uc_stack.ss_sp = stack ;
      ContextPing.uc_stack.ss_size = STACKSIZE ;
      ContextPing.uc_stack.ss_flags = 0 ;
      ContextPing.uc_link = 0 ;
   }
   else
   {
      perror ("Erro na criação da pilha: ") ;
      exit (1) ;
   }

   makecontext (&ContextPing, (void*)(*BodyPing), 1, "    Ping") ;   //ajusta os valores de ping, sendo eles:
                                                                     //a variavel contexto ping, a funcao deste contexto, a quantidade de parametros de sua inicializacao, e o parametro em si

   getcontext (&ContextPong) ;                                       //salva o contexto atual em pong

   stack = malloc (STACKSIZE) ;
   if (stack)
   {
      ContextPong.uc_stack.ss_sp = stack ;
      ContextPong.uc_stack.ss_size = STACKSIZE ;
      ContextPong.uc_stack.ss_flags = 0 ;
      ContextPong.uc_link = 0 ;
   }
   else
   {
      perror ("Erro na criação da pilha: ") ;
      exit (1) ;
   }

   makecontext (&ContextPong, (void*)(*BodyPong), 1, "        Pong") ;  //ajusta os valores de pong, sendo eles:
                                                                        //a variavel contexto pong, a funcao deste contexto, a quantidade de parametros de sua inicializacao, e o parametro em si

   swapcontext (&ContextMain, &ContextPing) ;   //salva o contexto atual em main e restaura o ping
   swapcontext (&ContextMain, &ContextPong) ;   //salva o contexto atual em main e restaura o pong

   printf ("main: fim\n") ;

   exit (0) ;
}

/*
---1:
-getcontext(&a): salva o contexto atual em a
-setcontext(&a): restaura o contexto anterior que foi salvo em a
-swapcontext(&a,&b): salva o contexto em a e restaura o contexto anterior que foi salvo em b
-makecontext() exemplo do codigo:
makecontext (&ContextPing, (void*)(*BodyPing), 1, "    Ping") ;
neste caso 
&ContextPing -> o contexto a ser salvo
(void*)(*BodyPing) -> a funcao a ser salva no contexto
1 -> a quantidade de parametros a ser passada para esta funcao
"    Ping" -> o parametro em si que foi passado

---2:
uc_stack.ss_sp       -> ponteiro para a primeira posicao da pilha
uc_stack.ss_size     -> tamanho da pilha
uc_stack.ss_flags    -> 
uc_link              -> ponteiro para o contexto que sera assumido quando este retornar

*/
