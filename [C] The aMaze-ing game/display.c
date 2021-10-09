#include"maze.h"
										// Davide De Vita
										// N86001908
#define SPEED_MODIFIER 2
#define MAX_SPEED 4
#define ERROR_TIME 100

int delta=0, last=0;

/* Ritorna 1 se è passato almeno 'delay' dall' ultimo controllo, 0 altrimenti */
int refresh_time(int refresh_delay)
{	clock_t now;
	now = clock(); /* Number of clock ticks passed since the beginning of the program */
	delta += now - last; /* update the number of clock ticks passed since last check */
	last = now; /* Record the current ticks count */

	if ((100*delta)/CLOCKS_PER_SEC > refresh_delay)  /* check if at least refresh_delay centiseconds have passed */
	{	delta=0;
		return 1;
	}
	else
		return 0;
}

/* Gestisce la modifica di velocità di visualizzazione */
int Haste_Slow(WINDOW *win, int delay, int *skip)
{	int key;
	if(!*skip)
	{	key=wgetch(win);
		if ( (key==KEY_RIGHT || key=='d' || key=='D') && delay>SPEED_MODIFIER+MAX_SPEED)
			return delay-SPEED_MODIFIER;
		else if( (key==KEY_LEFT || key=='a' || key=='A') && delay< 100-SPEED_MODIFIER)
			return delay+SPEED_MODIFIER;
		else if(key==' ')	//Make it skip
			*skip=1;
	}
	return delay;
}

/* Stampa di un messaggio (necessita di refresh() */
void show_message(int x, int y, char* str, int pair)
{	move(x,y); /* Move cursor to position (x,y) on the screen */
	attron(COLOR_PAIR(pair)); /* Set the current color to pair (see init_pair) */
	addstr(str);	/*adds the string str*/
}

/* Stampa la cella del labirinto */
void Display_Cell(WINDOW *win, MAZE Maze, int x, int y)
{	wmove(win, window_row(x), window_column(y));
	if(Maze->map[x][y]==WAY)	//Set the way or wall color
		wattron(win, COLOR_PAIR(4));
	else
		wattron(win, COLOR_PAIR(5));
	waddch(win, ' ');
	waddch(win, ' ');
}

/* Stampa il Labirinto */
void Display_Maze(WINDOW *win, MAZE Maze, int shall_refresh)
{	wattron(win, COLOR_PAIR(1)); //correct dislpay of box
	wclear(win);
	box(win, 0, 0);
	wrefresh(win);
	wmove(win, 1, 2);
	int i, j;
	for(i=0; i<Maze->rows; i++)
	{	for(j=0; j<Maze->columns; j++)
		{	Display_Cell(win, Maze, i, j);
		}
		wmove(win, i+2, 2);
	}
	if(shall_refresh)
	wrefresh(win);
}

/* Impostazione personale */
int window_row(int x)
{	return x+1;	//Personal setting of Maze Window
}

/* Impostazione personale */
int window_column(int y)
{	return (2*y)+2;	//Personal setting of Maze Window
}

/* Visualizzazione del percorso */
void Display_Path(WINDOW *Maze_Win, MAZE Maze, Adj_List Path, int x, int y, int delay)
{	wattron(Maze_Win, COLOR_PAIR(6));
	Location Ex=NULL;
	int ch, skip=0;
	show_message(x+1, y, "Premere SPAZIO per andare avanti veloce", 1);
	refresh();
	show_message(x, y, "Premere INVIO o ESC per tornare al Menù", 1);
	while(Path)
	{	ch = wgetch(Maze_Win);
		if(ch==' ')
			skip=1;
		if(skip || refresh_time(delay))	//if !skip the wait refresh time
		{	Move_Path_Walker(Maze_Win, Maze, Path);	//Display new location of walker
			if(Ex)	//clear the last location
			{	wmove(Maze_Win, window_row(Ex->riga), window_column(Ex->colonna));
				waddch(Maze_Win, ' ');
				waddch(Maze_Win, ' ');
				free(Ex);
				Ex=NULL;
			}
			Ex=New_Loc(Path->locazione->riga, Path->locazione->colonna);
			Path = Next(Path);
		}
		wrefresh(Maze_Win);
	}
	If_Free(Ex);
	int i;
	do{	i=getch();	//Wait for Enter or Esc
	}while(i!=27 && i!='\n');	
}

/* Visualizzazione di un personaggio che segue il percorso */
void Move_Path_Walker(WINDOW *win, MAZE Maze, Adj_List Path)
{	wmove(win, window_row(Path->locazione->riga), window_column(Path->locazione->colonna));
	/* Direction face */
	if(Path->next)
	{	if(Path->next->locazione->riga==(Path->locazione->riga+Maze->rows-1)%Maze->rows) //UP
		{	waddch(win, '/');
			waddch(win, '\\');
		}
		else if(Path->next->locazione->colonna==(Path->locazione->colonna+Maze->columns-1)%Maze->columns)	//LEFT
		{	waddch(win, '<');
			waddch(win, '-');
		}
		else if(Path->next->locazione->riga==(Path->locazione->riga+1)%Maze->rows) //DOWN
		{	waddch(win, '\\');
			waddch(win, '/');
		}
		else if(Path->next->locazione->colonna==(Path->locazione->colonna+1)%Maze->columns)	//RIGHT
		{	waddch(win, '-');
			waddch(win, '>');
		}
	}
	else
	{	waddch(win, 'o');
		waddch(win, '0');
	}
}

/* Aggiorna il messaggio sul Delay */
int Update_delay(int delay, int ex_delay)
{	if(delay!=ex_delay)
	{	char msg[25]={'\0'};
		sprintf(msg, "Velocità: %d centisecondi", ex_delay);
		show_message(3, 50, msg, 2);
		refresh();
		sprintf(msg, "Velocità: %d centisecondi", delay);
		show_message(3, 50, msg, 1);
		refresh();
	}
	return delay;
}

/* Stampa un messaggio d'errore per qualche istante */
void Error_Message(char *error_msg)
{	show_message(16, 25, error_msg, 8);
	refresh();
	refresh_time(1);
	while(!refresh_time(ERROR_TIME));
	show_message(16, 25, error_msg, 2);
	refresh();
	
}
