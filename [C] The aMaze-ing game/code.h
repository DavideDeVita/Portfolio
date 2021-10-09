#include<stdlib.h>
#include<stdio.h>
#include"heap_queue.h"
									//	Davide De Vita
									//	N 86/1908
typedef struct couple
{	int riga;
	int colonna;
} *Location;

typedef struct coda
{	Location Loc;
	struct coda *next;
} *Coda;

typedef struct queue
{	Coda head;
	Coda tail;
} *Queue;

Location New_Loc(int, int);

Queue New_Q();
Queue Accoda(Queue, Location);
Queue Decoda(Queue);
Location Testa(Queue);
int is_empty(Queue);
