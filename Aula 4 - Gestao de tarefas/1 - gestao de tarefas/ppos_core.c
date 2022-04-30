//Autor: Tiago Silva - 2023644

#include "ppos.h"
#include <stdlib.h>
#include <stdio.h>

int t_count = 0;
task_t ContextMain;
task_t* contextos[64*1024];
int atual;

// Inicializa o sistema operacional; deve ser chamada no inicio do main()
void ppos_init ()
{
    /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf (stdout, 0, _IONBF, 0) ;

    char *stack ; //Pilha de threads
    stack = malloc (64*1024) ;

   if(stack)
   {
       ContextMain.context.uc_stack.ss_sp = stack;
       ContextMain.context.uc_stack.ss_size = 64*1024;
       ContextMain.context.uc_stack.ss_flags = 0;
       ContextMain.context.uc_link = 0;
       ContextMain.id = t_count++;
       ContextMain.prev = NULL;
       ContextMain.next = NULL;
   }
   else
   {
        perror ("Erro na criação da pilha: ");
        exit (1);
   }
   getcontext(&ContextMain.context);
   contextos[0] = &ContextMain;
   atual = 0;
    
}

// Cria uma nova tarefa. Retorna um ID> 0 ou erro.
int task_create (task_t *task,			// descritor da nova tarefa
                 void (*start_func)(void *),	// funcao corpo da tarefa
                 void *arg)			// argumentos para a tarefa
{
    char *stack ; //Pilha de threads
    stack = malloc (64*1024) ;

   if(stack)
   {
        task->context.uc_stack.ss_sp = stack;
        task->context.uc_stack.ss_size = 64*1024;
        task->context.uc_stack.ss_flags = 0;
        task->context.uc_link = 0;
        task->id = t_count++;
   }
   else
   {
        perror ("Erro na criação da pilha: ");
        exit (1);
   }


   getcontext(&(task->context));
   makecontext(&(task->context), (void*)(*start_func), 1, arg);

   contextos[task->id] = task;
   return task->id;
}

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exit_code)
{
    atual = 0;
    setcontext(&(contextos[atual]->context));
}

// alterna a execução para a tarefa indicada
int task_switch (task_t *task)
{
    int prev = atual;
    atual = task->id;
    swapcontext(&(contextos[prev]->context), &(contextos[atual]->context));

    return 0;
}

// retorna o identificador da tarefa corrente (main deve ser 0)
int task_id ()
{
    return atual;
}
