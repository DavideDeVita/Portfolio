#include "main_lib.h"
									//	Davide De Vita
									//	N 86/1908
/* Seleziona l'elemento di indice 'quale' da List (il primo ha indice 1) */
List_Maze Select (List_Maze List, int quale)
{
int i=1;
List_Maze G;

	G=List;
	while(i<quale)
	{	G = G->link;
		i++;
	}
return G;
}

/* Cancella il selezionato da List */
List_Maze Cancella_da_Lista_Labirinto(List_Maze List, int select)
{	if(select>0)
	{	if(select==1)
			List = List->link;
		else
			Cancella_da_Lista_in_coda(List->link, List, select-1);
	}
return List;
}

/* Subroutine di Cancella ricorsiva in coda */
void Cancella_da_Lista_in_coda(List_Maze List, List_Maze P, int select)
{
	if(select == 1)
		P->link=List->link;
	else
		Cancella_da_Lista_in_coda(List->link, List, select - 1);
}

/* Distrugge e libera la memoria occupata da List */
void Free_List_Maze(List_Maze List)
{	List_Maze erase=List;
	while(erase)
	{	List=List->link;
		Free_Maze(erase->Maze);
		free(erase);
		erase=List;
	}
}

/* Inserisce alla fine di List */
List_Maze Inserimento_fine_Lista(List_Maze List, List_Maze Labirinto)
{
	if(List!=NULL)
		List->link = Inserimento_fine_Lista(List->link, Labirinto);
	else
		List=Labirinto;
return List;
}

/* Inserisce in testa a List */
List_Maze Inserimento_testa_Lista(List_Maze List, List_Maze Labirinto)
{	Labirinto->link=List;
	return Labirinto;
}

/* Inizializza un nuovo elemento per la Lista */
List_Maze Init()
{
List_Maze List;
	List=(List_Maze)malloc(sizeof(struct list));
	List->link = NULL;
	List->Maze = NULL;
return List;
}

/* Realloc e Calloc unite per una stringa */
char *ReCalloc(char*ptr, int old_size, int new_size)
{	ptr = realloc(ptr, new_size*sizeof(char));
	for(; old_size<new_size; old_size++)
		ptr[old_size]='\0';
	return ptr;
}
