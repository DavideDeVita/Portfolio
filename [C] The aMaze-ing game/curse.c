#include <stdlib.h>
#include <curses.h>
#include <string.h> 
#include <time.h> 
#include <unistd.h> 

#include "curse.h"
										// Davide De Vita
										// N86001908
/*Global*/
List_Maze List=NULL, nodo_lista=NULL;
int num=0, ASG=0;

/* Inizializzazione */
int init_curses(void) {
	/* Init screen & keyboard input */
	initscr();	/* Start the curses mode */
	noecho();    /* Do not echo pressed key on the screen */
	clear();    /* Clear the screen */  
	keypad(stdscr, TRUE); /* Allow control keys to be read (i.e., function and arrow keys) */
	curs_set(0);  /* Hide cursor */
	cbreak();   /* Line buffering disabled, pass on everything */
 	if(!has_colors()) {	
		endwin();
		printf("Your terminal does not support colors\n");
		return 0;
	}
	
	start_color();
	/* Set different color pairs <foreground,background> for the text printed on
		 the screen */
	init_pair(1, COLOR_WHITE, COLOR_BLACK); /* Writings */
	init_pair(2, COLOR_BLACK, COLOR_BLACK); /* Delete */
	init_pair(3, COLOR_BLACK, COLOR_WHITE);   /* Evidence */
	short wall, way;
	wall=choose_color(2, 5, "Scegliere colore per le mura", "Mura di colore ", -1);
	if(wall==UNDO)
		return 0;
	way=choose_color(13, 5, "Scegliere colore per i corridoi", "Corridoi di colore ",(int)wall);
	if(way==UNDO)
		return 0;
	init_pair(4, way, way);   /* Ways */
	init_pair(5, wall, wall);   /* Walls */
	Path_n_Visited_color(wall, way);	/* Path n Visit colors */
	init_pair(8, COLOR_RED, COLOR_BLACK);	/* Guards */
	init_pair(9, COLOR_BLUE, COLOR_YELLOW);	/* Quote*/
	clear();
	refresh(); /* Show everything on the screen*/
	return 1;
}

/* Ripristina i valori precedenti */
void close_curses() {
	attroff(COLOR_PAIR(1)); /* Restore screen colors */
	standend();
	nocbreak();
	endwin();   /* Close the window */
}

/* Crea una WINDOW che conterrà il Menù */
WINDOW *create_boxed_newwin(int height, int width, int starty, int startx) {
	/* Create a boxed windows with "height" lines, "width" coulumns. The upper
		 left-hand corner of the window is at line starty and column startx, with
		 respect to the current window */
	WINDOW *local_win;
	/* Create a new window inside the bounding box */
	local_win = newwin(height, width, starty, startx); 
	wrefresh(local_win);		 /* Show that box */
	nodelay(local_win,TRUE); /* Keyboard reads will be non-blocking in window */
	keypad(local_win, TRUE); /* Allow control keys to be read */
	
	return local_win;
}

							/* Logic */
/* Stampa il Menù evidenziando l'opzione selezionata */
void Print_Menu(WINDOW* win, int selected)
{	wclear(win);
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(1));
	char str[40]="\0";
	sprintf(str, "La lista conta attualmente %d labirint%c",num, (num!=1) ? 'i' : 'o');
	mvwaddstr(win, 1, 1, str);
	mvwaddstr(win, 2, 1,"ESC Per uscire, INVIO per decidere");
	int i, to;
	to = (num>0) ? N_OPTIONS : 4;
	char *Options[N_OPTIONS] = {"Genera Labirinto Random", "Creazione Manuale Labirinto", "Leggi da File",
		 "Modifica Colori", "Cancella Labirinto", "Modifica Labirinto",
		  "Salva su File", "Perocrso Minimo tra due punti (BFS)", "Perocrso Minimo tra due punti (A*)", "Giochi"};
	for(i=0; i<to; i++)
	{	if(i == selected)	// Evididence current selection
			wattron(win, COLOR_PAIR(3));
		mvwaddstr(win, i+3, 2, Options[i]);
		if(i==selected)
			wattron(win, COLOR_PAIR(1));
	}
	wrefresh(win);
}

/* Gestisce la scelta dell'opzione desiderata del Menù */
void The_Choice(WINDOW *win)
{	int selected = 0, max;
	int key=-3;
	do{	if(key!=-1)
		{	max = (num>0) ? N_OPTIONS : 4;
			if(selected>=max)
				selected=0;
			Print_Menu(win, selected);	// Display Menu options
		}
		key = wgetch(win);
		switch (key)
		{	case KEY_UP :
				if(selected>0)
					selected--;
				else
					selected = max-1;
				break;
			case KEY_DOWN :
				selected = (selected+1)%max;
				break;
			case ENTER :
				He_Choosed(win, selected);	//WIN passato per Game_Menu
				break;
			case KEY_HOME:
				selected = 0;
				break;
			case KEY_END:
				selected = max-1;
				break;
			case KEY_NPAGE:
				if(num>0)
					He_Choosed(win, 9);
				break;
			case 6: /* Ctrl f */	//Analisi ASG
				ASG=1-ASG;
			case 4: /* Ctrl d */	//Inverti ordine di inserimento
				Testa_o_Coda();
		}
	}while(key!=ESC);
}

/* Gestisce il comportamento in funzione della scelta dell' utente */
void He_Choosed(WINDOW *win, int selected)
{	int which_one=1;
	short wall, way;
	if(selected>=4 && selected!=9)
	{	if(num>1)
			which_one=Many_Mazes();
		nodo_lista = Select(List, which_one); //Extract the selected Maze
	}
	if(which_one!=UNDO)
	{	switch (selected)
		{	case 0 :
				Ask_Parameters_Random();
				break;
			case 1 :
				Ask_Parameters_New();
				break;
			case 2 :
				Ask_for_File(NULL);
				break;
			case 3 :
				wall=choose_color(2, MAIN_LIMIT+5, "Scegliere nuovo colore per le mura", "Mura di colore ", -1);
				if(wall==UNDO)
					break;
				way=choose_color(13, MAIN_LIMIT+5, "Scegliere nuovo colore per i corridoi", "Corridoi di colore ",(int)wall);
				if(way==UNDO)
					break;
				init_pair(4, way, way);   /* WAYS */
				init_pair(5, wall, wall);   /* WALLS */
				Path_n_Visited_color(wall, way);
				break;
			case 4 :
				List = Cancella_da_Lista_Labirinto(List, which_one);
				Free_Maze(nodo_lista->Maze);
				free(nodo_lista);
				nodo_lista = NULL;
				num--;
				Update_limits(List);
				break;
			case 5 :
				Edit_Map();
				break;
			case 6 :
				Ask_for_File(nodo_lista);
				break;
			case 7 :
				Ask_for_BFS();
				break;
			case 8 :
				Ask_for_A_Star();
				break;
			case 9 :
				Open_Game_Menu(win);
				break;
		}
		clear();
		refresh();
	}
}

/* Gestisce la scelta fra i labirinti */
int Many_Mazes()
{	WINDOW *Show_Mazes=NULL;
	int i, old_i, key;
	char str[3] ="\0";
	show_message(1, MAIN_LIMIT+4, "1", 3);
	for(i=1; i<num; i++)
	{	sprintf(str, "%d", i+1);
		show_message(1, MAIN_LIMIT+4+(3*i), str, 1);
	}
	refresh();
	Show_Mazes = Make_Maze_Win();
	i=0, old_i =0;
	do{	nodo_lista = Select(List, i+1);
		Display_Maze(Show_Mazes, nodo_lista->Maze, 1);
		wrefresh(Show_Mazes);
		key = getch();
		switch (key)
		{	case KEY_LEFT :
				i= (i-1+num)%num;
				break;
			case KEY_RIGHT :
				i = (i+1)%num;
				break;
			case ESC :
				i=UNDO;
				break;
			case KEY_HOME:
				i = 0;
				break;
			case KEY_END:
				i = num-1;
				break;
		}
		if(i!=UNDO && i!=old_i)
		{	sprintf(str, "%d", old_i+1);
			show_message(1, MAIN_LIMIT+4+(3*old_i), str, 1);
			sprintf(str, "%d", i+1);
			show_message(1, MAIN_LIMIT+4+(3*i), str, 3);
			old_i=i;
			refresh();
		}	
	}while(key!=ESC && key!=ENTER);
	int j;
	/* Delete selection bar*/
	for(j=0; j<num; j++)
		show_message(1, MAIN_LIMIT+4+(3*j), "  ", 2);
	refresh();
	wclear(Show_Mazes);
	wrefresh(Show_Mazes);
	delwin(Show_Mazes);
	//refresh();
	if(i!=UNDO)
		return i+1;
	else
		return UNDO;
}

/* Apre il menù secondario dedicato ai giochi */
void Open_Game_Menu(WINDOW *win)
{	int selected = 0, max=N_GAMES;
	int key=-3;
	do{	if(key!=-1)
			Print_Game_Menu(win, selected);
		key = wgetch(win);
		switch (key)
		{	case KEY_UP :
				if(selected>0)
					selected--;
				else
					selected = max-1;
				break;
			case KEY_DOWN :
				selected = (selected+1)%max;
				break;
			case ENTER :
				Which_Game(selected);
				break;
			case KEY_HOME:
				selected = 0;
				break;
			case KEY_END:
				selected = max-1;
				break;
			case KEY_PPAGE:
				key=ESC;
				break;
		}
	}while(key!=ESC && key!=ENTER);
}

/* Stampa le opzioni del menu dei giochi */
void Print_Game_Menu(WINDOW* win, int selected)
{	wclear(win);
	box(win, 0, 0);
	wattron(win, COLOR_PAIR(1));
	char str[40]="\0";
	sprintf(str, "La lista conta attualmente %d labirint%c",num, (num!=1) ? 'i' : 'o');
	mvwaddstr(win, 1, 1, str);
	mvwaddstr(win, 2, 1,"ESC Per uscire, INVIO per decidere");
	int i;
	char *Options[N_GAMES] = {"Standard Game", "Blind Mode", "Guard Game"};
	for(i=0; i<N_GAMES; i++)
	{	if(i == selected)
			wattron(win, COLOR_PAIR(3));
		mvwaddstr(win, i+3, 2, Options[i]);
		if(i==selected)
			wattron(win, COLOR_PAIR(1));
	}
	wrefresh(win);
}

/* Gestisce la scelta dell'opzione desiderata del Menù dei giochi */
void Which_Game(int selected)
{	int which_one=1;
	if(num>1)
		which_one=Many_Mazes();
	nodo_lista = Select(List, which_one);
	if(which_one!=UNDO)
	{	switch (selected)
		{	case 0 :
				Ask_for_Std_Game();
				break;
			case 1 :
				Ask_for_Blind_Game();
				break;
			case 2 :
				Ask_for_Guard_Game();
				break;
		}
		clear();
		refresh();
	}
}
							/* Ask */
/* Direttive per la creazione di un Labirinto random */
void Ask_Parameters_Random()
{	int rows, cols, algo, steps=0, p, n=0;
	do{	rows = Ask_Integer(17, 2, "Numero logico di righe: ");
		if(rows==UNDO)
			return;
		else if(rows==0)
			Error_Message("Selezionare un numero maggiore di 0");
	}while(rows==0);
	do{	cols = Ask_Integer(18, 2, "Numero logico di colonne: ");
		if(cols==UNDO)
			return;
		else if(cols==0)
			Error_Message("Selezionare un numero maggiore di 0");
	}while(cols==0);
	algo = Ask_Boolean(19, 2, "Usando Prim(1) o Reverse Delete(0)? ", "Reverse Delete", "Prim");
	if(algo==UNDO)
		return;
	steps = Ask_Boolean(20, 2, "Desideri vedere i passaggi (1-Si 0-No)? ", "No", "Si");
	if(steps==UNDO)
		return;
	p = Ask_Integer_Between(21, 2, "Probabilità di apertura vicoli ciechi (0-100)?", 0, 100);
	if(p==UNDO)
		return;
	if(p>0)
	{	n = Ask_Integer_Between(22, 2, "Muri abbattuti al più per ogni occorrenza: ", 0, 3);
		if(n==UNDO)
			return;
	}
	// Updates size of Maze Window if necessary
	May_Update_limits(2*rows, 2*cols);
	
	nodo_lista = Init();
	WINDOW *Maze_Win=NULL;
	if(steps)
	{	Maze_Win = Make_Maze_Win();
		show_message(1, MAIN_LIMIT+5, "Creando il labirinto . . .", 1);
		show_message(2, MAIN_LIMIT+5, "Usare le frecce per modificare la velocità", 1);
		show_message(4, 50, "SPAZIO per saltare alla fine", 1);
		refresh();
	}
	if(algo)
		nodo_lista->Maze = Prim_Random_Walk(rows, cols, Maze_Win);
	else
		nodo_lista->Maze = Reverse_Delete(rows, cols, Maze_Win);
	// Dead ends engine
	Via_Gluck(nodo_lista->Maze, p, n);
	show_message(1, MAIN_LIMIT+5, "Creando il labirinto . . .", 2);
	show_message(2, MAIN_LIMIT+5, "Usare le frecce per modificare la velocità", 2);
	if(nodo_lista->Maze!=NULL)
	{	List = Inserimento_Lista(List, nodo_lista);
		num++;
	}
	
	if(!steps)
		Maze_Win = Make_Maze_Win();
	Show_n_Wait(Maze_Win, nodo_lista->Maze, NULL, NULL);
}

/* Direttive per la creazione di un Labirinto */
void Ask_Parameters_New()
{	int rows, cols, fill, grid;
	do{	rows = Ask_Integer(17, 2, "Numero di righe: ");
		if(rows==UNDO)
			return;
		else if(rows==0)
			Error_Message("Selezionare un numero maggiore di 0");
	}while(rows==0);
	do{	cols = Ask_Integer(18, 2, "Numero di colonne: ");
		if(cols==UNDO)
			return;
		else if(cols==0)
			Error_Message("Selezionare un numero maggiore di 0");
	}while(cols==0);
	
	grid = Ask_Boolean(19, 2, "Si desidera una griglia? (1-si 0-no) ", "No", "Griglia");
	if(grid==UNDO)
		return;
	nodo_lista = Init();	
	if(!grid)
	{	fill = Ask_Boolean(20, 2, "Riempi con Muri(1) o Corridoi(0) ", "Corridoi", "Muri");
		if(fill==UNDO)
			return;
	
		if(fill)
			nodo_lista->Maze = Alloc_Maze(rows, cols, WALL);
		else
			nodo_lista->Maze = Alloc_Maze(rows, cols, WAY);
	}
	else
	{	fill = Ask_Boolean(20, 2, "Griglia aperta(0) o chiusa(1)?", "Aperta", "Chiusa");
		if(fill==UNDO)
			return;
		nodo_lista->Maze = Make_Grid(rows, cols, 1-fill);
	}
	
	if(nodo_lista->Maze!=NULL)
	{	List = Inserimento_Lista(List, nodo_lista);
		num++;
		May_Update_limits(nodo_lista->Maze->rows, nodo_lista->Maze->columns);
		Edit_Map();
		//Show_n_Wait(Maze_Win, nodo_lista->Maze, NULL, NULL);
	}
}

/* Direttive per la lettura o scrittura da/su file */
void Ask_for_File(List_Maze nodo)
{	char* filename = NULL;
	filename = Ask_for_string(17, 2, (nodo==NULL) ? "Da quale file? " : "Su quale file? ");
	if(!filename)
		return;
	if(nodo==NULL)	//Load
	{	nodo_lista=Init();
		nodo_lista->Maze = Load_Maze(filename);
		if(nodo_lista->Maze!=NULL)
		{	List = Inserimento_Lista(List, nodo_lista);
			num++;
			May_Update_limits(nodo_lista->Maze->rows, nodo_lista->Maze->columns);
			WINDOW *Maze_Win = Make_Maze_Win();
			Show_n_Wait(Maze_Win, nodo_lista->Maze, NULL, NULL);
		}
		else
		{	free(nodo_lista);
			nodo_lista=NULL;
		}
	}
	else			//Save
		Save_Maze(filename, nodo->Maze);
}

/* Modifica interattiva */
void Edit_Map()
{	Location L=NULL, last= New_Loc(0, 0);
	WINDOW *Maze_Win;
	Maze_Win = Make_Maze_Win();
	
	do{	Display_Maze(Maze_Win, nodo_lista->Maze, 1);
		L=Ask_for_Location(nodo_lista->Maze, last, 0);
		free(last);
		last=L;
		if(L)
		{	if(nodo_lista->Maze->map[L->riga][L->colonna]==WAY)
				nodo_lista->Maze->map[L->riga][L->colonna]=WALL;
			else
				nodo_lista->Maze->map[L->riga][L->colonna]=WAY;
		}
	}while(L);
	wclear(Maze_Win);
	wrefresh(Maze_Win);
	delwin(Maze_Win);
}

/* Direttive per BFS */
void Ask_for_BFS()
{	Location Source=NULL, Destination=NULL;
	int steps;
	Map_info info;
	Adj_List Path=NULL;
	WINDOW *Maze_Win;
	Maze_Win = Make_Maze_Win();
	box(Maze_Win, 0, 0);
	Source=Ask_for_Location(nodo_lista->Maze, NULL, 0);
	if(!Source)
		return;
	Destination=Ask_for_Location(nodo_lista->Maze, Source, 1);
	if(!Destination)
		return;
	steps = Ask_Boolean(20, 2, "Desideri vedere i passaggi (1-Si 0-No)? ", "No", "Si");
	if(steps==UNDO)
		return;
		
	if(steps)
	{	show_message(1, MAIN_LIMIT+5, "Esplorando il labirinto . . .", 1);
		show_message(2, MAIN_LIMIT+5, "Usare le frecce per modificare la velocità", 1);
		show_message(4, 50, "SPAZIO per saltare alla fine", 1);
		refresh();
		info = BFS(nodo_lista->Maze, Source, Destination, Maze_Win);
		show_message(1, MAIN_LIMIT+5, "Esplorando il labirinto . . .", 2);
		show_message(2, MAIN_LIMIT+5, "Usare le frecce per modificare la velocità", 2);
		show_message(4, 50, "SPAZIO per saltare alla fine", 2);
		refresh();
	}
	else
		info = BFS(nodo_lista->Maze, Source, Destination, NULL);
	// Compute n display the path
	Path=Compute_Path(nodo_lista->Maze, info, Source, Destination);
	Print_Path(Maze_Win, nodo_lista->Maze, info, Path, Source, Destination);
	free(Source);
	free(Destination);
}

/* Direttive per A Star */
void Ask_for_A_Star()
{	Location Source=NULL, Destination=NULL;
	Map_info info;
	Adj_List Path=NULL;
	int euristic, thoroid, steps;
	WINDOW *Maze_Win;
	Maze_Win = Make_Maze_Win();
	box(Maze_Win, 0, 0);
	euristic = Ask_Boolean(17, 2, "Euristica di Euclide o Manhattan? (1-Euclide 0-Manhattan) ", "Manhattan", "Euclide");
	if(euristic==UNDO)
		return;
	thoroid = Ask_Boolean(18, 2, "Euristica Toroidale o non? (1-Si 0-No) ", "No", "Toroidale");
	if(thoroid==UNDO)
		return;
	steps = Ask_Boolean(19, 2, "Desideri vedere i passaggi (1-Si 0-No)? ", "No", "Si");
	if(steps==UNDO)
		return;
	
	Source=Ask_for_Location(nodo_lista->Maze, NULL, 0);
	if(!Source)
		return;
	Destination=Ask_for_Location(nodo_lista->Maze, Source, 1);
	if(!Destination)
		return;
		
	if(steps)
	{	show_message(1, MAIN_LIMIT+5, "Esplorando il labirinto . . .", 1);
		show_message(2, MAIN_LIMIT+5, "Usare le frecce per modificare la velocità", 1);
		show_message(4, 50, "SPAZIO per saltare alla fine", 1);
		refresh();
		if(!ASG)
			info = A_Star(nodo_lista->Maze, Source, Destination, euristic+(2*thoroid), Maze_Win);
		else
			info = A_Star_G(nodo_lista->Maze, Source, Destination, euristic+(2*thoroid), Maze_Win);
		show_message(1, MAIN_LIMIT+5, "Esplorando il labirinto . . .", 2);
		show_message(2, MAIN_LIMIT+5, "Usare le frecce per modificare la velocità", 2);
		show_message(4, 50, "SPAZIO per saltare alla fine", 2);
		refresh();
	}
	else
		info = A_Star(nodo_lista->Maze, Source, Destination, euristic+(2*thoroid), NULL);
	
	Path=Compute_Path(nodo_lista->Maze, info, Source, Destination);
	Print_Path(Maze_Win, nodo_lista->Maze, info, Path, Source, Destination);
	free(Source);
	free(Destination);
}

/* Direttive per una partita Standard */
void Ask_for_Std_Game()
{	Location Source=NULL, Destination=NULL;
	WINDOW *Maze_Win;
	Maze_Win = Make_Maze_Win();
	box(Maze_Win, 0, 0);
	int random, repeat;
	if(Ways_Cells(nodo_lista->Maze)>=2)
	{	do{	random = Ask_Boolean(17, 2, "Chi sceglie? (0-Io 1-Tu) ", "Il giocatore", "Il gioco");
			if(random==UNDO)
				return;
			if(!random)
			{	Source=Ask_for_Location(nodo_lista->Maze, NULL, 0);
				if(!Source)
					return;
				Destination=Ask_for_Location(nodo_lista->Maze, Source, 1);
				if(!Destination)
					return;
			}
			else
			{	Source = Random_cell(nodo_lista->Maze);
				Destination = Random_Distanced_Location(nodo_lista->Maze, Source, 25+(10*rand()%6));
			}
			Std_Game(nodo_lista->Maze, Source, Destination);
			clear();
			repeat = Ask_Boolean(16, 2, "Rigiocare Standard Mode? (0-No 1-Si) ", "No, grazie", "Ancora! >_<");
			if(repeat==UNDO)
				return;
		}while(repeat);
		free(Source);
		free(Destination);
	}
	else
		Error_Message("Il Labirinto non ha abbastanza celle per questo gioco!");
}

/* Direttive per una partita Blind */
void Ask_for_Blind_Game()
{	Location Source=NULL, Destination=NULL;
	WINDOW *Maze_Win;
	int difficolta=1, random, repeat;
	Maze_Win = Make_Maze_Win();
	box(Maze_Win, 0, 0);
	if(Ways_Cells(nodo_lista->Maze)>=2)
	{	do{	random = Ask_Boolean(17, 2, "Chi sceglie? (0-Io 1-Tu) ", "Il giocatore", "Il gioco");
			if(random==UNDO)
				return;
			if(!random)
			{	Source=Ask_for_Location(nodo_lista->Maze, NULL, 0);
				if(!Source)
					return;
				Destination=Ask_for_Location(nodo_lista->Maze, Source, 1);
				if(!Destination)
				{	free(Source);
					return;
				}
			}
			else
			{	Source = Random_cell(nodo_lista->Maze);
				Destination = Random_Distanced_Location(nodo_lista->Maze, Source, 25+(10*rand()%6));
			}
			difficolta = Ask_Boolean(18, 2, "Con Torce? (1-Si 0-No) ", "Buio Pesto", "La luce aiuta");
			if(difficolta==UNDO)
			{	free(Source);
				free(Destination);
				return;
			}
			if(difficolta)
			{	difficolta = Ask_Difficolta(19, 2, difficolta);
				if(difficolta==UNDO)
				{	free(Source);
					free(Destination);
					return;
				}
			}
			Blind_Game(nodo_lista->Maze, Source, Destination, difficolta);
			clear();
			repeat = Ask_Boolean(16, 2, "Rigiocare Blind Mode? (0-No 1-Si) ", "No, grazie", "Ancora! =_=");
			if(repeat==UNDO)
				return;
		}while(repeat);
		free(Source);
		free(Destination);
	}
	else
		Error_Message("Il Labirinto non ha abbastanza celle per questo gioco!");
}

/* Direttive per una partita Guard */
void Ask_for_Guard_Game()
{	Location Source=NULL;
	WINDOW *Maze_Win;
	int difficolta=1, random, repeat, loss=0, game=0;
	Maze_Win = Make_Maze_Win();
	box(Maze_Win, 0, 0);
	if(Ways_Cells(nodo_lista->Maze)>=2)
	{	do{	random = Ask_Boolean(17, 2, "Chi sceglie? (0-Io 1-Tu) ", "Il giocatore", "Il gioco");
			if(random==UNDO)
				return;
			if(!random)
			{	Source=Ask_for_Location(nodo_lista->Maze, NULL, 0);
				if(!Source)
					return;
			}
			else
				Source = Random_cell(nodo_lista->Maze);
	
			difficolta = Ask_Difficolta(18, 2, difficolta);
			if(difficolta==UNDO)
			{	free(Source);
				return;
			}
			game=Guard_Mode(nodo_lista->Maze, Source, difficolta);
			clear();
			if(game==UNDO)
				Error_Message("Questo Labirinto non sembra adatto a questo gioco!");
			else
			{	if(game==0 || game==ESC)
					loss++;
				else
					loss=0;
				if(loss)	//Jokes
					Kid_on_me(loss);
				repeat = Ask_Boolean(16, 2, "Rigiocare Guard Mode? (0-No 1-Si) ", "No, grazie", "Ancora! ç_ç");
				if(repeat==UNDO)
					repeat=0;//free source before
			}
		}while(repeat==1);
		free(Source);
	}
	else
		Error_Message("Il Labirinto non ha abbastanza celle per questo gioco!");
}

/* MAIN FUNCTION */
int main() 
{	srand(time(NULL));
	WINDOW *win; /* Pointer to a window structure */
	if(init_curses())	 /* if ncurses intialization was successful ... */
	{	win = create_boxed_newwin(15,MAIN_LIMIT,0,0); /* create the menu area on the screen */
		if (win)	/* if the menu area was successfully created*/
			The_Choice(win);	/*then Menu*/
		Free_List_Maze(List);	/* Free Memory of List */
	}
	close_curses(); /* close ncurses */
	return 0;
}
