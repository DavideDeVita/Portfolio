#include"heap_structure.h"
									//	Davide De Vita
									//	N 86/1908
//	Violazione Ordine
	/* Restituisce 1 se v'è una violazione nel Max Heap */
	int Violazione_Max_Heap(Heap H, void *padre, void *figlio) //you just got Booolized
	{	if(figlio != NULL)
		{	if(padre==NULL || H->mthd_prior->Confronto(figlio, padre)>0)
				return 1;
		}
		return 0;
	}

	/* Restituisce 1 se v'è una violazione nel Min Heap */
	int Violazione_Min_Heap(Heap H, void *padre, void *figlio)
	{	if(figlio != NULL)
		{	if(padre==NULL || H->mthd_prior->Confronto(figlio, padre)<0)
				return 1;
		}
		return 0;
	}

//	Metodologia
	/* Imposta le funzioni di struttura per un Heap gestito ad albero */
	void get_Tree_mthd(Heap H)
	{
		H->mthd_struttura=(Method_st)malloc(sizeof(struct struttura));
		H->mthd_struttura->New=&Alloca_Tree;
		H->mthd_struttura->i_esimo=&nodo_i;
		H->mthd_struttura->set_value=&set_tree_v;
		H->mthd_struttura->set_prior=&set_tree_p;
		H->mthd_struttura->get_prior=&get_tree_p;
		H->mthd_struttura->get_value=&get_tree_v;
		H->mthd_struttura->Decreased=&Decrease_Tree;
		H->mthd_struttura->sinistro=&Tree_sx;
		H->mthd_struttura->destro=&Tree_dx;
		H->mthd_struttura->padre=&Tree_padre;
	}

//											Metodi
//typedef void *(*GET_PRIOR) (void *);
		/* Restituisce la priorità del nodo 'i' */
		void *get_tree_p(void *i)
		{	Tree T = (Tree)i;
			
			if(T!=NULL)
				return T->prior;
			else
				return NULL;
		}


//typedef void *(*GET_VAL) (void *);
		/* Restituisce il valore del nodo 'i' */
		void *get_tree_v(void *i)
		{	Tree T = (Tree)i;
		
			if(T!=NULL)
				return T->valor;
			else
				return NULL;
		}

		
//typedef void (*SET_PRIOR) (Heap, void *, void *);	
	/* Imposta la priorità del nodo 'i' */
	void set_tree_p(Heap H, void *i, void *val)
	{	Tree T =(Tree)i;
		
		if(T!=NULL)
			T->prior = H->mthd_prior->Assegnazione(T->prior, val);
	}

//	typedef void (*SET_VAL) (Heap, void *, void *);
	/* Imposta il valore del nodo 'i' */
	void set_tree_v(Heap H, void *i, void *val)
	{	Tree T =(Tree)i;
		
		if(T!=NULL)
			T->valor = H->mthd_dato->Assegnazione(T->valor, val);
	}


//typedef void *(*LINK) (void *);
	//Padre
		/* Restituisce il Padre del nodo 'nodo' */
		void *Tree_padre(void *nodo)
		{	Tree T = (Tree)nodo;
			return (void *)(T->pre);
		}
		
	//Sinistro
		/* Restituisce il sinistro del nodo 'nodo' */
		void *Tree_sx(void *nodo)
		{	Tree T = (Tree)nodo;
			return T->sx;
		}
		
	//Destro
		/* Restituisce il destro del nodo 'nodo' */
		void *Tree_dx( void *nodo)
		{	Tree T = (Tree)nodo;
			return T->dx;
		}

//typedef void *(*HEAP_POS) (Heap, unsigned int);
	/* Restituisce l'iesimo nodo del Heap */
	void *nodo_i(Heap H, unsigned int i)
	{
	Tree Curr=(Tree)H->heap;
	unsigned int b = 8*(sizeof(unsigned int))-1;
	
		if(i>0 && i<=H->heapsize)
		{	for( ;( (i & (1<<b))==0 && b>0); b--);
			//N.B.: 	b qui è la posizione del primo uno se presente oppure 0
			while(b>0)
			{	b--;	//il primo 1 si ignora
				if( (i & (1<<b) )==0)
					Curr = Curr->sx;
				else
					Curr = Curr->dx;
			}
			return Curr;
		}
		else
			return NULL;
	}
	
//	ALLOCAZIONE ();
	/* Alloca e inserisce un nuovo nodo nel Heap */
	void Alloca_Tree(Heap H, void *new_val)
	{
	Tree T= NULL, Padre;
		Padre = (Tree)nodo_i(H, H->heapsize/2);
		T = (Tree)malloc(sizeof(struct Albero));
		T->valor = NULL;
		T -> sx = NULL;
		T -> dx = NULL;
		T -> pre = Padre;
		
		if(H->mthd_dato!=NULL)
			T -> valor = H->mthd_dato->Assegnazione(T->valor, new_val);
		T -> prior = NULL;	//Default priorità sempre in basso
		if(Padre==NULL)
			H->heap=T;
		else
		{	if(H->heapsize%2==0)
				Padre->sx = T;
			else
				Padre->dx = T;
		}
	}

//	typedef void (*DECR_SIZE) (Heap);
	/* Rimuove l'ultimo nodo */
	void Decrease_Tree(Heap H)
	{	Tree Curr;
		Free_T(H);
		Curr = (Tree)nodo_i(H, H->heapsize/2);
		if(H->heapsize>1)
		{	if(H->heapsize%2==0)
				Curr->sx = NULL;
			else
				Curr->dx = NULL;
		}
		else
			H->heap=NULL;
	}
	
	/* Dealloca l'ultimo nodo */
	void Free_T(Heap H)
	{	Tree Curr = (Tree)nodo_i(H, H->heapsize);
		if(H->mthd_dato!=NULL)
		{	if(H->mthd_dato->Deallocazione!=NULL)
				H->mthd_dato->Deallocazione(Curr->valor);
		}
			if(H->mthd_prior->Deallocazione!=NULL)
				H->mthd_prior->Deallocazione(Curr->prior);
		free(Curr);
		Curr=NULL;
	}
