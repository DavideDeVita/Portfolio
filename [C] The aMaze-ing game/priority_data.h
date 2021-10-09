#include"heap_structure.h"
								//	Davide De Vita
								//	N 86/1908
//Selettore di Metodo
		Method_t Get_int_mthd();
		Method_t Get_float_mthd();

//callback dati
	int Confronto_int(void *, void *);
	void *Assegna_int(void *, void *);
	
	int Confronto_float(void *, void *);
	void *Assegna_float(void *, void *);
