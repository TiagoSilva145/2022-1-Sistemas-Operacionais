//Autores:
//Tiago Silva 2023644
//Vitor Triches 2040042
#include "queue.h"
#include <stdio.h>

// Conta o numero de elementos na fila
// Retorno: numero de elementos na fila

int queue_size (queue_t *queue)
{
    if(!queue)
        return 0;

    int i = 1;

    queue_t* aux;
    aux = queue->next;

    while(aux != queue)
    {
        aux = aux->next;
        i++;
    }
    return i;
}

//------------------------------------------------------------------------------
// Percorre a fila e imprime na tela seu conteúdo. A impressão de cada
// elemento é feita por uma função externa, definida pelo programa que
// usa a biblioteca. Essa função deve ter o seguinte protótipo:
//
// void print_elem (void *ptr) ; // ptr aponta para o elemento a imprimir

void queue_print (char *name, queue_t *queue, void print_elem (void*) )
{
    if(!queue)
        return;

    queue_t* aux;
    aux = queue;

    printf("%s:[", name);
    do
    {
        print_elem(aux);
        if(aux->next != queue)
            printf(" ");
        aux = aux->next;
    } while(aux != queue);
    printf("]\n");
        
}

//------------------------------------------------------------------------------
// Insere um elemento no final da fila.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - o elemento deve existir
// - o elemento nao deve estar em outra fila
// Retorno: 0 se sucesso, <0 se ocorreu algum erro

int queue_append (queue_t **queue, queue_t *elem)
{
    if(!queue)
    {
        //fprint(stderr, "Erro: fila nao existente\n");
        printf("Erro: fila nao existente\n");
        return -1;
    }

    if(!elem)
    {
        printf("Erro: elemento nao existente\n");
        return -1;
    }

    if(elem->next || elem->prev)
    {
        printf("Erro: tentou inserir elemento ja em outra fila\n");
        return -1;
    }

    if(!(*queue))
    {
        *queue = elem;
        elem->next = elem;
        elem->prev = elem;
        return 0;
    }
    queue_t* aux;

    aux = (*queue)->prev;

    aux->next = elem;
    elem->prev = aux;

    (*queue)->prev = elem;
    elem->next = *queue;

    return 0;
}

//------------------------------------------------------------------------------
// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - a fila nao deve estar vazia
// - o elemento deve existir
// - o elemento deve pertencer a fila indicada
// Retorno: 0 se sucesso, <0 se ocorreu algum erro

int queue_remove (queue_t **queue, queue_t *elem)
{
    if(!queue)
    {
        printf("Erro: fila nao existente\n");
        return -1;
    }

    if(!(*queue))
    {
        printf("Erro: fila vazia\n");
        return -1;
    }

    if(!elem)
    {
        printf("Erro: elemento nao existente\n");
        return -1;
    }

    queue_t* aux;
    queue_t* aux2;

    aux = *queue;

    if(aux == aux->next && aux == elem)
    {
        aux->next = aux->prev = NULL;
        *queue = NULL;

        return 0;
    }

    int i = 0;
    do
    {
        if(aux == elem)
        {
            aux2 = elem->prev;
            aux = elem->next;

            aux2->next = aux;
            aux->prev = aux2;

            elem->next = elem->prev = NULL;
            if(!i)
                *queue = aux;

            return 0;
        }
        i = 1;
        aux = aux->next;
    } while(aux != *queue);

    printf("Erro: elemento nao esta na fila\n");
    return -1;
}
