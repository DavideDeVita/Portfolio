#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
									//	Davide De Vita
									//	N 86/1908
//Tree
typedef struct Albero *Tree;

	struct Albero
	{	void *valor;
		void *prior;
		struct Albero *sx;
		struct Albero *dx;
		struct Albero *pre;
	};

//typedef	
typedef struct struttura *Method_st;

typedef struct coda_priorita *Heap;


//Definizioni di funzioni su tipi di dato
	typedef int (*CONFRONTO_F) (void *, void *);
	typedef void (*DEALLOCAZIONE_F) (void *);
	typedef void *(*ASSEGNAZIONE_F) (void *, void *);
	
//Metodi
typedef struct method *Method_t;

	struct method
	{	CONFRONTO_F Confronto;
		DEALLOCAZIONE_F Deallocazione;
		ASSEGNAZIONE_F Assegnazione;
	};

//Relazione d'ordine
	typedef int (*ORDINE)(Heap, void *, void *);
	
//Definizioni di funzioni su struttura
	typedef void (*ALLOCAZIONE_F) (Heap, void *);
	typedef void *(*HEAP_POS) (Heap, unsigned int);
	typedef void (*SET_VAL) (Heap, void *, void *);
	typedef void (*SET_PRIOR) (Heap, void *, void *);
	typedef void *(*GET_VAL) (void *);
	typedef void *(*GET_PRIOR) (void *);
	typedef void (*DECR_SIZE) (Heap);
	typedef void *(*LINK) (void *);


	struct struttura
	{	ALLOCAZIONE_F New;
		HEAP_POS i_esimo;
		SET_VAL set_value;
		SET_PRIOR set_prior;
		GET_VAL get_value;
		GET_PRIOR get_prior;
		DECR_SIZE Decreased;
		LINK sinistro;
		LINK destro;
		LINK padre;
	};

	struct coda_priorita
	{	void *heap;
		Method_t mthd_dato;
		Method_t mthd_prior;
		Method_st mthd_struttura;
		unsigned int heapsize;
		ORDINE Violazione;
	};


//	Violazione Ordine
	int Violazione_Max_Heap(Heap, void *, void *); //you just got Booolized
	int Violazione_Min_Heap(Heap, void *, void *);
//	Metodologia
	void get_Tree_mthd(Heap);
//	Metodi
	void *get_tree_p(void *);
	void *get_tree_v(void *);
	void set_tree_p(Heap, void *, void *);
	void set_tree_v(Heap, void *, void *);
	//Padre
		void *Tree_padre(void *);
	//Sinistro
		void *Tree_sx(void *);
	//Destro	
		void *Tree_dx(void *);
	void *nodo_i(Heap, unsigned int);
	void Alloca_Tree(Heap, void *);
	void Decrease_Tree(Heap);
	void Free_T(Heap);
