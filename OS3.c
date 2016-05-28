/*
 ============================================================================
 Name        : OS3.c
 Author      : Timmy Michael Opachevsky
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include<stdio.h>
#include <time.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

#define N 5
#define FIN_PROB 0.1
#define MIN_INTER_ARRIVAL_IN_NS 8000000
#define MAX_INTER_ARRIVAL_IN_NS 9000000
#define INTER_MOVES_IN_NS 100000
#define SIM_TIME 2

#define CARS 200

typedef struct square {
	int flag;
	int edge;
	pthread_mutex_t lock;
	struct square *next;
	struct square *prev;
} square;

typedef struct carParam {
	int generator;
	int license;
} carParam;

struct square trafficCircle[N][N];
int run = 1;
pthread_t carGenerators[4];
struct carParam arr1[CARS];
struct carParam arr2[CARS];
struct carParam arr3[CARS];
struct carParam arr4[CARS];

void* car(void *arg) {
	struct carParam *arr;
	struct square *p;
	double prob;
	int first = 1,i=0;

	arr = (struct carParam *) arg;
	switch (arr->generator) {              // with this switch we initialize the
	case 1:                                // starting point of the car
		p = &(trafficCircle[0][N - 1]);
		break;
	case 2:
		p = &(trafficCircle[0][0]);
		break;
	case 3:
		p = &(trafficCircle[N - 1][0]);
		break;
	case 4:
		p = &(trafficCircle[N - 1][N - 1]);
		break;
	}

	while (run) {
		pthread_mutex_lock(&(p->lock));
		if (p->flag == 0 && p->prev->flag == 0) {
			p->flag = 1;
			break;
		}
		pthread_mutex_unlock(&(p->lock));
	}
	pthread_mutex_unlock(&(p->lock));

	while (run) {
		usleep(INTER_MOVES_IN_NS / 1000);
		if ((p->edge) == 0 || first)
			prob = 1;
		else
			prob = ((double) (rand() % 1000)) / 1000;
		pthread_mutex_lock(&(p->lock));
		if(prob<FIN_PROB){
			p->flag=0;
			pthread_mutex_unlock(&(p->lock));
			break;
		}else if(p->next->flag==0){
			p->flag=0;
			p->next->flag=1;
			p=p->next;
			first=0;
		}
		pthread_mutex_unlock(&(p->prev->lock));
	}
	arr->license = -2;
	while (run) {
		i++;
	}

	return NULL;
}

void* Generator(void *arg) {
	pthread_t cars[CARS];
	struct carParam *arr;
	int counter = 0;
	int time, err, i = 0, j;

	arr = (struct carParam *) arg;

	for (i = 0; i < CARS; i++) {
		arr[i].license = -1;
		arr[i].generator = *((int*) arg);
	}
	j = 0;
	while (run) {
		time = (rand() % (MAX_INTER_ARRIVAL_IN_NS - MIN_INTER_ARRIVAL_IN_NS))
				+ MIN_INTER_ARRIVAL_IN_NS;
		usleep(time / 1000);

		arr[counter].license = counter;
		err = pthread_create(&(cars[counter]), NULL, &car, &(arr[counter]));
		if (err != 0)
			printf("\ncan't create thread :[%s]\n", strerror(err));

		for (i = 0; i < CARS; i++) {
			if (arr[i].license == -1) {
				counter = i;
				break;
			}
		}
		j++;
	}
	while (run) {
		i++;
	}
	return NULL;
}

void print() {
	int i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			printf("%d  ", trafficCircle[i][j].flag);
		}
		printf("\n");
	}
	printf("\n#############\n\n");
}

int main(void) {
	int i = 0, j = 0, err;
	//struct square *p;
	int num[] = { 1, 2, 3, 4 };

	srand(time(NULL));
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (pthread_mutex_init(&(trafficCircle[i][j].lock), NULL) != 0) {
				printf("\n mutex init failed\n");
				return 1;
			}
			trafficCircle[i][j].flag = 0;
			trafficCircle[i][j].edge = 0;
			switch (i) {
			case 0:
				if (j == 0) {
					trafficCircle[i][j].next = &trafficCircle[i + 1][j];
					trafficCircle[i][j].prev = &trafficCircle[i][j + 1];
					trafficCircle[i][j].edge = 1;
				} else if (j == N - 1) {
					trafficCircle[i][j].next = &trafficCircle[i][j - 1];
					trafficCircle[i][j].prev = &trafficCircle[i + 1][j];
					trafficCircle[i][j].edge = 1;
				} else {
					trafficCircle[i][j].next = &trafficCircle[i][j - 1];
					trafficCircle[i][j].prev = &trafficCircle[i][j + 1];
				}
				continue;
			case N - 1:
				if (j == N - 1) {
					trafficCircle[i][j].next = &trafficCircle[i - 1][j];
					trafficCircle[i][j].prev = &trafficCircle[i][j - 1];
					trafficCircle[i][j].edge = 1;
				} else if (j == 0) {
					trafficCircle[i][j].next = &trafficCircle[i][j + 1];
					trafficCircle[i][j].prev = &trafficCircle[i - 1][j];
					trafficCircle[i][j].edge = 1;
				} else {
					trafficCircle[i][j].next = &trafficCircle[i][j + 1];
					trafficCircle[i][j].prev = &trafficCircle[i][j - 1];
				}
				continue;
			}
			switch (j) {
			case 0:
				trafficCircle[i][j].next = &trafficCircle[i + 1][j];
				trafficCircle[i][j].prev = &trafficCircle[i - 1][j];
				continue;
			case N - 1:
				trafficCircle[i][j].next = &trafficCircle[i - 1][j];
				trafficCircle[i][j].prev = &trafficCircle[i + 1][j];
				continue;
			}
		}
	}

	for (i = 0; i < 1; i++) {
		err = pthread_create(&(carGenerators[i]), NULL, &Generator, &(num[i]));
		if (err != 0)
			printf("\ncan't create thread :[%s]\n", strerror(err));
	}
	for (i = 0; i < 10; i++) {
		usleep(SIM_TIME * 100000);
		print();
	}
	run = 0;

	sleep(5);
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			pthread_mutex_destroy(&(trafficCircle[i][j].lock));
		}
	}

	return EXIT_SUCCESS;
}
