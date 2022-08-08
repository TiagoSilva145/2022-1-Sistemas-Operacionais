//Autores:
//Tiago Silva 2023644
//Vitor Triches 2040042

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "queue.h"

typedef struct node
{
    struct node* prev;
    struct node* next;
    int info;
} Node;

int lru(int tam_quadros, int* sequencia, int tam_sequencia)
{
    int i = 0;
    
    Node* memoria = NULL;
    for(i = 0; i < tam_quadros; i++)
    {
        Node* aux = (Node*)malloc(sizeof(Node));
        aux->next = aux->prev = NULL;
        aux->info = sequencia[i];
        queue_append((queue_t**)&memoria, (queue_t*)aux);
    }
    
    int faltas = tam_quadros;

    faltas = tam_quadros;

    for(i = i; i < tam_sequencia; i++)
    {
        int encontrou = 0;
        Node* aux = NULL;

        for(int j = 0; j < tam_quadros; j++)
        {
            Node* itr = memoria;
            do
            {
                if(itr->info == sequencia[i])
                {
                    encontrou = 1;
                    aux = itr;
                    break;
                }
                itr = itr->next;
            } while (itr != memoria);
        }

        if(encontrou)
        {
            
            queue_remove((queue_t**)&memoria, (queue_t*)aux);
            queue_append((queue_t**)&memoria, (queue_t*)aux);
            continue;
        }
        else
        {
            Node* aux2 = memoria;
            queue_remove((queue_t**)&memoria, (queue_t*)memoria);
            free(aux2);

            aux = (Node*)malloc(sizeof(Node));
            aux->next = aux->prev = NULL;
            aux->info = sequencia[i];
            queue_append((queue_t**)&memoria, (queue_t*)aux);
            faltas++;
        }
    }

    return faltas;
}

int opt(int tam_quadros, int* sequencia, int tam_sequencia)
{
    int i = 0;

    int faltas = 0;
    int memoria[tam_quadros];
    for(i = 0; i < tam_quadros; i++)
    {
        memoria[i] = sequencia[i];
    }

    faltas = tam_quadros;

    for(i; i < tam_sequencia; i++)
    {
        int encontrou = 0;
        for(int j = 0; j < tam_quadros; j++)
        {
            if(sequencia[i] == memoria[j])
            {
                encontrou = 1;
                break;
            }
        }

        if(encontrou)
            continue;
        else
        {
            int tirar = -1;
            int indice = -1;
            for(int j = 0; j < tam_quadros; j++)
            {
                int candidato = -1;
                for(int k = i; k < tam_sequencia; k++)
                {
                    if(memoria[j] == sequencia[k])
                    {
                        candidato = k;
                        break;
                    }
                }
                if(candidato == -1)
                {
                    indice = j;
                    break;
                }
                else if(candidato > tirar)
                {
                    tirar = candidato;
                    indice = j;
                }
            }

            memoria[indice] = sequencia[i];
            faltas++;
        }
    }

    return faltas;
}

int fifo(int tam_quadros, int* sequencia, int tam_sequencia)
{
    int i = 0;

    int faltas = 0;
    int memoria[tam_quadros];
    for(i = 0; i < tam_quadros; i++)
    {
        memoria[i] = sequencia[i];
    }
    faltas = tam_quadros;

    int primeiro = 0;
    for(i = i; i < tam_sequencia; i++)
    {
        int encontrou = 0;
        for(int j = 0; j < tam_quadros; j++)
        {
            if(sequencia[i] == memoria[j])
            {
                encontrou = 1;
                break;
            }
        }

        if(encontrou)
            continue;
        else
        {
            memoria[primeiro % tam_quadros] = sequencia[i];
            primeiro++;
            faltas++;
        }
    }

    return faltas;
}

int main(int argc,char *argv[])
{
    //tamanho
    int tam = *argv[1] - 48;

    //recebe o nome do arquivo e abre
    char arquivo[50] = "";
    strcpy(arquivo, argv[2]);
    FILE* arq;
    arq = fopen(arquivo, "r");

    //cria a sequencia
    char aux[3];
    int sequencia[100];
    int i = 0;
    while(fgets(aux, 3, arq))
    {
        sequencia[i++] = aux[0] - 48;
    }

    printf ("%5d quadros, %7d refs: FIFO: %5d PFs, LRU: %5d PFs, OPT: %5d PFs\n"
            , tam, i, fifo(tam, sequencia, i), lru(tam, sequencia, i), opt(tam, sequencia, i)) ;


}