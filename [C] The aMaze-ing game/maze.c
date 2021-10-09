#include "maze.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
										// Davide De Vita
										// N86001908
/* Crea un Maze strutturato come una griglia */
MAZE Make_Grid(unsigned int effective_rows, unsigned int effective_cols, int open_path)
{	MAZE Maze = malloc(sizeof(struct maze));
	Maze->map = malloc(sizeof(char*)*2*effective_rows);
	Maze->rows=2*effective_rows;
	Maze->columns=2*effective_cols;
	int i, j;
	for(i=0; i<Maze->rows; i++)
	{	Maze->map[i] = malloc(sizeof(char)*effective_cols*2);
		for(j=0; j<Maze->columns; j++)
		{	/*	Griglia	*/	//righe e colonne max-1 sono percorsi toroidali
			if(i%2==0 && j%2==0)
				Maze->map[i][j]=WAY;
			else
			{	if(open_path)	//Griglia aperta o chiusa?
				{	if(i%2==1 && j%2==1)
						Maze->map[i][j]=WALL;
					else
						Maze->map[i][j]=WAY;
				}
				else
					Maze->map[i][j]=WALL;
			}
		}
	}
return Maze;
}

/* Crea un Maze riempito con mura o corridoi in base al valore Deafault */
MAZE Alloc_Maze(unsigned int rows, unsigned int cols, char Default)
{	MAZE Maze = malloc(sizeof(struct maze));
	Maze->map = malloc(sizeof(char*)*rows);
	int i, j;
	for(i=0; i<rows; i++)
	{	Maze->map[i] = malloc(sizeof(char)*cols);
		for(j=0; j<cols; j++)
			Maze->map[i][j]=Default;
	}
	Maze->rows=rows;
	Maze->columns=cols;
return Maze;
}

/* Libera la memoria di un Maze */
void Free_Maze(MAZE free_me)
{	int i;
	for(i=0; i<free_me->rows; i++)
		free(free_me->map[i]);
	free(free_me->map);
	free(free_me);
}

/* Conta il numero di celle WAY di un Maze */
int Ways_Cells(MAZE Maze)
{	int ways, i, j;
	ways=0;
	for(i=0; i<Maze->rows; i++)
		for(j=0; j<Maze->columns; j++)
			if(Maze->map[i][j]==WAY)
				ways++;
	return ways;
}

/* BFS - se win!=NULL verranno visualizzati i passaggi */
Map_info BFS(MAZE M, Location from, Location to, WINDOW* win)
{	Location v;
	Queue Q = New_Q();
	int **colore;
	Map_info Info;
	Info.rows=M->rows;
	Info.columns=M->columns;
	Adj_List adj;
	int i, j;
	int delay=10, ex_delay=0, skip=0;
	int found=0, iterations=0;
	//Init
	colore = malloc(sizeof(int*)*M->rows);
	Info.pred = malloc(sizeof(int*)*M->rows);
	Info.dist = malloc(sizeof(int*)*M->rows);
	for(i=0; i< M->rows; i++)
	{	colore[i]= malloc(sizeof(int)*M->columns);
		Info.pred[i]= malloc(sizeof(int)*M->columns);
		Info.dist[i]= malloc(sizeof(int)*M->columns);
		for(j=0; j<M->columns; j++)
		{	colore[i][j] = Bianco;
			Info.pred[i][j] = NO_PATH;
			Info.dist[i][j] = -1;
		}
	}
	if(win)	//if show steps
	{	Display_Maze(win, M, 1);
		wattron(win, COLOR_PAIR(7));
	}
		
	if(Exists_Location(M, from)==1 && Exists_Location(M, to)==1 )
	{	Q=Accoda(Q, from);
		colore[from->riga][from->colonna] = Grigio;
		Info.pred[from->riga][from->colonna] = START_V;
		Info.dist[from->riga][from->colonna] = 0;
		if(win && !skip)
		{	delay=Haste_Slow(win, delay, &skip);
			ex_delay=Update_delay(delay, ex_delay);
			while(!refresh_time(delay));
			wmove(win, window_row(from->riga), window_column(from->colonna));
			waddch(win, ' ');
			waddch(win, ' ');
			wrefresh(win);
		}
		while(!is_empty(Q) && !found)
		{	iterations++;
			v=Testa(Q);
			
			if(Same_Location(to, v))
			{	found=1;
				while (!is_empty(Q))
					Q = Decoda(Q);
			}
			else
			{	adj = Locazioni_adiacenti(M, v);
				while(adj!=NULL)
				{	if(colore[adj->locazione->riga][adj->locazione->colonna]==Bianco)
					{	colore[adj->locazione->riga][adj->locazione->colonna]=Grigio;
						if(win && !skip)
						{	delay = Haste_Slow(win, delay, &skip);
							ex_delay = Update_delay(delay, ex_delay);
							while(!refresh_time(delay));
							wmove(win, window_row(adj->locazione->riga), window_column(adj->locazione->colonna));
							waddch(win, ' ');
							waddch(win, ' ');
							wrefresh(win);
						}
						Info.pred[adj->locazione->riga][adj->locazione->colonna] = To_Number(v, M);
						Info.dist[adj->locazione->riga][adj->locazione->colonna] = Info.dist[v->riga][v->colonna]+1;
						Q = Accoda(Q, adj->locazione);
					}
					adj = Next(adj);
				}
				colore[v->riga][v->colonna] = Nero;
				Q = Decoda(Q);
			}
		}
		for(i=0; i< M->rows; i++)
			free(colore[i]);
		free(colore);
	}
	Info.steps = iterations;
	return Info;
}

/* A Star - se win!=NULL verranno visualizzati i passaggi
	N.B.: Se il Labirinto è toroidale ma l'euristica no potrebbe non essere trovato il percorso minimo
	Scelta voluta per non far fare alle guardie lo stesso percorso */
Map_info A_Star(MAZE M, Location from, Location to, int Euristic, WINDOW *win)
{	Map_info Info;
	Heap Q;
	int found = 0, i, j;
	int delay=10, ex_delay=0, skip=0;
	Info.rows=M->rows;
	Info.columns=M->columns;
	unsigned int iterations=0;
	if(Exists_Location(M, from)==1 && Exists_Location(M, to)==1 && (Euristic==MANHATTAN || Euristic==EUCLIDEAN || Euristic==T_MANHATTAN || Euristic==T_EUCLIDEAN))
	{	int v, position;
		Location V;
		void *heap_head=NULL;
		Adj_List adj;
		float cost;
		Info.pred = malloc(sizeof(int*)*M->rows);
		Info.dist = malloc(sizeof(int*)*M->rows);
		for(i=0; i< M->rows; i++)
		{	Info.pred[i]= malloc(sizeof(int)*M->columns);
			Info.dist[i]= malloc(sizeof(int)*M->columns);
			for(j=0; j<M->columns; j++)
			{	Info.pred[i][j] = NO_PATH;
				Info.dist[i][j] = -1;
			}
		}
		Info.pred[from->riga][from->colonna]=START_V;
		Info.dist[from->riga][from->colonna]=0;
		if(Euristic == MANHATTAN || Euristic == T_MANHATTAN)
			Q=Crea_Coda(MIN_HEAP, INTEGER);
		else if(Euristic ==EUCLIDEAN || Euristic ==T_EUCLIDEAN)
			Q=Crea_Coda(MIN_HEAP, FLOAT);
		if(win)
		{	Display_Maze(win, M, 1);
			wattron(win, COLOR_PAIR(7));
		}
		v = To_Number(from, M);
		Heap_Inserimento(Q, &v, &found);
		while(Q->heapsize>0 && !found)
		{	iterations++;
			heap_head=Heap_Estrai(Q);
			v = *((int *)heap_head);
			V = To_Location(Info, v);
			if(win && !skip)
			{	delay=Haste_Slow(win, delay, &skip);
				ex_delay=Update_delay(delay, ex_delay);
				while(!refresh_time(delay));
				wmove(win, window_row(V->riga), window_column(V->colonna));
				waddch(win, ' ');
				waddch(win, ' ');
				wrefresh(win);
			}
			if(Same_Location(V, to))
				found = 1;
			else
			{	adj = Locazioni_adiacenti(M, V);
				while(adj!=NULL)
				{	cost = Info.dist[V->riga][V->colonna]+1;
					if(Euristic == MANHATTAN)
						cost += Manhattan(adj->locazione, to);
					else if(Euristic ==EUCLIDEAN)
						cost += Euclidean(adj->locazione, to);
					else if(Euristic == T_MANHATTAN)
						cost += Thoroid_Manhattan(M, adj->locazione, to);
					else if(Euristic ==T_EUCLIDEAN)
						cost += Thoroid_Euclidean(M, adj->locazione, to);
					int passing = To_Number(adj->locazione, M);
					position = Find_index(Q, &passing);
					if(position !=-1 && (Info.dist[adj->locazione->riga][adj->locazione->colonna]>Info.dist[V->riga][V->colonna]+1 || Info.dist[adj->locazione->riga][adj->locazione->colonna]==-1))
					{	Info.pred[adj->locazione->riga][adj->locazione->colonna] = v;
						Info.dist[adj->locazione->riga][adj->locazione->colonna] = Info.dist[V->riga][V->colonna]+1;
						Alter_ith_prior(Q, passing, &cost);
					}
/* Non necessario */		else if(position==-1 && Info.pred[adj->locazione->riga][adj->locazione->colonna]!=NO_PATH && Info.dist[adj->locazione->riga][adj->locazione->colonna]>Info.dist[V->riga][V->colonna]+1)
/* se l'euristica è */		{	Info.pred[adj->locazione->riga][adj->locazione->colonna] = v;
/*  Monotona */				Info.dist[adj->locazione->riga][adj->locazione->colonna] = Info.dist[V->riga][V->colonna]+1;
						Heap_Inserimento(Q, &passing, &cost);
					}
					else if(position ==-1 && Info.pred[adj->locazione->riga][adj->locazione->colonna]==NO_PATH)
					{	Info.pred[adj->locazione->riga][adj->locazione->colonna] = v;
						Info.dist[adj->locazione->riga][adj->locazione->colonna] = Info.dist[V->riga][V->colonna]+1;
						Heap_Inserimento(Q, &passing, &cost);
					}
					adj = Next(adj);
				}					
			}
		}
		Delete_Heap(Q);
	}
	Info.steps = iterations;
	return Info;
}

/* Versione Greedy dell'algoritmo A Star che basa la priorità SOLO sull'euristica
	Non è detto trovi il percorso minimo.
	La aggiungo per far fare alle guardie percorsi diversi e perchè è notevolmente più rapida
		difatti questa ricerca verrà eseguita dalle guardie che non hanno effettivamente visto
		il giocatore duqnue non è necessario facciano il percorso minimo ma essendo molteplici
		è utile che ci mettano poco */
Map_info A_Star_G(MAZE M, Location from, Location to, int Euristic, WINDOW *win)
{	Map_info Info;
	Heap Q;
	int found = 0, i, j;
	int delay=10, ex_delay=0, skip=0;
	Info.rows=M->rows;
	Info.columns=M->columns;
	unsigned int iterations=0;
	if(Exists_Location(M, from)==1 && Exists_Location(M, to)==1 && (Euristic==MANHATTAN || Euristic==EUCLIDEAN || Euristic==T_MANHATTAN || Euristic==T_EUCLIDEAN))
	{	int v, position;
		Location V;
		void *heap_head=NULL;
		Adj_List adj;
		float cost;
		Info.pred = malloc(sizeof(int*)*M->rows);
		Info.dist = malloc(sizeof(int*)*M->rows);
		for(i=0; i< M->rows; i++)
		{	Info.pred[i]= malloc(sizeof(int)*M->columns);
			Info.dist[i]= malloc(sizeof(int)*M->columns);
			for(j=0; j<M->columns; j++)
			{	Info.pred[i][j] = NO_PATH;
				Info.dist[i][j] = -1;
			}
		}
		Info.pred[from->riga][from->colonna]=START_V;
		Info.dist[from->riga][from->colonna]=0;
		if(Euristic == MANHATTAN || Euristic == T_MANHATTAN)
			Q=Crea_Coda(MIN_HEAP, INTEGER);
		else if(Euristic ==EUCLIDEAN || Euristic ==T_EUCLIDEAN)
			Q=Crea_Coda(MIN_HEAP, FLOAT);
		if(win)
		{	Display_Maze(win, M, 1);
			wattron(win, COLOR_PAIR(7));
		}
		v = To_Number(from, M);
		Heap_Inserimento(Q, &v, &found);
		while(Q->heapsize>0 && !found)
		{	iterations++;
			heap_head=Heap_Estrai(Q);
			v = *((int *)heap_head);
			V = To_Location(Info, v);
			if(win && !skip)
			{	delay=Haste_Slow(win, delay, &skip);
				ex_delay=Update_delay(delay, ex_delay);
				while(!refresh_time(delay));
				wmove(win, window_row(V->riga), window_column(V->colonna));
				waddch(win, ' ');
				waddch(win, ' ');
				wrefresh(win);
			}
			if(Same_Location(V, to))
				found = 1;
			else
			{	adj = Locazioni_adiacenti(M, V);
				while(adj!=NULL)
				{	cost = 0;//Info.dist[V->riga][V->colonna]+1;
					if(Euristic == MANHATTAN)
						cost += Manhattan(adj->locazione, to);
					else if(Euristic ==EUCLIDEAN)
						cost += Euclidean(adj->locazione, to);
					else if(Euristic == T_MANHATTAN)
						cost += Thoroid_Manhattan(M, adj->locazione, to);
					else if(Euristic ==T_EUCLIDEAN)
						cost += Thoroid_Euclidean(M, adj->locazione, to);
					int passing = To_Number(adj->locazione, M);
					position = Find_index(Q, &passing);
					if(position !=-1 && (Info.dist[adj->locazione->riga][adj->locazione->colonna]>Info.dist[V->riga][V->colonna]+1 || Info.dist[adj->locazione->riga][adj->locazione->colonna]==-1))
					{	Info.pred[adj->locazione->riga][adj->locazione->colonna] = v;
						Info.dist[adj->locazione->riga][adj->locazione->colonna] = Info.dist[V->riga][V->colonna]+1;
						Alter_ith_prior(Q, passing, &cost);
					}
/*Come sopra*/			else if(position==-1 && Info.pred[adj->locazione->riga][adj->locazione->colonna]!=NO_PATH && Info.dist[adj->locazione->riga][adj->locazione->colonna]>Info.dist[V->riga][V->colonna]+1)
					{	Info.pred[adj->locazione->riga][adj->locazione->colonna] = v;
						Info.dist[adj->locazione->riga][adj->locazione->colonna] = Info.dist[V->riga][V->colonna]+1;
						Heap_Inserimento(Q, &passing, &cost);
					}
					else if(position ==-1 && Info.pred[adj->locazione->riga][adj->locazione->colonna]==NO_PATH)
					{	Info.pred[adj->locazione->riga][adj->locazione->colonna] = v;
						Info.dist[adj->locazione->riga][adj->locazione->colonna] = Info.dist[V->riga][V->colonna]+1;
						Heap_Inserimento(Q, &passing, &cost);
					}
					adj = Next(adj);
				}					
			}
		}
		Delete_Heap(Q);
	}
	Info.steps = iterations;
	return Info;
}

/* Stima euristica di Manhattan calcolata per mappe toroidali */
int Thoroid_Manhattan(MAZE M, Location da, Location a)
{	int stima=0;
	if(da->riga < a->riga)
		stima = int_min( abs(a->riga-da->riga), abs(da->riga+M->rows-a->riga) );
	else
		stima = int_min( abs(da->riga-a->riga), abs(da->riga-M->rows-a->riga) );

	if(da->colonna < a->colonna)
		stima += int_min( abs(da->colonna-a->colonna), abs(da->colonna+M->columns-a->colonna) );
	else
		stima += int_min( abs(da->colonna-a->colonna), abs(da->colonna-M->columns-a->colonna) );

	return stima;
}

/* Stima euristica di Manhattan */
int Manhattan(Location da, Location a)
{	return abs(a->riga - da->riga)+abs(a->colonna - da->colonna);
}

/* Minimo fra due interi */
int int_min(int c, int d)
{	if(c<=d)
		return c;
	else
		return d;
}

/* Minimo fra due reali */
float float_min(float c, float d)
{	if(c<=d)
		return c;
	else
		return d;
}

/* returns x^2 */
int pow2(int x)
{	return x*x;
}

/* Stima euristica di Euclide calcolata per mappe toroidali */
float Thoroid_Euclidean(MAZE M, Location da, Location a)
{	float stima=0;
	if(da->riga < a->riga)
		stima = float_min( pow2(da->riga-a->riga), pow2(da->riga+M->rows-a->riga) );
	else
		stima = float_min( pow2(da->riga-a->riga), pow2(da->riga-M->rows-a->riga) );

	if(da->colonna < a->colonna)
		stima += float_min( pow2(da->colonna-a->colonna), pow2(da->colonna+M->columns-a->colonna) );
	else
		stima += float_min( pow2(da->colonna-a->colonna), pow2(da->colonna-M->columns-a->colonna) );

	return sqrt(stima);
}

/* Stima euristica di Euclide */
float Euclidean(Location da, Location a)
{	return sqrt(pow2(a->riga - da->riga)+pow2(a->colonna - da->colonna));
}
					/* GESTIONE DELLE ADJ_LIST E LOCAZIONI */
/* Verifica se ha senso calcolare o meno il percorso trovato */
Adj_List Compute_Path(MAZE Maze, Map_info Info, Location da, Location a)
{	if(Exists_Location( Maze, da)==1 && Exists_Location(Maze, a)==1)
	{	if(Pred(Info, a)!= NO_PATH)
			return Make_Path(Info, da, a);
		else
			return NULL;
	}	
	else
		return NULL;
}

/* Crea un Adj_List contenente il percorso calcolato da una visita */
Adj_List Make_Path(Map_info Info, Location da, Location a)
{	Adj_List adj=NULL;
	while(!Same_Location(da, a))
	{	adj = Insert_Location(adj, a);
		a = To_Location(Info, Pred(Info, a));
	}
	adj = Insert_Location(adj, da);
	return adj;
}

/* Computa il corretto messaggio d'errore causato dalle locazioni */
char* Unexisting_Location_path(MAZE Maze, Location da, Location a)
{	unsigned int error_type = 0+(Exists_Location(Maze, da)==-1)+(2*(Exists_Location(Maze, da)==0));
	error_type += (3*(Exists_Location(Maze, a)==-1))+(6*(Exists_Location(Maze, a)==0));
	char *msg=NULL;
	if( error_type==1 )
	{	msg = malloc(sizeof(char)*strlen("La sorgente  (punto [%d,%d]) è un muro!\n"));
		sprintf(msg, "La sorgente  (punto [%d,%d]) è un muro!", da->riga, da->colonna);
	}
 	else if( error_type==2 )
	{	msg = malloc(sizeof(char)*strlen("La sorgente  (punto [%d,%d]) non esiste!\n"));
		sprintf(msg, "La sorgente  (punto [%d,%d]) non esiste!", da->riga, da->colonna);
	}
	else if( error_type==3 )
 	{	msg = malloc(sizeof(char)*strlen("La destinazione  (punto [%d,%d]) è un muro!\n"));
		sprintf(msg, "La destinazione  (punto [%d,%d]) è un muro!", a->riga, a->colonna);
	}
	else if( error_type==4 )
	{	msg = malloc(sizeof(char)*strlen("I due punti (sorgente [%d,%d] e destinazione [%d,%d]) sono muri!\n"));
		sprintf(msg, "I due punti (sorgente [%d,%d] e destinazione [%d,%d]) sono muri!", da->riga, da->colonna, a->riga, a->colonna);
	}
 	else if( error_type==5 )
	{	msg = malloc(sizeof(char)*strlen("La destinazione (punto [%d,%d]) è un muro, mentre la sorgente [%d,%d] non esiste!\n"));
		sprintf(msg, "La destinazione (punto [%d,%d]) è un muro, mentre la sorgente [%d,%d] non esiste!", a->riga, a->colonna, da->riga, da->colonna);
	}
 	else if( error_type==6 )
	{	msg = malloc(sizeof(char)*strlen("La destinazione  (punto [%d,%d]) non esiste!\n"));
		sprintf(msg, "La destinazione  (punto [%d,%d]) non esiste!", a->riga, a->colonna);
	}
	else if( error_type==7 )
 	{	msg = malloc(sizeof(char)*strlen("La sorgente (punto [%d,%d]) è un muro, mentre la destinazione [%d,%d] non esiste!\n"));
		sprintf(msg, "La sorgente (punto [%d,%d]) è un muro, mentre la destinazione [%d,%d] non esiste!", da->riga, da->colonna, a->riga, a->colonna);
	}
	else if (error_type==8 )
 	{	msg = malloc(sizeof(char)*strlen("I due punti (sorgente [%d,%d] e destinazione [%d,%d]) non esistono!\n"));
		sprintf(msg, "I due punti (sorgente [%d,%d] e destinazione [%d,%d]) non esistono!", da->riga, da->colonna, a->riga, a->colonna);
	}
	else
	{	msg = malloc(sizeof(char)*strlen("Murphy esiste\n"));
		sprintf(msg, "Murphy esiste");
	}
	return msg;
}

/* Verifica se 2 Locazioni coincidono */
int Same_Location(Location da, Location a)
{	return ( (da->riga==a->riga) && (da->colonna==a->colonna) );
}

/* Calcola il numero indicante il predecessore */
int Pred(Map_info Info, Location l)
{	return Info.pred[l->riga][l->colonna];
}

/* Converte un numero in una Locazione */
Location To_Location(Map_info Info, int number)
{	Location Ret = malloc(sizeof(struct couple));
	if(number%Info.columns==0){
		Ret->colonna=0;
		Ret->riga=number/Info.columns;
	}
	else{
		Ret->riga=(number/Info.columns);
		number -= (number/Info.columns)*Info.columns;
		Ret->colonna=number;
	}
	return Ret;
}

/* Converte una Locazione in un numero */
int To_Number(Location L, MAZE Maze)
{	return (L->riga*Maze->columns)+L->colonna;
}

/* Restituisce una lista contenente gli adiacenti WAY */
Adj_List Locazioni_adiacenti(MAZE M, Location L)
{	Adj_List adj=NULL;
	if(Exists_Location(M, L))
	{	if(M->map[(L->riga+M->rows-1)%M->rows][L->colonna]==WAY)
			adj = Insert_adj_list(adj, (L->riga+M->rows-1)%M->rows, L->colonna);
		if(M->map[L->riga][(L->colonna+M->columns-1)%M->columns]==WAY)
			adj = Insert_adj_list(adj, L->riga, (L->colonna+M->columns-1)%M->columns);
		if(M->map[(L->riga+1)%M->rows][L->colonna]==WAY)
			adj = Insert_adj_list(adj, (L->riga+1)%M->rows, L->colonna);
		if(M->map[L->riga][(L->colonna+1)%M->columns]==WAY)
			adj = Insert_adj_list(adj, L->riga, (L->colonna+1)%M->columns);
	}
	return adj;
}

/* Restituisce una lista contenente gli adiacenti WALL */
Adj_List Get_Walls_Around(MAZE M, Location L)
{	Adj_List adj=NULL;
	if(Exists_Location(M, L))
	{	if(M->map[(L->riga+M->rows-1)%M->rows][L->colonna]==WALL)
			adj = Insert_adj_list(adj, (L->riga+M->rows-1)%M->rows, L->colonna);
		if(M->map[L->riga][(L->colonna+M->columns-1)%M->columns]==WALL)
			adj = Insert_adj_list(adj, L->riga, (L->colonna+M->columns-1)%M->columns);
		if(M->map[(L->riga+1)%M->rows][L->colonna]==WALL)
			adj = Insert_adj_list(adj, (L->riga+1)%M->rows, L->colonna);
		if(M->map[L->riga][(L->colonna+1)%M->columns]==WALL)
			adj = Insert_adj_list(adj, L->riga, (L->colonna+1)%M->columns);
	}
	return adj;
}

/* Restituisce una lista contenente i vicoli ciechi del Labirinto */
Adj_List Get_Dead_Ends(MAZE M)
{	Location L = New_Loc(0, 0);
	Adj_List adj=NULL, dead_ends=NULL;
	for(L->riga=0; L->riga<M->rows; L->riga++)
	{	for(L->colonna=0; L->colonna<M->columns; L->colonna++)
		{	if(M->map[L->riga][L->colonna]==WAY)
			{	adj = Locazioni_adiacenti(M, L);
				if(List_Size(adj)==1)
					dead_ends = Insert_Location(dead_ends, L);
				Free_List(adj);
			}
		}
	}
	return dead_ends;
}

/* Restituisce 1 se la Locazione è WAY, -1 se è WALL, 0 se non esiste */
int Exists_Location(MAZE M, Location L)
{	if(L->riga>=0 && L->colonna>=0 && L->riga<M->rows && L->colonna<M->columns)
	{	if(M->map[L->riga][L->colonna]==WAY)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}
/*Liste*/
/* Inserimento in Lista */
Adj_List Insert_adj_list(Adj_List adj, unsigned int riga, unsigned int colonna)
{	Adj_List new = malloc(sizeof(struct list_of_adjs));
	new->next = adj;
	new->locazione = New_Loc(riga, colonna);
	return new;
}

/* Inserimento in Lista usando Locazione*/
Adj_List Insert_Location(Adj_List adj, Location L)
{	Adj_List new = malloc(sizeof(struct list_of_adjs));
	new->next = adj;
	new->locazione = New_Loc(L->riga, L->colonna);
	return new;
}

/* Funzione per scorrere una lista da eliminare:
	Restituisce il next di un nodo cancellando il nodo attuale.
	Utilizzata anche nella cancellazione */
Adj_List Next(Adj_List adj)
{	Adj_List erase = adj, ret= adj->next;
	free(erase->locazione);
	free(erase);
	return ret;
}

/* Rimuove la PRIMA occorrenza della Locazione se presente */
Adj_List Remove_Loc(Adj_List adj, Location L)
{	Adj_List Pre = NULL, eye=adj;
	while(eye!=NULL && !Same_Location(eye->locazione, L))
	{	Pre = eye;
		eye=eye->next;
	}
	if(eye!=NULL)
	{	if(Pre==NULL)
			adj = Next(adj);
		else
			Pre->next = Next(Pre->next);
	}
	return adj;
}

/* Restituisce 1 se L appartiene alla Lista, 0 altrimenti */
int Is_in(Adj_List List, Location L)
{	Adj_List eye=List;
	int in=0;
	while(eye && !in)
	{	if(Same_Location(eye->locazione, L))
			in = 1;
		eye=eye->next;
	}
	return in;
}

/* Restituisce la cardinalità della Lista */
int List_Size(Adj_List adj)
{	int ret=0;
	Adj_List eye=adj;
	while(eye!=NULL)
	{	ret++;
		eye=eye->next;
	}
	return ret;
}

/* Distrugge e libera la memoria occupata dalla Lista */
void Free_List(Adj_List List)
{	while(List)
		List=Next(List);
}

/* Restituisce un elemento a caso della Lista */
Location Get_Random_element(Adj_List adj)
{	if(adj)
	{	int which=rand()%(List_Size(adj));
		Adj_List eye=adj;
		while(which>0){
			eye=eye->next;
			which--;
		}
		if(eye)
			return New_Loc(eye->locazione->riga, eye->locazione->colonna);
		else
			return NULL;
	}
	else
		return NULL;
}

/* Capovolge la Lista */
Adj_List Revert_List(Adj_List List)	//It
{	Adj_List Curr=List, Pre=NULL, temp=NULL;
	while(Curr)
	{	temp=Curr->next;
		Curr->next=Pre;
		Pre=Curr;
		Curr=temp;
	}
	return Pre;
}

/* Se la Locazione è allocata ne libera la memoria */
void If_Free(Location erase)
{	if(erase)
		free(erase);
}
