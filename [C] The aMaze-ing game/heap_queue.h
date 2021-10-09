#include "priority_data.h"
									//	Davide De Vita
									//	N 86/1908
//Creazione Nuova Coda
#define MIN_HEAP 0
#define MAX_HEAP 1
#define INTEGER 1
#define FLOAT 0

Heap Crea_Coda(unsigned int, unsigned int);
void Delete_Heap(Heap);
//Funzioni
void Alter_prior(Heap, void *, void *);
void Alter_ith_prior(Heap, int, void *);
void Increase_prior(Heap, void *, void *);
void Decrease_prior(Heap, void *, void *);
void Heapify(Heap, void *);
void Heap_Inserimento(Heap, void *, void *);
void *Heap_Estrai(Heap);
void Decrease_Heapseize(Heap);
int Find_index(Heap, void *);
void Heap_Swap(Heap, void *, void *);
