#include "maze.h"
									//	Davide De Vita
									//	N 86/1908
typedef struct list *List_Maze;

struct list
{	MAZE Maze;
	List_Maze link;
};

List_Maze Select (List_Maze , int );
List_Maze Cancella_da_Lista_Labirinto(List_Maze , int);
void Cancella_da_Lista_in_coda(List_Maze, List_Maze , int );
void Free_List_Maze(List_Maze);
List_Maze Inserimento_fine_Lista(List_Maze , List_Maze );
List_Maze Inserimento_testa_Lista(List_Maze, List_Maze);
List_Maze Init();
char *ReCalloc(char*, int, int);
