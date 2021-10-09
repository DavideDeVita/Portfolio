#include<stdlib.h>
#include<stdio.h>
#include"code.h"
									//	Davide De Vita
									//	N 86/1908
/* Crea una nuova Locazione (o Cella) */
Location New_Loc(int riga, int colonna)
{	Location L = malloc(sizeof(struct couple));
	L->riga = riga;
	L->colonna = colonna;
	return L;
}

/* Inizializza una coda */
Queue New_Q()
{	Queue Q = malloc(sizeof(struct queue));
	Q->head=NULL;
	Q->tail=NULL;
	return Q;
}

/* Aggiunge un elemento in coda */
Queue Accoda(Queue Q, Location Loc)
{	Coda New = malloc(sizeof(struct coda));
	New->Loc=New_Loc(Loc->riga, Loc->colonna);
	New->next = NULL;
	
	if(Q->tail==NULL)	// && Q->tail == NULL)
	{	Q->head = New;
		Q->tail = New;
	}
	else
	{	Q->tail->next=New;
		Q->tail = New;
	}
	return Q;
}

/* Rimuove un elemento dalla testa di una coda */
Queue Decoda(Queue Q)
{	Coda erase = Q->head;
	if(erase!=NULL)
	{	Q->head = erase->next;
		if(is_empty(Q))
			Q->tail=NULL;
		free(erase->Loc);
		free(erase);
	}
	/*else
		 The Queue is empty */
	return Q;
}

/* Il valore in testa alla coda */
Location Testa(Queue Q)
{	if(Q->head!=NULL)
		return Q->head->Loc;
	else
		return NULL;
}

/* Restituisce 1 se la coda è vuota, 0 altrimenti */
int is_empty(Queue Q)
{	if(Q->head==NULL)
		return 1;
	else
		return 0;
}
