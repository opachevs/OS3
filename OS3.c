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
	int x, y, tmp =1;

	arr = (struct carParam *) arg;
	switch (arr->generator) {
	case 1:
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

		if (tmp) {
			pthread_mutex_lock(&(p->prev->lock));
			pthread_mutex_lock(&(p->lock));
			if (p->flag == 0 && p->prev->flag == 0) {
				p->flag = 1;
				tmp = 0;
			}
			pthread_mutex_unlock(&(p->lock));
			pthread_mutex_unlock(&(p->prev->lock));

		} else {


		}

	}
	return NULL;
}

void* Generator(void *arg) {
	pthread_t cars[CARS];
	struct carParam *arr;
	int counter = 0;
	int time, err, i;

	arr = (struct carParam *) arg;

	while (i < CARS) {
		arr[i++].license = -1;
		arr[i++].generator = *((int*) arg);
	}

	while (run) {
		time = (rand() % (MAX_INTER_ARRIVAL_IN_NS - MIN_INTER_ARRIVAL_IN_NS))
				+ MIN_INTER_ARRIVAL_IN_NS;
		usleep(time / 1000);

		arr[counter].license = counter;
		err = pthread_create(&(cars[counter]), NULL, &car, &(arr[counter]));
		if (err != 0)
			printf("\ncan't create thread :[%s]\n", strerror(err));

		for (i = 0; i < CARS; i++) {
			if (arr[i].license == -1)
				counter = i;
		}

	}

	return NULL;
}

int main(void) {
	int i = 0, j = 0, tmp = 0;
	struct square *p;

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (pthread_mutex_init(&(trafficCircle[i][j].lock), NULL) != 0) {
				printf("\n mutex init failed\n");
				return 1;
			}
			trafficCircle[i][j].flag = tmp++;
			switch (i) {
			case 0:
				if (j == 0) {
					trafficCircle[i][j].next = &trafficCircle[i + 1][j];
					trafficCircle[i][j].prev = &trafficCircle[i][j + 1];
				} else if (j == N - 1) {
					trafficCircle[i][j].next = &trafficCircle[i][j - 1];
					trafficCircle[i][j].prev = &trafficCircle[i + 1][j];
				} else {
					trafficCircle[i][j].next = &trafficCircle[i][j - 1];
					trafficCircle[i][j].prev = &trafficCircle[i][j + 1];
				}
				continue;
			case N - 1:
				if (j == N - 1) {
					trafficCircle[i][j].next = &trafficCircle[i - 1][j];
					trafficCircle[i][j].prev = &trafficCircle[i][j - 1];
				} else if (j == 0) {
					trafficCircle[i][j].next = &trafficCircle[i][j + 1];
					trafficCircle[i][j].prev = &trafficCircle[i - 1][j];
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
	p = &(trafficCircle[0][0]);
	for (i = 0; i < 30; i++) {
		if (p->prev != NULL)
			printf("%d   %d\n", p->flag, p->prev->flag);
		else
			printf("%d\n", p->flag);
		p = p->next;
	}

	return EXIT_SUCCESS;
}
