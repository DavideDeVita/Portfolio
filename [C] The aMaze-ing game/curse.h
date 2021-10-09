#include "main_lib.h"
										// Davide De Vita
										// N86001908
#define ESC 27
#define ENTER 10
#define DEL 263

#define N_OPTIONS 10
#define N_GAMES 3
#define SIGHT_GROW_LIMIT 500

#define UNDO -1
#define CTRL_HOME 535
#define CTRL_END 530
#define CTRL_UP 566
#define CTRL_RIGHT 560
#define CTRL_DOWN 525
#define CTRL_LEFT 545
#define ALT_UP 564
#define ALT_RIGHT 558
#define ALT_DOWN 523
#define ALT_LEFT 543
#define MAIUSC_UP 337
#define MAIUSC_RIGHT 402
#define MAIUSC_DOWN 336
#define MAIUSC_LEFT 393
#define CTRL_MAIUSC_UP 567
#define CTRL_MAIUSC_RIGHT 561
#define CTRL_MAIUSC_DOWN 526
#define CTRL_MAIUSC_LEFT 546

#define MAIN_LIMIT 45

typedef struct character
{	int x;
	int y;
	int last_dir;
	int alerted;
	Location Last;
} CHARACTER;

typedef struct character_list
{	CHARACTER Ch;
	struct character_list *next;
} *CHARACTER_LIST;

/*Curses Functions*/
int init_curses();
void close_curses();
WINDOW *create_boxed_newwin(int, int, int, int);
/* Logic */
void Print_Menu(WINDOW*, int);
void The_Choice(WINDOW *);
void Open_Game_Menu(WINDOW *);
void Print_Game_Menu(WINDOW*, int);
void Which_Game(int);
void He_Choosed(WINDOW *, int);
int Many_Mazes();
/* Ask */
void Ask_Parameters_Random();
void Ask_Parameters_New();
void Ask_for_File(List_Maze);
void Edit_Map();
Location Ask_for_Location(MAZE, Location, int);
void Ask_for_BFS();
void Ask_for_A_Star();
void Ask_for_Std_Game();
void Ask_for_Blind_Game();
void Ask_for_Guard_Game();
/* Other Logic */
WINDOW *Make_Maze_Win();
void Update_limits(List_Maze);
void May_Update_limits(int, int);
void Show_n_Wait(WINDOW *, MAZE, Location, Location);
/* Engine */
List_Maze Inserimento_Lista(List_Maze, List_Maze);
void Testa_o_Coda();
int Ask_Integer(int, int , char*);
int Ask_Integer_Between(int, int, char*, int, int);
int Ask_Difficolta(int, int, int);
int Is_An_Option(int);
void Smile(int, int, int);
void Kid_on_me(int);
void Sentence_Quote(const char*);
int Ask_Boolean(int, int, char*, char*, char*);
char* Ask_for_string(int, int, char *);
	/* Colors */
short choose_color(int, int, char *, char*, int);
void Path_n_Visited_color(short, short);
	/**/
Location Select_Location(WINDOW *, MAZE, Location, int);
int redirect(int);
void Print_Path(WINDOW *, MAZE, Map_info, Adj_List, Location, Location);
/* GAME */
	int Std_Game(MAZE, Location, Location);
	void Std_Move_Character(WINDOW *, int, int, int, int, int);

	int Blind_Game(MAZE, Location, Location, int);
	Adj_List Create_Torches(MAZE, int, Location);
	void What_can_I_see(WINDOW *, MAZE, int, int, Location, int, int, Adj_List, Adj_List);
	void Make_Light_Around(WINDOW *, MAZE, int, int, int, Adj_List);
	void Simmetric_rows(WINDOW *, MAZE, int, int, int, int, int, Adj_List);
	void Simmetric_columns(WINDOW *, MAZE, int, int, int, Adj_List);
	void Display_Cell_Torch(WINDOW *, MAZE, int, int, Adj_List);
	
	int Guard_Mode(MAZE, Location, int);
	void Display_Map(WINDOW *, MAZE, int, int, int **, int, CHARACTER_LIST, int, int);
	void Where_am_I(WINDOW *, int, int, int, int);
	void Am_I_in_Troubles(WINDOW *, MAZE, int, int, CHARACTER_LIST, int, int, int**, int, CHARACTER_LIST, int);
	int Busted(CHARACTER_LIST, int, int, int *, int *);
	int In_Sight(WINDOW *, MAZE, int, int, int **, int, int, int, int, CHARACTER_LIST, CHARACTER_LIST);
	int Frontal_Sight(WINDOW *, MAZE, int **, int, int, int, int, int, int, CHARACTER_LIST, CHARACTER_LIST);
	int Sight_simmetric_rows(WINDOW *, MAZE, int, int, int **, int, int, int, int, int, CHARACTER_LIST, CHARACTER_LIST);
	int Sight_simmetric_columns(WINDOW *, MAZE, int, int, int, int **, int, int, CHARACTER_LIST, CHARACTER_LIST);
	int Eye_of_Guard(WINDOW *, MAZE, int, int, int **, int, int, CHARACTER_LIST, CHARACTER_LIST);
	int** Start_Map(MAZE);
	CHARACTER_LIST Make_Guards(MAZE, int, Location, int);
	int Is_in_CHL(CHARACTER_LIST, int, int);
	CHARACTER_LIST Insert_CH_Location(CHARACTER_LIST List, Location);
	int **Spread_in_Maze(int, int, MAZE, Location);
	void Spread(int**, MAZE, int, int);
	void La_ronda(MAZE, int, CHARACTER_LIST);
	void Update_Guard_Position(CHARACTER_LIST, MAZE, Location, Location);
	void Print_Guard(WINDOW *, int, int, int);
	CHARACTER_LIST Fire_Drill(CHARACTER_LIST, MAZE, int, int, int);
	int Is_in_CHL(CHARACTER_LIST, int, int);
	CHARACTER_LIST Insert_CH_Location(CHARACTER_LIST, Location);
	CHARACTER_LIST Does_Bul_Hit(MAZE, int **, CHARACTER_LIST *, int, int, CHARACTER_LIST, int *, int *, int *, int *);
	CHARACTER_LIST Remove_Ch(CHARACTER_LIST, int, int);
	void Free_CHL(CHARACTER_LIST);
	CHARACTER_LIST Next_CH(CHARACTER_LIST);
	CHARACTER_LIST Revert_List_CH(CHARACTER_LIST);
	CHARACTER_LIST Bul_Movement(MAZE, CHARACTER_LIST);
	void Single_Bul_Movement(MAZE, CHARACTER_LIST, CHARACTER_LIST);
	int Bullet_Cross(CHARACTER_LIST, MAZE, CHARACTER_LIST );
	void Display_Bullets(WINDOW *, CHARACTER_LIST);
	int Guard_Game_redirect(int);
	int Is_Fire_Key(int);
	int Fire_direction(int, int);
	void Update_Message(int, int, int, int, int, int);
	void Guard_Rules(int, int *, int *, int *, int , int *, int *, int *, int *, int *);
	void GG_Settings(int, int *, int *, int *, int, int *, int *, int *, int *, int *);
