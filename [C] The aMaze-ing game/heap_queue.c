#include "heap_queue.h"
#include <ncurses.h>
									//	Davide De Vita
									//	N 86/1908
/* Crea una Coda a priorità (Heap) (PER LE VISITE SCRITTE) */
Heap Crea_Coda(unsigned int type, unsigned int data_type)
{	Heap H = NULL;
	if( (type == MIN_HEAP || type == MAX_HEAP) && (data_type==FLOAT || data_type==INTEGER) )
	{	H = (Heap)malloc(sizeof(struct coda_priorita));
		H -> heap = NULL;
		get_Tree_mthd(H);
		H->mthd_dato = Get_int_mthd();	//convert to location
		if(data_type==INTEGER)
			H->mthd_prior = Get_int_mthd();
		else
			H->mthd_prior = Get_float_mthd();
		H->heapsize = 0;
		if(type == MIN_HEAP)
			H -> Violazione = &Violazione_Min_Heap;
		else if (type == MAX_HEAP)
			H -> Violazione = &Violazione_Max_Heap;
	}
return H;
}

/* Elimina e libera la memoria di un Heap */
void Delete_Heap(Heap H)
{
	while(H->heapsize>0)
		Decrease_Heapseize(H);
	free(H->heap);
	if(H->mthd_dato!=NULL)
		free(H->mthd_dato);
	free(H->mthd_prior);
	free(H->mthd_struttura);
	
	free(H);
}

/* Altera la priorità dell' elemento "di" a "con" */
void Alter_prior(Heap H, void *di, void *con)	//Heap, quale, nuovo valore
{	if(H->Violazione(H, H->mthd_struttura->get_prior(di), con))
		Increase_prior(H, di, con);
	else
		Decrease_prior(H, di, con);
}

/* Altera la priorità dell' elemento di valore "val_di" a "con" */
void Alter_ith_prior(Heap H, int val_di, void *con)
{	void *val = NULL, *prior=NULL;
	val = Assegna_int(val, &val_di);
	prior = H->mthd_prior->Assegnazione(prior, con);
	int position = Find_index(H, val);
	if(position>=0)
	{	val = H->mthd_struttura->i_esimo(H, position);
		Alter_prior(H, val, prior);
	}
}

/* Incrementa (Fa salire come priorità) */
void Increase_prior(Heap H, void *di, void *con)	//Salire
{	void *padre, *local = di;
	H->mthd_struttura->set_prior(H, local, con);
	padre = H->mthd_struttura->padre(local);
	while(padre!=NULL && (H->Violazione(H, H->mthd_struttura->get_prior(padre), con)==1) )
	{	Heap_Swap(H, local, padre);
		local = padre;
		padre = H->mthd_struttura->padre(padre);
	}
}

/* Decrementa (Fa scendere come priorità) */
void Decrease_prior(Heap H, void *di, void *con)	//Scendere
{	H->mthd_struttura->set_prior(H, di, con);
	Heapify(H, di);
}

/* Ristabilisce un Heap dati due sottoalberi Heap */
void Heapify(Heap H, void *root)
{	void *sx = H->mthd_struttura->sinistro(root);
	void *dx = H->mthd_struttura->destro(root);
	void *Max = root;
	
	if(sx!=NULL && H->Violazione(H, H->mthd_struttura->get_prior(Max), H->mthd_struttura->get_prior(sx)))
		Max = sx;

	if(dx!=NULL && H->Violazione(H, H->mthd_struttura->get_prior(Max), H->mthd_struttura->get_prior(dx)))
		Max = dx;

	if(Max != root)
	{	Heap_Swap(H, root, Max);
		Heapify(H, Max);
	}
}

/* Inserisce un nuovo valore nel Heap */
void Heap_Inserimento(Heap H, void *value, void *prior)
{	H->heapsize++;
	H->mthd_struttura->New(H, value);
	Alter_prior(H, H->mthd_struttura->i_esimo(H, H->heapsize), prior);
}

/* Estrae il valore di Priorità massima */
void *Heap_Estrai(Heap H)	// Un' altroooo!
{	void *select, *leaf, *ret=NULL;
	if(H->heapsize > 0)
	{	select = H->mthd_struttura->i_esimo(H, 1);
		if(H->mthd_dato!=NULL)
			ret = H->mthd_dato->Assegnazione(ret, H->mthd_struttura->get_value(select));
		else
			ret = H->mthd_prior->Assegnazione(ret, H->mthd_struttura->get_prior(select));
			
		if(H->heapsize!=1)
		{	leaf = H->mthd_struttura->i_esimo(H, H->heapsize);
			if(H->mthd_dato!=NULL)
				H->mthd_struttura->set_value(H, select, H->mthd_struttura->get_value(leaf));			
			H->mthd_struttura->set_prior(H, select, H->mthd_struttura->get_prior(leaf));
			Heapify(H, select);
		}
		Decrease_Heapseize(H);
	}
return ret;
}

/* Decrementa la dimensione eliminando `l' ultimo` elemento */
void Decrease_Heapseize(Heap H)
{
	H->mthd_struttura->Decreased(H);
	H->heapsize--;
}

/* Trova l'indice realtivo ad un valore (il primo indice) */
int Find_index(Heap H, void *val)
{	int ret=0, trovato=0;
	void *i_th=NULL;
	while(ret<H->heapsize && !trovato)
	{	ret++;
		i_th=H->mthd_struttura->i_esimo(H, ret);
		if(H->mthd_dato->Confronto(val, H->mthd_struttura->get_value(i_th))==0)
			trovato=1;
	}
	if(!trovato)
		ret = -1;
	return ret;
}

/* Esegue uno scambio di valori */
void Heap_Swap(Heap H, void *ex_root, void *max)
{	void *tmp=NULL;
		tmp = H->mthd_prior->Assegnazione(tmp, H->mthd_struttura->get_prior(ex_root));
		H->mthd_struttura->set_prior(H, ex_root, H->mthd_struttura->get_prior(max));
		H->mthd_struttura->set_prior(H, max, tmp);
		if(H->mthd_dato!=NULL)
		{	tmp = H->mthd_dato->Assegnazione(tmp, H->mthd_struttura->get_value(ex_root));
			H->mthd_struttura->set_value(H, ex_root, H->mthd_struttura->get_value(max));
			H->mthd_struttura->set_value(H, max, tmp);
		}
}
