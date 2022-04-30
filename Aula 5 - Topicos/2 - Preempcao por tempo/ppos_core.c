//Autor: Tiago Silva - 2023644
//Autor: Vitor Rosa - 2040042

#include "ppos.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

#include <signal.h>
#include <sys/time.h>
#include <time.h>

int t_count = 0;
task_t ContextMain;
task_t disp;

task_t *inicio_fila;
task_t *atual;

#define PREEMPCAO 0

struct sigaction action;
struct itimerval timer;

int ticks = 0;
#define QUANTUM 20
clock_t tempo_inicial;


void tratador()
{
    if(++ticks <  QUANTUM);
	else
	{
		ticks = 0;
        task_yield();
		float tempo = (clock() - tempo_inicial) / 1000;
		printf("%f\n", tempo);
	}

    /*if(ticks == QUANTUM)
    {
        ticks = 0;
        task_yield();
		//float tempo = (clock() - tempo_inicial) / 1000;
		//printf("%f\n", tempo);
    }
    else
        ticks++;*/
}

// define a prioridade estática de uma tarefa (ou a tarefa atual)
void task_setprio (task_t *task, int prio)
{
    if(task)
    {
        task->prioridade = prio;
        task->prioridade_dinamica = prio;
    }
    else
    {
        atual->prioridade = prio;
        atual->prioridade_dinamica = prio;
    }
}

// retorna a prioridade estática de uma tarefa (ou a tarefa atual)
int task_getprio (task_t *task)
{
    if(task)
        return task->prioridade;
    else
        return atual->prioridade;
}

//Funcao de escalonamento
task_t* scheduler()
{
    task_t* menor = inicio_fila;
    task_t* itr = inicio_fila;

    do
    {
        if(itr->prioridade_dinamica < menor->prioridade_dinamica)
            menor = itr;
        itr = itr->next;
    } while (itr != inicio_fila);

    itr = inicio_fila;

    do
    {
        if(itr != menor)
            itr->prioridade_dinamica -= 1;
        itr = itr->next;
    } while (itr != inicio_fila);

    menor->prioridade_dinamica = menor->prioridade;
    return menor;
}

//Funcao do dispatcher
void dispatcher()
{
    while(queue_size((queue_t*)inicio_fila))
    {
        task_t *aux;
        if(PREEMPCAO)
            aux = scheduler();
        else
        {
            aux = inicio_fila;
            queue_remove((queue_t**)&inicio_fila, (queue_t*)aux);
            queue_append((queue_t**)&inicio_fila, (queue_t*)aux);
        }
        
        if(aux)
        {
            task_switch(aux);
        }
    }

    setcontext(&ContextMain.context);
}

void inicializa_tempo()
{
    tempo_inicial = clock();

    // registra a ação para o sinal de timer SIGALRM
	action.sa_handler = tratador;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	if (sigaction(SIGALRM, &action, 0) < 0)
	{
		perror("Erro em sigaction: ");
		exit(1);
	}

	// ajusta valores do temporizador
	timer.it_value.tv_usec = 1;	   // primeiro disparo, em micro-segundos
	timer.it_value.tv_sec = 0;	   // primeiro disparo, em segundos
	timer.it_interval.tv_usec = 1000; // disparos subsequentes, em micro-segundos
	timer.it_interval.tv_sec = 0;  // disparos subsequentes, em segundos

	// arma o temporizador ITIMER_REAL (vide man setitimer)
	if (setitimer(ITIMER_REAL, &timer, 0) < 0)
	{
		perror("Erro em setitimer: ");
		exit(1);
	}
}

// Inicializa o sistema operacional; deve ser chamada no inicio do main()
void ppos_init ()
{
    inicializa_tempo();

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
       ContextMain.prioridade = 0;
       ContextMain.prioridade_dinamica = 0;
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
       disp.prioridade = 0;
       disp.prioridade_dinamica = 0;
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