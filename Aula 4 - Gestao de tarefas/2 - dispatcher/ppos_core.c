//Autor: Tiago Silva - 2023644
//Autor: Vitor Rosa - 2040042

#include "ppos.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

int t_count = 0;
task_t ContextMain;
task_t disp;

//task_t* contextos[64*1024];
task_t *inicio_fila;
task_t *atual;

//Funcao de escalonamento
task_t* scheduler()
{
    return inicio_fila;
}

//Funcao do dispatcher
void dispatcher()
{
    //int total = t_count-2;
    //int atual = -1;
    while(queue_size((queue_t*)inicio_fila))
    {
        task_t *aux = scheduler();
        
        if(aux)
        {
            queue_remove((queue_t**)&inicio_fila, (queue_t*)aux);
            queue_append((queue_t**)&inicio_fila, (queue_t*)aux);
            task_switch(aux);
        }
    }

    setcontext(&ContextMain.context);
}

// Inicializa o sistema operacional; deve ser chamada no inicio do main()
void ppos_init ()
{
    /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf (stdout, 0, _IONBF, 0) ;

    char *stack ; //Pilha de threads
    stack = malloc (64*1024) ;

    //Cria a tarefa da main
   if(stack)
   {
       //Variaveis do contexto
       ContextMain.context.uc_stack.ss_sp = stack;
       ContextMain.context.uc_stack.ss_size = 64*1024;
       ContextMain.context.uc_stack.ss_flags = 0;
       ContextMain.context.uc_link = 0;
       //Variaveis da struct
       ContextMain.id = t_count++;
       ContextMain.next = NULL;
       ContextMain.prev = NULL;
       ContextMain.preemptable = 1;
       ContextMain.status = 1;
   }
   else
   {
        perror ("Erro na criação da pilha: ");
        exit (1);
   }
   getcontext(&ContextMain.context);
   
   atual = &ContextMain;
   //contextos[0] = &ContextMain;
   //atual = &ContextMain;

   //Cria a tarefa do dispatcher
   if(stack)
   {
       //Variaveis do contexto
       disp.context.uc_stack.ss_sp = stack;
       disp.context.uc_stack.ss_size = 64*1024;
       disp.context.uc_stack.ss_flags = 0;
       disp.context.uc_link = 0;
       //Variaveis da struct
       disp.id = t_count++;
       disp.next = NULL;
       disp.prev = NULL;
       disp.preemptable = 1;
       disp.status = 1;
   }
   else
   {
        perror ("Erro na criação da pilha: ");
        exit (1);
   }
   getcontext(&(disp.context));
   makecontext(&(disp.context), (void*)(*dispatcher), 0);
    
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

   queue_append((queue_t**)&inicio_fila, (queue_t*)task);
   return task->id;
}

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exit_code)
{
    queue_remove((queue_t**)&inicio_fila, (queue_t*)atual);
    setcontext(&disp.context);
}

// alterna a execução para a tarefa indicada
int task_switch (task_t *task)
{
    task_t *prev = atual;
    atual = task;
    swapcontext(&prev->context, &atual->context);

    return 0;
}

// retorna o identificador da tarefa corrente (main deve ser 0)
int task_id ()
{
    return atual->id;
}

// libera o processador para a próxima tarefa, retornando à fila de tarefas
// prontas ("ready queue")
void task_yield ()
{
    task_switch(&disp);
}