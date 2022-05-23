//Vitor Triches 2040042
//Tiago Gonçalves


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

#define NUM_FILOSOFOS 5
#define PENSAR 0
#define COMER 1
#define TEMPO_PENSANDO 2000//ms
#define TEMPO_COMENDO 2000//ms
int itr;
sem_t hashi[NUM_FILOSOFOS];//semáforo para cada hashi
sem_t mutex;

typedef struct Filosofo{
	int estado;
	int hashis_na_mao;
}Filosofo;

Filosofo filosofos[NUM_FILOSOFOS]; 

void pensar(){
	sleep(1);
}
void comer(){
	sleep(1);
}

void mostrarEstadoFilosofos(){
   for(int i = 0; i < NUM_FILOSOFOS; i++){
        if(filosofos[i].estado == PENSAR){
            printf("Filosofo %d pensando\n", i);
        }
        else if(filosofos[i].estado == COMER){
            printf("Filosofo %d comendo\n", i);
        }
   }
   printf("\n");
}
 
void pegarHashi(int filosofo_id, int i){
	sem_wait(&mutex); 
	if(filosofos[filosofo_id].hashis_na_mao >= 0 || filosofos[filosofo_id].hashis_na_mao < 2){
		filosofos[filosofo_id].hashis_na_mao++;
		printf("Filosofo %d segurou o hashi %d e esta aguardando\n", filosofo_id, i);
		filosofos[filosofo_id].estado = PENSAR;
		sleep(1);
	}

	if(filosofos[filosofo_id].hashis_na_mao == 2){
		filosofos[filosofo_id].estado = COMER;
		sleep(1);
	}

	mostrarEstadoFilosofos();
	sem_post(&mutex); 
	sem_wait(&hashi[i]);
}

void devolverHashi(int filosofo_id, int i){
	sem_wait(&mutex);

	if(filosofos[filosofo_id].hashis_na_mao >= 0){
		printf("Filosofo %d devolveu hashi %d\n", filosofo_id, i);
		filosofos[filosofo_id].hashis_na_mao--;
		filosofos[filosofo_id].estado = PENSAR;
		sem_post(&hashi[i]);
	}
   	
	mostrarEstadoFilosofos();
	sem_post(&mutex);
}


void *filosofo(void* j){
	int i = (int) j;
	printf("%d\n", i);
	int esquerda = (i+1) % NUM_FILOSOFOS;
	int direita = i;
	while(1){  
    	pensar(i);
    	pegarHashi(i, esquerda);
		pegarHashi(i, direita);
    	comer(i);
    	devolverHashi(i, esquerda);
		devolverHashi(i, direita);
	}
}


int main(){
	srand(time(NULL));

	for(itr = 0; itr < NUM_FILOSOFOS; itr++){
		filosofos[itr].estado = PENSAR;
		filosofos[itr].hashis_na_mao = 0;
	}

	pthread_t thread[NUM_FILOSOFOS];
	
	sem_init(&mutex, 0, 1);
	for(itr = 0; itr < NUM_FILOSOFOS; itr++){
    	sem_init(&hashi[itr], 0, 1);
	}
 
	for(itr = 0; itr < NUM_FILOSOFOS; itr++){
   	    pthread_create(&thread[itr], NULL, filosofo, (void*)itr);
	}
 
	for(itr = 0; itr < NUM_FILOSOFOS; itr++){
    	pthread_join(thread[itr],NULL);
	}
    return 0;
}


