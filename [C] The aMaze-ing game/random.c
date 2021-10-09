#include "maze.h"
									//	Davide De Vita
									//	N 86/1908
int H_V=0, delay=14, ex_delay=0, skip=0;
/* Creazione casuale tramite Prim */
MAZE Prim_Random_Walk(unsigned int rows, unsigned int cols, WINDOW *win)
{	MAZE Maze = Alloc_Maze(2*rows, 2*cols, WALL);
	Adj_List List=NULL, adj=NULL;
	Location cell=NULL, newcell=NULL;
	cell=Random_cell_even(Maze);
	Maze->map[cell->riga][cell->colonna]=WAY;
	List = Insert_Location(List, cell);
	int Rooms_Left=rows*cols-1;
	while(Rooms_Left>0 && List!=NULL)
	{	cell = Get_Random_element(List);
		adj = Get_unvisited_adjacents(Maze,cell);
		if(adj!=NULL)
		{	newcell = Get_Random_element(adj);
			Dig_to(Maze,cell,newcell);
			Rooms_Left--;
			if(List_Size(adj)==1)
				List = Remove_Loc(List, cell);
			List = Insert_Location(List, newcell);
			Free_List(adj);
		}
		else
			List = Remove_Loc(List, cell);
		if(win && !skip)
		{	delay=Haste_Slow(win, delay, &skip);
			ex_delay=Update_delay(delay, ex_delay);
			while(!refresh_time(delay));
			Display_Maze(win, Maze, 1);
		}
	}
	if(Rooms_Left==0)
		Free_List(List);
	delay=14;
	ex_delay=0;
	skip=0;
	return Maze;
}

/* Apre il percorso tra le due Locazioni */
void Dig_to(MAZE Maze, Location da, Location a)
{	if(da->riga == a->riga)
	{	if(da->colonna<a->colonna)
		{	if(da->colonna==0 && a->colonna==Maze->columns-2)
				Maze->map[da->riga][(a->colonna+1)%Maze->columns]=WAY;
			else
				Maze->map[da->riga][(da->colonna+1)%Maze->columns]=WAY;
		}
		else
		{	if(a->colonna==0 && da->colonna==Maze->columns-2)
				Maze->map[da->riga][(da->colonna+1)%Maze->columns]=WAY;
			else
				Maze->map[da->riga][(da->colonna-1)%Maze->columns]=WAY;
		}
	}
	else
	{	if(da->riga<a->riga)
		{	if(da->riga==0 && a->riga==Maze->rows-2)
				Maze->map[(a->riga+1)%Maze->rows][da->colonna]=WAY;
			else
				Maze->map[(da->riga+1)%Maze->rows][da->colonna]=WAY;
		}
		else
		{	if(a->riga==0 && da->riga==Maze->rows-2)
				Maze->map[(da->riga+1)%Maze->rows][da->colonna]=WAY;
			else
				Maze->map[(da->riga-1)%Maze->rows][da->colonna]=WAY;
		}
	}
	//Maze->map[da->riga][da->colonna]=WAY;
	Maze->map[a->riga][a->colonna]=WAY;
}

/* Restituisce una Locazione casuale che abbia entrambi gli indici pari */
Location Random_cell_even(MAZE M)
{	Location L = New_Loc(0,0);	//need even-even
	L->riga = 2*(rand()%(M->rows/2));
	L->colonna = 2*(rand()%(M->columns/2));
	return L;
}

/* Restituisce una Locazione casuale */
Location Random_cell(MAZE M)
{	Location L = New_Loc(0,0);
	do{	L->riga = rand()%(M->rows);
		L->colonna = rand()%(M->columns);
	}while(M->map[L->riga][L->colonna]==WALL);
	return L;
}

/* Restituisce una Locazione casuale che abbia una distanza minima maggiore di una percentuale
	della massima distanza toroidale sul labirinto.. ad ogni iterazione fallita la percentuale 
	scende per ridurre la probabilità loop.
	Legge debole dei grandi numeri */
Location Random_Distanced_Location(MAZE Maze, Location Distant_From, int percentual)
{	Location L=NULL;
	//% of longest thoroidal distance
	float min_dist=0;
	if(Distant_From)
		min_dist = (sqrt(pow2(Maze->rows/2)+pow2(Maze->columns/2))*percentual)/100;
	if(min_dist<1)
		min_dist=(int_min(Maze->rows, Maze->columns)+1)/2;
	do{	if(L)
		{	free(L);
			if(min_dist>0.5)
				min_dist-=0.5;
		}
		L = Random_cell(Maze);
	}while(Distant_From && Thoroid_Euclidean(Maze, L, Distant_From)<min_dist);
	return L;
}

/* Restituisce una lista contenente gli adiacenti non ancora visitati */
 Adj_List Get_unvisited_adjacents(MAZE Maze, Location L)
{	Adj_List ret=NULL;
	Location A = New_Loc((L->riga-2)%Maze->rows, L->colonna);	//UP
	if(L->riga-2<0)
		A->riga = L->riga+Maze->rows-2;
	if(!is_Visited(Maze, A))
		ret = Insert_Location(ret, A);
	A->riga = L->riga;	A->colonna = (L->colonna+2)%Maze->columns; 	//RIGHT
	if(!is_Visited(Maze, A))
		ret = Insert_Location(ret, A);
	A->riga = (L->riga+2)%Maze->rows;	A->colonna = L->colonna;	//DOWN
	if(!is_Visited(Maze, A))
		ret = Insert_Location(ret, A);
	A->riga = L->riga;	A->colonna = (L->colonna-2)%Maze->columns;	//LEFT
	if(L->colonna-2<0)
		A->colonna = L->colonna+Maze->columns-2;
	if(!is_Visited(Maze, A))
		ret = Insert_Location(ret, A);
	return ret;
}

/* Si definisce visitata una Locazione se ha almeno un adiacente WAY */
int is_Visited(MAZE Maze, Location L)
{	if(Maze->map[(L->riga+Maze->rows-1)%Maze->rows][L->colonna]==WAY || Maze->map[L->riga][(L->colonna+1)%Maze->columns]==WAY || Maze->map[(L->riga+1)%Maze->rows][L->colonna]==WAY || Maze->map[L->riga][(L->colonna+Maze->columns-1)%Maze->columns]==WAY )
		return 1;
	else
		return 0;
}

//Reverse Delete
/* Creazione casuale tramite Reverse Delete */
MAZE Reverse_Delete(unsigned int rows, unsigned int cols, WINDOW *win)
{	MAZE Maze = Alloc_Maze(2*rows, 2*cols, WAY);
	int i;
	for(i=0; i<Maze->columns; i++)
		Maze->map[Maze->rows-1][i]=WALL;
	for(i=0; i<Maze->rows; i++)
		Maze->map[i][Maze->columns-1]=WALL;
	
	Divide(Maze, 0, 0, Maze->rows-1, Maze->columns-1, win);
	int h_wall = Random_odd(Maze->rows);
	int v_wall = Random_odd(Maze->columns);
	if(h_wall!=Maze->rows-1 && v_wall!=Maze->columns-1)
		Ricentra(Maze, h_wall, v_wall);
	
	if(win && !skip)
	{	delay=Haste_Slow(win, delay, &skip);
		ex_delay=Update_delay(delay, ex_delay);
		while(!refresh_time(delay));
		Display_Maze(win, Maze, 1);
	}
	delay=14;
	ex_delay=0;
	skip=0;
	return Maze;
}

/* Ricentra il Labirinto */
void Ricentra(MAZE Maze, int new_r, int new_c)
{	char **new = malloc(sizeof(char*)*Maze->rows);
	int i, j;
	for (i=0; i<Maze->rows; i++)
		new[i] = malloc(sizeof(char)*Maze->columns);
	for(i=0; i<Maze->rows; i++)
		for(j=0; j<Maze->columns; j++)
			new[i][j]=Maze->map[(i+Maze->rows-new_r-1)%Maze->rows][(j+Maze->columns-new_c-1)%Maze->columns];
	for (i=0; i<Maze->rows; i++)
		free(Maze->map[i]);
	free(Maze->map);
	Maze->map=new;
}

/* Divisione ricorsiva */
void Divide(MAZE Maze, int r, int c, int R, int C, WINDOW *win)
{	if (R > 1 && C > 1)
	{	if (R > C)
			Divide_horizontally(Maze,r,c,R,C, win);
		else
		{	if(R==C)
			{	if(H_V==0)
				{	Divide_horizontally(Maze,r,c,R,C, win);
					H_V++;
				}
				else
				{	Divide_vertically(Maze,r,c,R,C, win);
					H_V--;
				}
			}
			else
				Divide_vertically(Maze,r,c,R,C, win);
		}
	}
	//H_V=0;
}

/* Divisione orizzontale */
void Divide_horizontally(MAZE Maze, int r, int c, int R, int C, WINDOW *win)
{	int h_wall = Random_odd(R);
	int door = Random_even(C);
	int j;
	for (j=0; j<C; j++)
	{	if (j!=door) 
			Maze->map[r+h_wall][c+j] = WALL;
	}
	if(win && !skip)
	{	delay=Haste_Slow(win, delay, &skip);
		ex_delay=Update_delay(delay, ex_delay);
		while(!refresh_time(delay));
		Display_Maze(win, Maze, 1);
	}
	Divide(Maze, r, c, h_wall, C, win);
	Divide(Maze, r+h_wall+1, c, (R-h_wall)-1, C, win);
}

/* Divisione verticale */
void Divide_vertically(MAZE Maze, int r, int c, int R, int C, WINDOW *win)
{	int v_wall = Random_odd(C);
	int door = Random_even(R);
	int i;
	for (i=0; i<R; i++)
	{	if(i != door)
			Maze->map[r+i][c+v_wall] = WALL;
	}
	if(win && !skip)
	{	delay=Haste_Slow(win, delay, &skip);
		ex_delay=Update_delay(delay, ex_delay);
		while(!refresh_time(delay));
		Display_Maze(win, Maze, 1);
	}
	Divide(Maze, r, c, R, v_wall, win);
	Divide(Maze, r, c+v_wall+1, R, (C-v_wall)-1, win);
}

/* Restituisce un dispari casuale minore di bound */
int Random_odd(int bound)
{	int val = 2*(rand()%(bound/2))+1;
	return val;
}

/* Restituisce un pari casuale minore di bound */
int Random_even(int bound)
{	int val = 2*(rand()%((bound+1)/2));
	return val;
}

/* Rimozione dei vicoli ciechi */
void Via_Gluck(MAZE Maze, int p, int num)	//p in [0-100] se num in [0-3] muri da tentare di aprire con probabilità p
{	if(p>-1 && p<101 && num>-1 && num<4)
	{	if(p!=0 && num!=0)
		{	int chance, left;
			Adj_List holes = Get_Dead_Ends(Maze), walls=NULL;
			Location wall=NULL;
			while(holes)
			{	walls = Get_Walls_Around(Maze, holes->locazione);
				left=0;
				if(List_Size(walls)<3)
					left=3-List_Size(walls);
				else
					left=0;
				if(p>0)
				{	for(; left<num; left++)
					{	wall = Get_Random_element(walls);
						chance = rand()%100;
						if(chance<p)
							Maze->map[wall->riga][wall->colonna]=WAY;
						walls=Remove_Loc(walls, wall);
					}
				}
				holes = Next(holes);
			}
		}
	}
}
