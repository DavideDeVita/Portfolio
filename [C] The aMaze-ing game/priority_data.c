#include"priority_data.h"
									//		Davide De Vita
									//		N 86 / 1908
//Selettore di Metodo
	/* Imposta le funzioni per il tipo float */
		Method_t Get_float_mthd()
		{
			Method_t M;		
				M = (Method_t) malloc( sizeof(struct method));
				M -> Confronto = &Confronto_float;
				M -> Deallocazione = NULL;
				M -> Assegnazione = &Assegna_float;
		return M;
		}
		
		/* Imposta le funzioni per il tipo int */
		Method_t Get_int_mthd()
		{
			Method_t M;	
				M = (Method_t) malloc( sizeof(struct method));
				M -> Confronto = &Confronto_int;
				M -> Deallocazione = NULL;
				M -> Assegnazione = &Assegna_int;
		return M;
		}
//Callback

	//Int
		int Confronto_int(void *c, void *d)
		{
		int C, D;
			C = *((int *)c);
			D = *((int *)d);
		return ( (C<D) ? -1 : (C==D) ? 0 : 1 );
		}

		void *Assegna_int(void *target, void *valor)
		{
		int a;
			a= *((int *)valor);
			if(target ==NULL)
				target = malloc(sizeof(int));
			memcpy(target, &a, sizeof(int));
		return target;
		}
		
	//Float
		int Confronto_float(void *c, void *d)
		{
		float C, D;
			C = *((float *)c);
			D = *((float *)d);

		return ( (C<D) ? -1 : (C==D) ? 0 : 1 );
		}

		void *Assegna_float(void *target, void *valor)
		{
		float a;
			a= *((float *)valor);

			if(target ==NULL)
				target = malloc(sizeof(float));

			memcpy(target, &a, sizeof(float));
		return target;
		}
