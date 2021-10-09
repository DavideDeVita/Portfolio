#include "code.h"
#include <time.h>
#include <curses.h>
									//	Davide De Vita
									//	N 86/1908
#define WALL 'W'
#define WAY ' '

//Display
#define SINGLE_LINE 1
#define DOUBLE_LINE 2
#define BOXES 0
#define DOUBLE_BOXES 3
#define ALL -1
//Euristiche
#define MANHATTAN 0
#define EUCLIDEAN 1
#define T_MANHATTAN 2
#define T_EUCLIDEAN 3
//Array Colore
#define Bianco 1
#define Grigio 0
#define Nero -1
//Array Pred			convertire in Location
#define NO_PATH -1
#define START_V -2
#define CLOSED -3		//A*


struct maze
{	char **map;
	unsigned int rows;
	unsigned int columns;
};

typedef struct maze* MAZE;

typedef struct list_of_adjs
{	Location locazione;
	struct list_of_adjs *next;
} *Adj_List;

typedef struct Maze_visit_info
{	int **pred;
	int **dist;
	unsigned int rows;
	unsigned int columns;
	unsigned int steps;
} Map_info;

MAZE Alloc_Maze(unsigned int, unsigned int, char);
MAZE Make_Grid(unsigned int, unsigned int, int);
int Ways_Cells(MAZE);
void Free_Maze(MAZE);
Map_info BFS(MAZE, Location, Location, WINDOW*);
Map_info A_Star(MAZE, Location, Location, int, WINDOW*);
Map_info A_Star_G(MAZE, Location, Location, int, WINDOW*);
int Thoroid_Manhattan(MAZE, Location, Location);
int Manhattan(Location, Location);
int int_min(int, int);
float float_min(float, float);
int pow2(int);
float Thoroid_Euclidean(MAZE, Location, Location);
float Euclidean(Location, Location);
Adj_List Compute_Path(MAZE, Map_info, Location, Location);
Adj_List Make_Path(Map_info, Location, Location);
char *Unexisting_Location_path(MAZE, Location, Location);
int Pred(Map_info, Location);
//Random
MAZE Prim_Random_Walk(unsigned int, unsigned int, WINDOW *);
void Dig_to(MAZE, Location, Location);
Location Random_cell_even(MAZE);
Location Random_cell(MAZE);
Location Random_Distanced_Location(MAZE, Location, int);
Adj_List Get_unvisited_adjacents(MAZE, Location);
int is_Visited(MAZE, Location);
MAZE Reverse_Delete(unsigned int, unsigned int, WINDOW *);
void Divide(MAZE, int, int, int, int, WINDOW *);
void Divide_horizontally(MAZE, int, int, int, int, WINDOW *);
void Divide_vertically(MAZE, int, int, int, int, WINDOW *);
int Random_odd(int);
int Random_even(int);
void Ricentra(MAZE Maze, int, int);
void Via_Gluck(MAZE, int, int);
//Location
int Same_Location(Location, Location);
Location To_Location(Map_info, int);
int To_Number(Location, MAZE);
Adj_List Locazioni_adiacenti(MAZE, Location);
Adj_List Get_Walls_Around(MAZE, Location);
Adj_List Get_Dead_Ends(MAZE);
int Exists_Location(MAZE, Location);
void If_Free(Location);
//Liste
Adj_List Insert_adj_list(Adj_List, unsigned int, unsigned int);
Adj_List Insert_Location(Adj_List, Location);
Adj_List Next(Adj_List);
Adj_List Remove_Loc(Adj_List, Location);
int Is_in(Adj_List, Location);
int List_Size(Adj_List);
void Free_List(Adj_List);
Location Get_Random_element(Adj_List);
Adj_List Revert_List(Adj_List);
//Display
int refresh_time(int);
int Haste_Slow(WINDOW *, int, int*);
void show_message(int , int , char*, int);
void Display_Maze(WINDOW *, MAZE, int);
int window_row(int);
int window_column(int);
void Display_Cell(WINDOW *, MAZE, int, int);
void Display_Path(WINDOW *, MAZE, Adj_List, int, int, int);
void Move_Path_Walker(WINDOW *, MAZE, Adj_List);
int Update_delay(int, int);
void Error_Message(char *);
//Parse
void Save_Maze(char *, MAZE);
MAZE Load_Maze(char *);
MAZE Read_Parameters(FILE*);
void Step_back(FILE *);
void Ignore_spaces(FILE *);
int is_digit(char);
int Read_num(FILE *);
int ctoi(char);
void Invalid_format(char *, MAZE);
