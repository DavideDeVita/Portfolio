#include "curse.h"
#include<unistd.h>
										// Davide De Vita
										// N86001908
/*Standard Game*/
/* Go from Source to Destination */
/* Gestisce una partita di Standard Game: Inizializzazione, Prossima mossa e esito */
int Std_Game(MAZE Maze, Location Start, Location Destination)
{	WINDOW *Game_Win=NULL;
	Adj_List Path=NULL;
	clear();
	refresh();
	if(Exists_Location(Maze, Start)==1 && Exists_Location(Maze, Destination)==1 && !Same_Location(Start, Destination))
	{	Game_Win = newwin(Maze->rows+2, 2*(Maze->columns+2), 5, 1);
		nodelay(Game_Win ,TRUE); /* Keyboard reads will be non-blocking in window */
		keypad(Game_Win, TRUE); /* Allow control keys to be read (i.e., function and arrow keys) */
		Display_Maze(Game_Win, Maze, 1);	//Show the maze of the game
		wattron(Game_Win, COLOR_PAIR(3));
		int x=Start->riga, y=Start->colonna, ex_x=x, ex_y=y;
		int ch, steps=0;
		char *result=NULL;
		/* Print Character in Start Location */
		wmove(Game_Win, window_row(x), window_column(y));
		waddch(Game_Win, 'o');
		waddch(Game_Win, '0');
		/* Start saving the way */
		Path = Insert_adj_list(Path, x, y);
		/* Print Target in Destination Location */
		wmove(Game_Win, window_row(Destination->riga), window_column(Destination->colonna));
		wattron(Game_Win, COLOR_PAIR(1));
		waddch(Game_Win, '>');
		waddch(Game_Win, '<');
		do{	ch=wgetch(Game_Win);
			ch=redirect(ch);
			if(ch==KEY_UP || ch==KEY_LEFT || ch==KEY_DOWN || ch==KEY_RIGHT)
			{	switch (ch)
				{	case KEY_UP:
						x=(x+Maze->rows-1)%Maze->rows;
						break;
					case KEY_RIGHT:
						y=(y+1)%Maze->columns;
						break;
					case KEY_DOWN:
						x=(x+1)%Maze->rows;
						break;
					case KEY_LEFT:
						y=(y+Maze->columns-1)%Maze->columns;
						break;
				}
				if(Maze->map[x][y]==WALL)
				{	x=ex_x;
					y=ex_y;
					Std_Move_Character(Game_Win, x, y, ex_x, ex_y, ' ');
				}
				if(x!=ex_x || y!=ex_y)
				{	Std_Move_Character(Game_Win, x, y, ex_x, ex_y, ch);
					steps++;	//Effective step
					Path = Insert_adj_list(Path, x, y);
					ex_x=x;
					ex_y=y;
				}
			}
		}while(ch!=ESC && (x!=Destination->riga || y!=Destination->colonna));
		show_message(0, 5, "Usa le frecce per muoverti", 2);
		if(x==Destination->riga && y==Destination->colonna)
		{	show_message(1, 15, "Complimenti giocatore !", 3);
			result = malloc(sizeof(char)*strlen("Completato in 000 passi"));
			sprintf(result, "Completato in %d passi", steps);
		}
		else
		{	show_message(1, 15, "Ci hai provato", 3);
			result = malloc(sizeof(char)*strlen("Provato per 000 passi"));
			sprintf(result, "Provato per %d passi", steps);
		}
		show_message(2, 25, result, 1);
		free(result);
		refresh();
		wclear(Game_Win);
		wrefresh(Game_Win);
		Display_Maze(Game_Win, Maze, 1);
		Path=Revert_List(Path);
		Display_Path(Game_Win, Maze, Path, 2, 5, 10);
		return (x==Destination->riga && y==Destination->colonna);
	}
	else
		return UNDO;
}

/* Stampa il giocatore nella prossima posizione ripulendo la precedente
	Utilizzato in Std e Blind Game */
void Std_Move_Character(WINDOW *Game_Win, int x, int y, int ex_x, int ex_y, int ch)
{	// Clear old one
	wmove(Game_Win, window_row(ex_x), window_column(ex_y));
	wattron(Game_Win, COLOR_PAIR(4));
	waddch(Game_Win, ' ');
	waddch(Game_Win, ' ');
	wattron(Game_Win, COLOR_PAIR(3));
	// Print next
	wmove(Game_Win, window_row(x), window_column(y));
	if(ch==KEY_UP)	//UP
	{	waddch(Game_Win, '/');
		waddch(Game_Win, '\\');
	}
	else if(ch==KEY_LEFT)	//LEFT
	{	waddch(Game_Win, '<');
		waddch(Game_Win, '-');
	}
	else if(ch==KEY_DOWN)	//DOWN
	{	waddch(Game_Win, '\\');
		waddch(Game_Win, '/');
	}
	else if(ch==KEY_RIGHT)	//RIGHT
	{	waddch(Game_Win, '-');
		waddch(Game_Win, '>');
	}
	else	//Stay
	{	waddch(Game_Win, 'o');
		waddch(Game_Win, '0');
	}
	// Wait to update
	while(!refresh_time(5));
	wrefresh(Game_Win);
}

/*Blind Game*/
		/* Muovi il personaggio da Source a Destination
		 potendo vedere solo poche celle attorno a te.
		 Abilitando le torce potrai mantenere la visione attiva anche di aree da te distanti */
 /* Gestisce un 'intera partita Blind: Inizializzazione, prossima mossa e esito */
int Blind_Game(MAZE Maze, Location Start, Location Destination, int torch)
{	WINDOW *Game_Win=NULL;
	clear();
	refresh();
	if(Exists_Location(Maze, Start)==1 && Exists_Location(Maze, Destination)==1)
	{	Game_Win = newwin(Maze->rows+2, (2*Maze->columns)+4, 5, 1);
		nodelay(Game_Win ,TRUE); /* Keyboard reads will be non-blocking in window */
		keypad(Game_Win, TRUE); /* Allow control keys to be read (i.e., function and arrow keys) */
		show_message(0, 5, "Usa le frecce per muoverti", 1);
		show_message(1, 5, "Premi SPAZIO per accendere le torce sparse", 1);
		int x=Start->riga, y=Start->colonna, ex_x=x, ex_y=y;
		int ch, steps, last_ch=-1, sight;
		char *result=NULL;
		Adj_List Torches=NULL, Lighted_up=NULL, Path=NULL;
		Location new_torch=NULL;
		/* Gestione Torce e difficoltà*/
		sight = ((Maze->rows*Maze->columns) > SIGHT_GROW_LIMIT) ? 3 : 2;
		if(torch)	//Quante torce?
		{	torch =  Ways_Cells(Maze)/(5*sight*torch);
			if(torch<sight)
				torch =sight;
			result = malloc(sizeof(char)*strlen("000 torce sparse..."));
			sprintf(result, "%d torce sparse..", torch);
			show_message(2, 7, result, 1);
			free(result);
			result = malloc(sizeof(char)*strlen("000 ancora da accendere"));
			sprintf(result, "%d ancora da accendere", torch);
			show_message(2, 50, result, 1);
			refresh();
			Torches = Create_Torches(Maze, torch, Destination);
		}
		steps=0;
		// Inizia a tener traccia del percorso
		Path = Insert_adj_list(Path, x, y);
		What_can_I_see(Game_Win, Maze, x, y, Destination, sight, last_ch, Torches, Lighted_up);
		do{	ch=wgetch(Game_Win);
			ch=redirect(ch);
			if(ch==KEY_UP || ch==KEY_LEFT || ch==KEY_DOWN || ch==KEY_RIGHT || ch==' ')
			{	if(ch!=' ')
					last_ch = ch;
				switch (ch)
				{	case KEY_UP:
						x=(x+Maze->rows-1)%Maze->rows;
						break;
					case KEY_RIGHT:
						y=(y+1)%Maze->columns;
						break;
					case KEY_DOWN:
						x=(x+1)%Maze->rows;
						break;
					case KEY_LEFT:
						y=(y+Maze->columns-1)%Maze->columns;
						break;
					case ' ':
						new_torch=New_Loc(x, y);
						if(Is_in(Torches, new_torch))
						{	Torches = Remove_Loc(Torches, new_torch);
							Lighted_up = Insert_Location(Lighted_up, new_torch);
							show_message(2, 50, result, 2);
							torch--;
							sprintf(result, "%d ancora da accendere", torch);
							show_message(2, 50, result, 1);
							refresh();
						}
						free(new_torch);
						break;
				}
				if(Maze->map[x][y]==WALL)
				{	x=ex_x;
					y=ex_y;
					Std_Move_Character(Game_Win, x, y, ex_x, ex_y, ' ');
				}
				if(x!=ex_x || y!=ex_y)
				{	Std_Move_Character(Game_Win, x, y, ex_x, ex_y, ch);
					steps++;
					Path = Insert_adj_list(Path, x, y);
					ex_x=x;
					ex_y=y;
				}
				What_can_I_see(Game_Win, Maze, x, y, Destination, sight, last_ch, Torches, Lighted_up);
			}
		}while(ch!=ESC && (x!=Destination->riga || y!=Destination->colonna));
		//Clear old messages
		show_message(0, 5, "Usa le frecce per muoverti", 2);
		show_message(1, 5, "Premi SPAZIO per accendere le torce sparse", 2);
		Free_List(Torches);
		Free_List(Lighted_up);
		if(result)
		{	show_message(2, 7, "000 torce sparse ...", 2);
			show_message(2, 50, result, 2);
			free(result);
		}
		//What about the game
		if(x==Destination->riga && y==Destination->colonna)
		{	show_message(0, 10, "Complimenti giocatore", 3);
			result = malloc(sizeof(char)*strlen("Completato in 000 passi"));
			sprintf(result, "Completato in %d passi", steps);
		}
		else
		{	show_message(0, 10, "Ci hai provato", 3);
			result = malloc(sizeof(char)*strlen("Provato per 000 passi"));
			sprintf(result, "Provato per %d passi", steps);
		}
		show_message(1, 15, result, 1);
		free(result);
		refresh();
		wclear(Game_Win);
		wrefresh(Game_Win);
		Display_Maze(Game_Win, Maze, 1);
		Path=Revert_List(Path);
		Display_Path(Game_Win, Maze, Path, 2, 5, 10);
		if(x==Destination->riga && y==Destination->colonna)	//For future implementations
		{	if(torch)	return 1;
			else	return 3;
		}
		else
			return 0;
	}
	else
		return UNDO;
}

/* Restituisce una lista di locazione diverse ognuna contenente una torcia.
	Nessuna torcia sarà situata in Not_here(Destinazione)
	ogni torcia viene distanziata dalla precedente per aumentarne la dispersione */
Adj_List Create_Torches(MAZE Maze, int total, Location Not_here)
{	Location last_torch=NULL, new_torch;
	int i;
	Adj_List Torches=NULL;
	//Creazione torce
	for(i=0; i<total; i++)
	{	new_torch=Random_Distanced_Location(Maze, last_torch, 35);
		If_Free(last_torch);
		if(!Is_in(Torches, new_torch) && !Same_Location(new_torch, Not_here))
			Torches = Insert_Location(Torches, new_torch);
		else
			i--;
		last_torch=new_torch;
	}
	If_Free(last_torch);
	return Torches;
}

/* Illumina le celle che il giocatore può attualmente vedere. */
void What_can_I_see(WINDOW *win, MAZE Maze, int x, int y, Location Destination, int sight, int last_ch, Adj_List Torches, Adj_List Lighted_up)
{	wclear(win);
	wattron(win, COLOR_PAIR(1));
	box(win, 0, 0);
	/* See Around*/
	int eye, no_wall, i, j;
	Make_Light_Around(win, Maze, x, y, sight, Torches);
	/* Front sight */
	j = (sight==3) ? 5 : 3;
	no_wall=1;
	if(last_ch!=-1)
	{	switch (last_ch)
		{	case KEY_DOWN:
				for(i=1; i<=j && no_wall==1; i++)
				{	eye=(x+i)%Maze->rows;
					no_wall*=(Maze->map[eye][y]==WAY);
				}
				//If no wall in front of me increase sight
				for(i=sight+1; i<=j && no_wall==1; i++)
				{	eye=(x+i)%Maze->rows;
					if(Maze->map[eye][y]==WALL)
						no_wall=0;
					Display_Cell_Torch(win, Maze, eye, y, Torches);
				}
				break;
			case KEY_RIGHT:
				for(i=1; i<=j && no_wall==1; i++)
				{	eye=(y+i)%Maze->columns;
					no_wall*=(Maze->map[x][eye]==WAY);
				}
				//If no wall in front of me increase sight
				for(i=sight+1; i<=j && no_wall==1; i++)
				{	eye=(y+i)%Maze->columns;
					if(Maze->map[x][eye]==WALL)
						no_wall=0;
					Display_Cell_Torch(win, Maze, x, eye, Torches);
				}
				break;
			case KEY_UP:
				for(i=1; i<=j && no_wall==1; i++)
				{	eye=(x-i+Maze->rows)%Maze->rows;
					no_wall*=(Maze->map[eye][y]==WAY);
				}
				//If no wall in front of me increase sight
				for(i=sight+1; i<=j && no_wall==1; i++)
				{	eye=(x-i+Maze->rows)%Maze->rows;
					if(Maze->map[eye][y]==WALL)
						no_wall=0;
					Display_Cell_Torch(win, Maze, eye, y, Torches);
				}
				break;
			case KEY_LEFT:
				for(i=1; i<=j && no_wall==1; i++)
				{	eye=(y-i+Maze->columns)%Maze->columns;
					no_wall*=(Maze->map[x][eye]==WAY);
				}
				//If no wall in front of me increase sight
				for(i=sight+1; i<=j && no_wall==1; i++)
				{	eye=(y-i+Maze->columns)%Maze->columns;
					if(Maze->map[x][eye]==WALL)
						no_wall=0;
					Display_Cell_Torch(win, Maze, x, eye, Torches);
				}
				break;
		}
	}
	/* Lighted Torches */
	if(Lighted_up)
	{	Adj_List torch = Lighted_up;
		while(torch)
		{	Make_Light_Around(win, Maze, torch->locazione->riga, torch->locazione->colonna, sight, Torches);
			torch=torch->next;
		}
	}
	/* Where am I */
	wattron(win, COLOR_PAIR(3));
	wmove(win, window_row(x), window_column(y));
	switch(last_ch)
	{	case KEY_DOWN:
			waddch(win, '\\');
			waddch(win, '/');
			break;
		case KEY_RIGHT:
			waddch(win, '-');
			waddch(win, '>');
			break;
		case KEY_UP:
			waddch(win, '/');
			waddch(win, '\\');
			break;
		case KEY_LEFT:
			waddch(win, '<');
			waddch(win, '-');
			break;
		default:
			waddch(win, 'o');
			waddch(win, '0');
	}
	/* My Arrival */
	wmove(win, window_row(Destination->riga), window_column(Destination->colonna));
	waddch(win, '>');
	waddch(win, '<');
	wrefresh(win);
}

/* Illumina le celle attorno ad una posizione */
void Make_Light_Around(WINDOW *win, MAZE Maze, int x, int y, int sight, Adj_List Torches)
{	int i, j;
	for(i=0; i<=sight; i++)
	{	for(j=0; j<=i; j++)
			Simmetric_rows(win, Maze, i, j, x, y, sight, Torches);
	}
}

/* Esegue lo stesso comando (sight-i) righe sopra e sotto dalla posizione */
void Simmetric_rows(WINDOW *win, MAZE Maze, int i, int j, int x, int y, int sight, Adj_List Torches)
{	int I;
	I=(x-(sight-i)+Maze->rows)%Maze->rows;
	Simmetric_columns(win, Maze, I, j, y, Torches);
	if(i!=sight)
	{	I=(x+(sight-i))%Maze->rows;
		Simmetric_columns(win, Maze, I, j, y, Torches);
	}
}

/* Esegue lo stesso comando j colonne a dx e sx dalla posizione */
void Simmetric_columns(WINDOW *win, MAZE Maze, int I, int j, int y, Adj_List Torches)
{	int J=(y-j+Maze->columns)%Maze->columns;
		Display_Cell_Torch(win, Maze, I, J, Torches);
	if(j!=0)
	{	J=(y+j)%Maze->columns;
		Display_Cell_Torch(win, Maze, I, J, Torches);
	}
}

/* Stampa una cella per Blind Game */
void Display_Cell_Torch(WINDOW *win, MAZE Maze, int i, int j, Adj_List Torches)
{	if(Maze->map[i][j]==WAY)
	{	Location L = New_Loc(i, j);
		if(Is_in(Torches, L))
		{	wattron(win, COLOR_PAIR(6));
			wmove(win, window_row(i), window_column(j));
			waddstr(win, " '");
		}
		else
			Display_Cell(win, Maze, i, j);
		free(L);
	}
	else
		Display_Cell(win, Maze, i, j);
}

/* Guard Mode */
/*	Raccogli le monete @ sparse per la mappa per vincere.
	Drill Ammo: Spara con 'f' nell'ultima direzione presa o indirizza con 'w''a''s''d', questi colpi
	<=		distruggono il primo ostacolo che incontrano, sia esso un muro, una guardia o tu!
	Ghost Walk: Godi del vantaggio di poter attraversare muri, proiettili e persone..
	",		 ma solo per un passo: una volta consumato tornerai nel tuo stato corporeo.
	Difficoltà 1: "Chi vive, quando vive, non si vede: vive"
				L'antico potere dell' invisibilita degli spettri ti permette di non essere visto
				finché resterai in Ghost Mode le guardie non si accorgeranno della tua presenza!
				Inizi con un Drill Ammo e un Ghost Walk.
	Difficoltà 2+: "Μηδὲν ἄγαν."	«Mēden agān»
				Queste guardie incantate vedono oltre la semplice prigione di carne; fiutando
				l'odore della paura potrano percepire la tua presenza anche in Ghost Mode!
				Inizi solo con un Drill Ammo
	Difficoltà 3+: "Sapere Aude".
				Mosse da un sentimento comune le guardie collaboreranno
				per raggiungere il loro bersaglio!
				Non hai armi in partenza.
	Difficoltà 4+: "Ad Maiora"
				Risvegliate da un antico potere, queste guardie vedono più lontano di quelle
				che hai fin ora incontrato.
				Il loro raggio di visione passa da 2 a 3 (3 -> 5 per la vista frontale);
	Difficoltà 5: "Wir sind gewohnt dass die Menschen verhöhnen was sie nicht verstehen"
				Poteri più antichi dell' uomo pervadono queste mura: Ogni cella del labirinto
				contiene una moneta o un oggetto; L'aria densa di un potere arcano libera il vero
				potere del Ghost Walk, trasportandoti nel limbo fra questo mondo e quello dei morti,
				nemmeno la vista incantata delle guardie può giungere fin lì.. ma nemmeno la tua può
				scorgere ciò che accade nella realtà terrena! Potresti anche partire in Ghost Mode
	*/
/* Gestisce una partita di Guard Game: Inizializzazione, turni e esito */
int Guard_Mode(MAZE Maze, Location Start, int difficolta)
{	WINDOW *Game_Win=NULL;
	clear();
	refresh();
	if(Exists_Location(Maze, Start)==1)
	{	int x=Start->riga, y=Start->colonna, ex_x=x, ex_y=y;
		int ch, steps, last_ch=-1, sight, last_dir=-1, default_trouble=0;
		int refr_hp, min_delay=15, max_delay=21, growing_scale, far_off=10;
		int tot_weapon, tot_enemy, start_enemy, tot_coins, drill_ammo=0, ghost_walk=0, game_over=0, ghost=0;
		int **Objects=NULL;
		CHARACTER_LIST Guards=NULL, Fired_Drills=NULL;
		Adj_List Path=NULL;
		/* Gestione armi e difficoltà */
		sight = (difficolta>3) ? 3 : 2;
		ch=Ways_Cells(Maze);	// ex guards :1-2 i%	3 2%	4-5 3% delle celle WAYS
		//if(difficolta<3)	//0,5*difficolta%
			tot_enemy = ch*difficolta/400;
		/* else if (difficolta==3)
			tot_enemy = ch*2/100;
		else if
			tot_enemy = ch*3/100; */
		if(tot_enemy==0)
			tot_enemy++;
		tot_weapon = tot_enemy*3-((tot_enemy*difficolta)/2);
		if(tot_weapon<2)
			tot_weapon++;
		
		if(difficolta!=5)
			tot_coins = (difficolta*ch)/5;
		else
		{	tot_coins = ch-tot_weapon-1;	//Ogni cella
			ghost=(rand()%365)%12==0;	//Chance di partire ghost
		}
		if(tot_coins+tot_weapon>=ch)
			tot_weapon=ch-tot_coins-difficolta;
		if(ch<2*(tot_enemy+1))
			return UNDO;
		/* Messo qui per eventuali modifiche */				//5%
		Guard_Rules(difficolta, &min_delay, &max_delay, &tot_enemy, (ch/20), &ghost, &drill_ammo, &ghost_walk, &default_trouble, &far_off);
		Objects=Spread_in_Maze(tot_weapon, tot_coins, Maze, Start);
		if(Objects==NULL)
			return UNDO;
		/* Inclinazione dell' iperbole */
		growing_scale=(min_delay-max_delay)*450;
		/*Init Map and Win*/
		show_message(0, 5, "Raccogli le monete sparse in giro per la mappa per vincere", 1);
		show_message(1, 5, "Usa le frecce per muoverti. (freccia + Ctrl o Alt per girarti senza muoverti)", 1);
		show_message(2, 5, "Premi 'f' per sparare, o 'w''a''s''d' per indirizzare il colpo; SPAZIO per fermarti", 1);
		refresh();
		Game_Win = newwin(Maze->rows+2, (2*Maze->columns)+4, 7, 1);
		nodelay(Game_Win ,TRUE); /* Keyboard reads will be non-blocking in window */
		keypad(Game_Win, TRUE); /* Allow control keys to be read (i.e., function and arrow keys) */
		Path = Insert_adj_list(Path, x, y);
		Guards =  Make_Guards(Maze, tot_enemy, Start, far_off);
		Display_Map(Game_Win, Maze, x, y, Objects, last_ch, Fired_Drills, difficolta, ghost);
		Am_I_in_Troubles(Game_Win, Maze, x, y, Guards, sight, difficolta, Objects, ghost, Fired_Drills, default_trouble);
		wrefresh(Game_Win);
		if(difficolta<3)
		{	drill_ammo++;
			if(difficolta<2)
				ghost_walk++;
		}
		start_enemy=tot_enemy;
		Update_Message(tot_weapon, tot_enemy, tot_coins, drill_ammo, ghost_walk, 1);
		do{	if(last_ch!=-1)
			{	Update_Message(tot_weapon, tot_enemy, tot_coins, drill_ammo, ghost_walk, 1);
				Fired_Drills=Bul_Movement(Maze, Fired_Drills);
				Fired_Drills = Does_Bul_Hit(Maze, Objects, &Guards, x, y, Fired_Drills, &game_over, &tot_enemy, &ghost, &ghost_walk);
				if(!game_over)
				{	La_ronda(Maze, difficolta, Guards);
					game_over=Busted(Guards, x, y, &ghost, &ghost_walk);
					/* Qualche guardia ha urtato un drill? (forse lo sposto in La ronda) */
					/* tecnicamente ci sono meno bullets che guardie */
					Fired_Drills = Does_Bul_Hit(Maze, Objects, &Guards, x, y, Fired_Drills, &game_over, &tot_enemy, NULL, NULL);
					if(!game_over)
					{	Display_Map(Game_Win, Maze, x, y, Objects, last_ch, Fired_Drills, difficolta, ghost);
						Am_I_in_Troubles(Game_Win, Maze, x, y, Guards, sight, difficolta, Objects, ghost, Fired_Drills, default_trouble);
						wrefresh(Game_Win);
					}
				}
			}
			if(!game_over)
			{	ch=wgetch(Game_Win);
				if(ch==-1)
					ch = last_ch;
				else
				{	ch=Guard_Game_redirect(ch);
					if(ch==KEY_UP || ch==KEY_LEFT || ch==KEY_DOWN || ch==KEY_RIGHT)
					{	steps++;
						// Vedi Am i in troubles per spiegazioni su quest' if
						if(difficolta==5 && steps%start_enemy==0)
							Guards = Revert_List_CH(Guards);
						last_ch = ch;
						last_dir = ch;
					}
					else if(Is_Fire_Key(ch) || ch=='g') //Using Items
					{	if(Is_Fire_Key(ch) && drill_ammo>0 && last_dir>-1)
						{	//Single Movement per muovere l'ultimo proiettile prima di sarare ed evitare conflitti
							//N.B.: Tre colpi in rapida successione ne annullano uno!
							//Single_Bul_Movement(Maze, Fired_Drills, NULL);
							Fired_Drills=Fire_Drill(Fired_Drills, Maze, x, y, Fire_direction(ch, last_dir));
							drill_ammo--;
							Fired_Drills = Does_Bul_Hit(Maze, Objects, &Guards, x, y, Fired_Drills, &game_over, &tot_enemy, &ghost, &ghost_walk);
							if(!game_over)
							{	Single_Bul_Movement(Maze, Fired_Drills, NULL);
								Fired_Drills = Does_Bul_Hit(Maze, Objects, &Guards, x, y, Fired_Drills, &game_over, &tot_enemy, &ghost, &ghost_walk);
							}
							ch=last_ch;
						}
						else if(ch=='g')
						{	if(ghost)
								ghost=0;
							else if(ghost_walk>0)
								ghost=1;
							if(last_ch!=-1)
								ch=last_ch;
							else
								ch=' ',	last_ch=' ';
						}
					}
					else if(ch==CTRL_UP || ch==CTRL_LEFT || ch==CTRL_DOWN || ch==CTRL_RIGHT)
					{	if(ch==CTRL_UP)
							last_dir=KEY_UP;
						else if(ch==CTRL_RIGHT)
							last_dir=KEY_RIGHT;
						else if(ch==CTRL_DOWN)
							last_dir=KEY_DOWN;
						else if(ch==CTRL_LEFT)
							last_dir=KEY_LEFT;
						ch=' ';
					}
				}
				
				if(!game_over)
				{	switch (ch)
					{	case KEY_UP:
							x=(x+Maze->rows-1)%Maze->rows;
							break;
						case KEY_RIGHT:
							y=(y+1)%Maze->columns;
							break;
						case KEY_DOWN:
							x=(x+1)%Maze->rows;
							break;
						case KEY_LEFT:
							y=(y+Maze->columns-1)%Maze->columns;
							break;
						case ' ':
							last_ch =' ';
							break;
						case '~':
							Free_CHL(Guards);
							Guards=NULL;
							Free_CHL(Fired_Drills);
							Fired_Drills=NULL;
							tot_enemy=0;
							break;
					}
				/* Something has changed */
					if(x!=ex_x || y!=ex_y)
					{	if(Maze->map[x][y]==WALL && !ghost)
							x=ex_x,	y=ex_y;
						else
						{	if(Maze->map[x][y]==WALL)
							{	if(ghost_walk>0)
									ghost_walk--;
								ghost=0;
							}
							else if(Objects[x][y]>0 && Objects[x][y]<4)
							{	if(Objects[x][y]==1)
									drill_ammo++,	tot_weapon--;
								else if(Objects[x][y]==2)
									ghost_walk++,	tot_weapon--;
								else if(Objects[x][y]==3)
									tot_coins--;
								Objects[x][y]=0;
							}
							ex_x=x;
							ex_y=y;
							Path = Insert_adj_list(Path, x, y);
						}
						game_over=Busted(Guards, x, y, &ghost, &ghost_walk);
						if(!game_over)
							Fired_Drills = Does_Bul_Hit(Maze, Objects, &Guards, x, y, Fired_Drills, &game_over, &tot_enemy, &ghost, &ghost_walk);
					}
					/* Se ci sono nemici vivi aggiorna 16<y<22
					  	per un tale y mi servo della funzione iperbole
					  	y=-(a/x)+b	x=tot_enemy*r*c/100
					  */
					refr_hp=(tot_enemy*Maze->rows*Maze->columns)/100;
					refr_hp=growing_scale/(refr_hp+450)+max_delay;
  					while(!refresh_time(tot_enemy>0 ? refr_hp : min_delay ));	//Delay
				}
			}
			/*update  bullets*/
		}while(ch!=ESC && tot_coins>0 && !game_over);
		show_message(0, 5, "Raccogli le monete sparse in giro per la mappa per vincere", 2);
		show_message(1, 5, "Usa le frecce per muoverti. (freccia + Ctrl o Alt per girarti senza muoverti)", 2);
		show_message(2, 5, "Premi 'f' per sparare, o 'w''a''s''d' per indirizzare il colpo; SPAZIO per fermarti", 2);
		Update_Message(99, 99, 99, 99, 99, 2);
		Free_CHL(Guards);
		Free_CHL(Fired_Drills);
		if(game_over)
			show_message(0, 5, "GAME OVER", 8);
		else if(tot_coins==0)
			show_message(0, 5, "Complimenti!", 9);
		else
			show_message(0, 5, "Resa", 3);
		refresh();
		wclear(Game_Win);
		wrefresh(Game_Win);
		Display_Maze(Game_Win, Maze, 1);
		Path=Revert_List(Path);
		Display_Path(Game_Win, Maze, Path, 2, 5, 10);
		if(ch==ESC)
			return ESC;
		else
		{	if(tot_coins!=0) return 0;
			else if(tot_enemy!=start_enemy)
			{	if(tot_enemy==0)	return -3;
				else	return 1;
			}
			else	return 3;
		}
	}
	else
		return UNDO;
}

/* Stampa la mappa di Guard Game nell' ordine: oggetti, proiettili e giocatore
	Oggetti stampati dopo hanno maggior priorità di visualizzazione */
void Display_Map(WINDOW *win, MAZE Maze, int x, int y, int **Map, int last_ch, CHARACTER_LIST Drills, int difficolta, int ghost)
{	wclear(win);
	box(win, 0, 0);
	Display_Maze(win, Maze, 0);
	int i, j;
	/* items */
	if(difficolta<5 || !ghost)	//Can't see
	{	wattron(win, COLOR_PAIR(6));
		for(i=0; i<Maze->rows; i++)
		{	for(j=0; j<Maze->columns; j++)
			{	if(Map[i][j]==1)//Drill
				{	wmove(win, window_row(i), window_column(j));
					waddch(win, '<');
					waddch(win, '=');
				}
				else if(Map[i][j]==2)//Ghost / Bomb
				{	wmove(win, window_row(i), window_column(j));
					waddch(win, '"');
					waddch(win, ',');
				}
				else if(Map[i][j]==3)//Coin
				{	wmove(win, window_row(i), window_column(j));
					waddch(win, ' ');
					waddch(win, '@');
				}
			}
		}
		/* Bullets */
		Display_Bullets(win, Drills);
	}
	/* Where am I */
	Where_am_I(win, x, y, last_ch, ghost);
	//wrefresh(win);
}

/* Stampa il giocatore per la modalità Guard Game */
void Where_am_I(WINDOW *win, int x, int y, int last_ch, int ghost)
{	if(!ghost)
		wattron(win, COLOR_PAIR(3));
	else
		wattron(win, COLOR_PAIR(1));
	wmove(win, window_row(x), window_column(y));
	switch(last_ch)
	{	case KEY_DOWN:
			waddch(win, '\\');
			waddch(win, '/');
			break;
		case KEY_RIGHT:
			waddch(win, '-');
			waddch(win, '>');
			break;
		case KEY_UP:
			waddch(win, '/');
			waddch(win, '\\');
			break;
		case KEY_LEFT:
			waddch(win, '<');
			waddch(win, '-');
			break;
		default:
			waddch(win, 'o');
			waddch(win, '0');
	}
}

/* Gestisce la visione e logica delle guardie:
	Stampa le guardie e mostra quanto possono vedere senza coprire gli oggetti;
	Se una guardia può vedere il giocatore verrà visualizzata di colore rosso.
	Se la difficoltà è superiore a 3 quando una guardia avvista il giocatore
	tutte le successive nella lista sono allertate
	(Questa scelta è stata presa per lasciare alcune guardie nella loro psizione così da
	vincolare il giocatore a scappare evitandole)
	A difficoltà 1 la ghost mode rende invisibile alle guardie
	A difficoltà 5 la ghost mode rende invisibile alle guardie ma non permette al giocatore
	di vedere guardie, oggetti e proiettili. Inoltre la lista delle guardie si capovolge
	ogni G(numero di guardie iniziali) turni così durante gli inseguimenti le guardie tenderanno
	 a chiudere il giocatore */
void Am_I_in_Troubles(WINDOW *win, MAZE Maze, int x, int y, CHARACTER_LIST Guards, int sight, int difficolta, int **Map, int ghost, CHARACTER_LIST Drills, int default_trouble)
{	int trouble=default_trouble;
	CHARACTER_LIST L=Guards;
	while(L)					// AND a destra per stampare le guardie anche s enon possono vedermi	SPOSTA IN IN SIGHT E IMPEDISCI DI DIVENIRE ROSSO
	{	if((difficolta<5 || !ghost) && In_Sight(win, Maze, L->Ch.x, L->Ch.y, Map, x, y, sight, L->Ch.last_dir, Guards, Drills) && (difficolta!=1 || !ghost))
		{	trouble=1;
			L->Ch.alerted = 1;
			if(L->Ch.Last)
				free(L->Ch.Last);
			L->Ch.Last = New_Loc(x, y);
		}
		else if(trouble && difficolta>2)
		{	L->Ch.alerted = 2;	//Avvisato
			if(L->Ch.Last)
				free(L->Ch.Last);
			L->Ch.Last = New_Loc(x, y);
		}
		else if(L->Ch.Last)
		{	if(L->Ch.alerted == 2)
			{	L->Ch.alerted = 0;
				free(L->Ch.Last);
				L->Ch.Last=NULL;
			}
			else if(L->Ch.alerted == 1)
				L->Ch.alerted = 3;
		}
		L = L->next;
	}
}

/* Restituisce 1 se le Guardie catturano il giocatore.
	Se questo tuttavia è in Ghost Mode restituisce zero, rimuove il ghost ammo e annulla
	la ghost mode */
int Busted(CHARACTER_LIST Guards, int x, int y, int *ghost, int *ghost_walk)
{	if(Is_in_CHL(Guards, x, y))
	{	if(!(*ghost))
			return 1;
		else
		{	(*ghost)=0;
			if(*ghost_walk)
				(*ghost_walk)--;
		}
	}
	return 0;
}

/* Stampa la gurdia e illumina ciò che può vedere, restituendo 1 se il giocatore è visto */
int In_Sight(WINDOW *win, MAZE Maze, int x, int y, int **Map, int X, int Y, int sight, int last_dir, CHARACTER_LIST Guards, CHARACTER_LIST Drills)
{	int i, j, trouble=0;
	wattron(win, COLOR_PAIR(7));
	/*Around*/
	for(i=0; i<=sight; i++)
	{	for(j=0; j<=i; j++)
			if(i!=sight || j!=0)//USELESS ITERATION
				trouble+=Sight_simmetric_rows(win, Maze, i, j, Map, x, y, sight, X, Y, Guards, Drills);
	}
	/*front*/
	trouble += Frontal_Sight(win, Maze, Map, x, y, sight, X, Y, last_dir, Guards, Drills);
	if(trouble>0)
	{	wattron(win, COLOR_PAIR(8));
		Print_Guard(win, x, y, last_dir);
		return 1;
	}
	else
	{	wattron(win, COLOR_PAIR(1));
		Print_Guard(win, x, y, last_dir);
		return 0;
	}
}

/* Subroutine di In Sight per gestire la vista frontale amplificata */
int Frontal_Sight(WINDOW *win, MAZE Maze, int **Map, int x, int y, int sight, int X, int Y, int last_dir, CHARACTER_LIST Guards, CHARACTER_LIST Drills)
{	int i;
	int j = (sight==3) ? 5 : 3;
	int no_wall=1, trouble=0, eye;
	switch (last_dir)
	{	case KEY_DOWN:
			for(i=1; i<=j && no_wall==1; i++)
			{	eye=(x+i)%Maze->rows;
				if(Maze->map[eye][y]==WALL)
					no_wall=0;
			}
				//If no wall in front of me increase sight
			for(i=sight+1; i<=j && no_wall==1; i++)
			{	eye=(x+i)%Maze->rows;
				if(Maze->map[eye][y]==WALL)
					no_wall=0;
				trouble = Eye_of_Guard(win, Maze, eye, y, Map, X, Y, Guards, Drills);
			}
			break;
		case KEY_RIGHT:
			for(i=1; i<=j && no_wall==1; i++)
			{	eye=(y+i)%Maze->columns;
				if(Maze->map[x][eye]==WALL)
					no_wall=0;
			}
			//If no wall in front of me increase sight
			for(i=sight+1; i<=j && no_wall==1; i++)
			{	eye=(y+i)%Maze->columns;
				if(Maze->map[x][eye]==WALL)
					no_wall=0;
				trouble = Eye_of_Guard(win, Maze, x, eye, Map, X, Y, Guards, Drills);
			}
			break;
		case KEY_UP:
			for(i=1; i<=j && no_wall==1; i++)
			{	eye=(x-i+Maze->rows)%Maze->rows;
				if(Maze->map[eye][y]==WALL)
					no_wall=0;
			}
			//If no wall in front of me increase sight
			for(i=sight+1; i<=j && no_wall==1; i++)
			{	eye=(x-i+Maze->rows)%Maze->rows;
				if(Maze->map[eye][y]==WALL)
					no_wall=0;
				trouble = Eye_of_Guard(win, Maze, eye, y, Map, X, Y, Guards, Drills);
			}
			break;
		case KEY_LEFT:
			for(i=1; i<=j && no_wall==1; i++)
			{	eye=(y-i+Maze->columns)%Maze->columns;
				if (Maze->map[x][eye]==WALL)
					no_wall=0;
			}
			//If no wall in front of me increase sight
			for(i=sight+1; i<=j && no_wall==1; i++)
			{	eye=(y-i+Maze->columns)%Maze->columns;
				if(Maze->map[x][eye]==WALL)
					no_wall=0;
				trouble = Eye_of_Guard(win, Maze, x, eye, Map, X, Y, Guards, Drills);
			}
			break;
	}
	return trouble;
}

/* Esegue lo stesso comportamento (sight-i) righe sopra e sotto la posizione */
int Sight_simmetric_rows(WINDOW *win, MAZE Maze, int i, int j, int **Map, int x, int y, int sight, int X, int Y, CHARACTER_LIST Guards, CHARACTER_LIST Drills)
{	int I, trouble=0;
	I=(x-(sight-i)+Maze->rows)%Maze->rows;
	trouble=Sight_simmetric_columns(win, Maze, I, j, y, Map, X, Y, Guards, Drills);
	if(i!=sight)
	{	I=(x+(sight-i))%Maze->rows;
		trouble+=Sight_simmetric_columns(win, Maze, I, j, y, Map, X, Y, Guards, Drills);
	}
	return trouble;
}

/* Esegue lo stesso comportamento j colonne a sx e dx dalla posizione */
int Sight_simmetric_columns(WINDOW *win, MAZE Maze, int I, int j, int y, int **Map, int X, int Y, CHARACTER_LIST Guards, CHARACTER_LIST Drills)
{	int trouble=0, J;
	J=(y-j+Maze->columns)%Maze->columns;
	trouble=Eye_of_Guard(win, Maze, I, J, Map, X, Y, Guards, Drills);
	if(j!=0)
	{	J=(y+j)%Maze->columns;
		trouble+=Eye_of_Guard(win, Maze, I, J, Map, X, Y, Guards, Drills);
	}
	return trouble;
}

/* Illumina la cella vista dalla guardia
	se questa non è un oggetto, un proiettile, un muro o il giocatore */
int Eye_of_Guard(WINDOW *win, MAZE Maze, int i, int j, int **Map, int x, int y, CHARACTER_LIST Guards, CHARACTER_LIST Drills)
{	if(Maze->map[i][j]==WALL)
		return 0;
	/*else*/
	if(i==x && j==y)
		return 1;
	else if(Map[i][j]==0 && !Is_in_CHL(Guards, i, j) && !Is_in_CHL(Drills, i, j))
	{	wattron(win, COLOR_PAIR(7));
		wmove(win, window_row(i), window_column(j));
		waddch(win, ' ');
		waddch(win, ' ');
	}
	return 0;
}

/* Inizializza una matrice corrispondente al Labirinto di gioco
	per memorizzare le posizioni di monete e oggetti */
int** Start_Map(MAZE Maze)
{	int **Map=malloc(sizeof(int *)*Maze->rows);
	int i, j;
	for(i=0; i<Maze->rows; i++)
	{	Map[i]=malloc(sizeof(int)*Maze->columns);
		for(j=0; j<Maze->columns; j++)
			if(Maze->map[i][j]==WAY)
				Map[i][j]=0;
			else
				Map[i][j]=-2;
	}
	return Map;
}

/* Crea e restituisce una lista di locazioni corrispondenti alle guardie
	ciascuna sarà distanzata dal giocatore */
CHARACTER_LIST Make_Guards(MAZE Maze, int total, Location Start, int far_off)
{	CHARACTER_LIST List=NULL;
	int i;
	Location L;
	for(i=0; i<total; i++)
	{	L=Random_Distanced_Location(Maze, Start, far_off);
		if(!Is_in_CHL(List, L->riga, L->colonna) )// && !Same_Location(L, Start))
			List = Insert_CH_Location(List, L);
		else
			i--;
		free(L);
	}
	return List;
}

/* Sparge nel Labirinto gli oggetti e le monete prestando attenzione a non sovrapporle
	a muri, al giocatore o tra loro */
int **Spread_in_Maze(int tot_weapons, int tot_coins, MAZE Maze, Location Source)
{	int **Objects=NULL;
	int i;
	if(Ways_Cells(Maze)<=(tot_weapons+tot_coins))
		return NULL;
	Objects=Start_Map(Maze);
	Objects[Source->riga][Source->colonna]=-1;
	/*Wall or Me -2/-1	Drill 1	Ghost 2	Coin 3*/
	i = (rand()%(tot_weapons-1))+1;
	if(i<tot_weapons-i)
		i = tot_weapons-i;
	Spread(Objects, Maze, i, 1);
	Spread(Objects, Maze, tot_weapons-i, 2);
	if(tot_coins == Ways_Cells(Maze)-tot_weapons-1)
	{	int i, j;
		for(i=0; i<Maze->rows; i++)
			for(j=0; j<Maze->columns; j++)
				if(Objects[i][j]==0)
					Objects[i][j]=3;
	}
	else
		Spread(Objects, Maze, tot_coins, 3);
	Objects[Source->riga][Source->colonna]=0;
	return Objects;
}

/* Subroutine di Spread in Maze per spargere un particolare oggetto 
	Drill Ammo, Ghost Walk, Coin) */
void Spread(int** Map, MAZE Maze, int total, int val)
{	int i;
	Location L;
	for(i=0; i<total; i++)
	{	L=Random_cell(Maze);
		if(Map[L->riga][L->colonna]==0)
			Map[L->riga][L->colonna]=val;
		else
			i--;
		free(L);
	}
}
/* Gestisce il movimento e gli inseguimenti delle guardie:
	Se una guardia ha un' ultima posizione nota del giocatore la raggiunge
	altrimenti si muove randomicamente.
	se la posizione del giocatore non dovesse risultare raggiungibile la guardia si muoverà
	randomicamente */
void La_ronda(MAZE Maze, int difficolta, CHARACTER_LIST Guards)
{	CHARACTER_LIST List=Guards;
	Adj_List Adj=NULL;
	int EurN = (difficolta==3) ? MANHATTAN : (difficolta==4) ? EUCLIDEAN :  T_MANHATTAN;
	int EurL=(difficolta<3) ? MANHATTAN : difficolta-2;
	Map_info Info;
	Location Pos=New_Loc(0, 0), Next_pos;
	while(List)
	{	Pos->riga=List->Ch.x;
		Pos->colonna=List->Ch.y;
		if(List->Ch.Last && Same_Location(Pos, List->Ch.Last))
		{	free(List->Ch.Last);
			List->Ch.Last=NULL;
			List->Ch.alerted=0;
		}
		
		if(List->Ch.alerted==0)
		{	Adj=Locazioni_adiacenti(Maze, Pos);
			Next_pos= Get_Random_element(Adj);
		}
		else
		{	if(List->Ch.alerted==1)
				Info=A_Star(Maze, Pos, List->Ch.Last, T_EUCLIDEAN, NULL);
			else if(List->Ch.alerted==2)
			{	/*if(rand()%2==0)
					Info=A_Star(Maze, Pos, List->Ch.Last, EurN, NULL);
				else*/
					Info=A_Star_G(Maze, Pos, List->Ch.Last, EurN, NULL);
			}
			else if(List->Ch.alerted==3)
				Info=A_Star(Maze, Pos, List->Ch.Last, EurL, NULL);
			Adj = Compute_Path(Maze, Info, Pos, List->Ch.Last);
			if(Adj)
			{	Adj = Next(Adj);
				if(Adj)
				Next_pos= New_Loc(Adj->locazione->riga, Adj->locazione->colonna);
				else
				{	Adj=Locazioni_adiacenti(Maze, Pos);
					Next_pos= Get_Random_element(Adj);
				}
			}
			else
			{	Adj=Locazioni_adiacenti(Maze, Pos);
				Next_pos= Get_Random_element(Adj);
			}
		}
		Update_Guard_Position(List, Maze, Pos, Next_pos);
		Free_List(Adj);
		If_Free(Next_pos);
		List=List->next;
	}
	free(Pos);
}

/* Aggiorna l'ultima direzione presa dalla guardia per poterla successivamente stampare */
void Update_Guard_Position(CHARACTER_LIST Guard, MAZE Maze, Location Pos, Location Next)
{	if(Next)
	{	if(Next->riga==(Pos->riga+1)%Maze->rows)
			Guard->Ch.last_dir=KEY_DOWN;
		else if(Next->colonna==(Pos->colonna+1)%Maze->columns)
			Guard->Ch.last_dir=KEY_RIGHT;
		else if(Next->riga==(Pos->riga+Maze->rows-1)%Maze->rows)
			Guard->Ch.last_dir=KEY_UP;
		else if(Next->colonna==(Pos->colonna+Maze->columns-1)%Maze->columns)
			Guard->Ch.last_dir=KEY_LEFT;
		else
			Guard->Ch.last_dir='A';
		Guard->Ch.x=Next->riga;
		Guard->Ch.y=Next->colonna;
	}
	else
		Guard->Ch.last_dir=' ';
}

/* Stampa la guardia */
void Print_Guard(WINDOW *win, int x, int y, int last_dir)
{	wmove(win, window_row(x), window_column(y));
	switch (last_dir)
	{	case KEY_UP:
			waddch(win, '/');
			waddch(win, '\\');
			break;
		case KEY_RIGHT:
			waddch(win, '-');
			waddch(win, '>');
			break;
		case KEY_DOWN:
			waddch(win, '\\');
			waddch(win, '/');
			break;
		case KEY_LEFT:
			waddch(win, '<');
			waddch(win, '-');
			break;
		case ' ':
			waddch(win, '0');
			waddch(win, '0');
			break;
		case 'A':
			waddch(win, 'A');
			waddch(win, 'A');
			break;
	}
}

/* Aggiunge un nuovo proiettile alla lista dei proiettili sparati
	L'inserimento avviene in coda alla lista per ovviare ad un bug che causava l'eliminazione
	dei proiettili sparati in rapida successione */
CHARACTER_LIST Fire_Drill(CHARACTER_LIST Fired_Drills, MAZE Maze, int x, int y, int last_ch)
{	if(last_ch==KEY_DOWN)
		x= (x+1)%Maze->rows;
	else if(last_ch==KEY_UP)
		x= (x+Maze->rows-1)%Maze->rows;
	else if(last_ch==KEY_RIGHT)
		y= (y+1)%Maze->columns;
	else if(last_ch==KEY_LEFT)
		y= (y+Maze->columns-1)%Maze->columns;
	else	//Wrong something
		return Fired_Drills;
	CHARACTER_LIST New = malloc(sizeof(struct character_list));
	CHARACTER_LIST Curr=Fired_Drills, Pre=NULL;
	New->Ch.x=x;
	New->Ch.y=y;
	New->Ch.alerted = 0;
	New->Ch.Last = NULL;
	New->Ch.last_dir=last_ch;
	New->next=NULL;
	/* In coda */
	while(Curr)
	{	Pre=Curr;
		Curr=Curr->next;
	}
	if(!Pre)
		Fired_Drills=New;
	else
	{	Pre->next=New;
	}
	return Fired_Drills;
	/* 
	New->next = Fired_Drills;
	return New;
	 */
}

/* Restituisce 1 se le coordinate sono presenti nella Lista di Character */
int Is_in_CHL(CHARACTER_LIST List, int riga, int colonna)
{	CHARACTER_LIST eye=List;
	int in=0;
	while(eye && !in)
	{	if(eye->Ch.x==riga && eye->Ch.y==colonna)
			in = 1;
		eye=eye->next;
	}
	return in;
}

/* Aggiunge un elemento alla Character list */
CHARACTER_LIST Insert_CH_Location(CHARACTER_LIST List, Location L)
{	CHARACTER_LIST New = malloc(sizeof(struct character_list));
	New->Ch.x=L->riga;
	New->Ch.y=L->colonna;
	New->Ch.alerted = 0;
	New->Ch.Last = NULL;
	New->Ch.last_dir=' ';
	New->next = List;
	return New;
	
}

/* Controlla se un proiettile si imbatte in un ostacolo e gestisce il comportamento */
CHARACTER_LIST Does_Bul_Hit(MAZE Maze, int **Map, CHARACTER_LIST *Guards, int x, int y, CHARACTER_LIST Fired_Drills, int *hit_me, int *enemy, int *ghost, int *ghost_walk)
{	if(Is_in_CHL(Fired_Drills, x, y) && ghost && !(*ghost))
		*hit_me=1;
	else
	{	if(ghost && Is_in_CHL(Fired_Drills, x, y))	//if ghost then check if hit
		{	(*ghost)=0;
			if(*ghost_walk)
				(*ghost_walk)--;
		}
		CHARACTER_LIST Curr=Fired_Drills, erase;
		while(Curr)
		{	erase=Curr;
			Curr=Curr->next;
			if(Is_in_CHL(*Guards, erase->Ch.x, erase->Ch.y))	//Guard_Kill
			{	(*enemy)--;
				*Guards=Remove_Ch(*Guards, erase->Ch.x, erase->Ch.y);
				Fired_Drills=Remove_Ch(Fired_Drills, erase->Ch.x, erase->Ch.y);
			}
			else if(Maze->map[erase->Ch.x][erase->Ch.y]==WALL)
			{	Map[erase->Ch.x][erase->Ch.y]=0;
				Maze->map[erase->Ch.x][erase->Ch.y]=WAY;
				Fired_Drills=Remove_Ch(Fired_Drills, erase->Ch.x, erase->Ch.y);
			}
		}
	}
	return Fired_Drills;
}

/* Libera la memoria associata ad una Character list */
void Free_CHL(CHARACTER_LIST List)
{	while(List)
		List=Next_CH(List);
}

/* Rimuove un elemento dalla Character list */
CHARACTER_LIST Remove_Ch(CHARACTER_LIST List, int x, int y)
{	CHARACTER_LIST Curr=List, Pre=NULL;
	while(Curr && (Curr->Ch.x!=x || Curr->Ch.y!=y))
	{	Pre = Curr;
		Curr=Curr->next;		
	}
	if(Curr)
	{	if(!Pre)
			List = Next_CH(List);
		else
			Pre->next = Next_CH(Pre->next);
	}
	return List;
}

/* Rimuove la testa della character list passata */
CHARACTER_LIST Next_CH(CHARACTER_LIST List)
{	CHARACTER_LIST erase = List, ret = List->next;
	free(erase);
	return ret;
}

/* Capovolge la Lista */
CHARACTER_LIST Revert_List_CH(CHARACTER_LIST List)	//It
{	CHARACTER_LIST Curr=List, Pre=NULL, temp=NULL;
	while(Curr)
	{	temp=Curr->next;
		Curr->next=Pre;
		Pre=Curr;
		Curr=temp;
	}
	return Pre;
}

/* Aggiorna le posizioni dei proiettili.
	la variabile alerted verifica se questa è stata già aggiornata o meno
	Questo viene fatto per gestire correttamente le intersezioni di proiettili */
CHARACTER_LIST Bul_Movement(MAZE Maze, CHARACTER_LIST Fired_Drills)
{	CHARACTER_LIST Curr=Fired_Drills;
	while(Curr)
	{	if(Curr->Ch.alerted==1) //Già aggiornato
			Curr=Curr->next;
		else
		{	switch (Curr->Ch.last_dir)
			{	case KEY_UP:
					Curr->Ch.x= (Curr->Ch.x+Maze->rows-1)%Maze->rows;
					break;
				case KEY_LEFT:
					Curr->Ch.y= (Curr->Ch.y+Maze->columns-1)%Maze->columns;
					break;
				case KEY_DOWN:
					Curr->Ch.x= (Curr->Ch.x+1)%Maze->rows;
					break;
				case KEY_RIGHT:
					Curr->Ch.y= (Curr->Ch.y+1)%Maze->columns;
					break;
			}
			if(Bullet_Cross(Fired_Drills, Maze, Curr))
			{	int x=Curr->Ch.x, y=Curr->Ch.y;
				Fired_Drills=Remove_Ch(Fired_Drills, x, y);
				Fired_Drills=Remove_Ch(Fired_Drills, x, y);
				Curr=Fired_Drills;
			}
			else
			{	Curr->Ch.alerted=1;
				Curr=Curr->next;
			}
		}
	}
	Curr=Fired_Drills;
	while(Curr)	//Reset all 0
	{	Curr->Ch.alerted=0;
		Curr=Curr->next;
	}
	return Fired_Drills;	
}

/* Aggiorna la posizione dell'ultimo proiettile.
	Questo viene fatto per permettere al colpo appena sparato di percorrere due caselle
	evitando che i tutti i colpi si muovano
	Se thisOne =NULL Si muove l'ultimo  */
void Single_Bul_Movement(MAZE Maze, CHARACTER_LIST Fired_Drills, CHARACTER_LIST This_one)
{	CHARACTER_LIST Curr=Fired_Drills, Drill=NULL;
	if(!This_one)
	{	while(Curr)
		{	Drill=Curr;
			Curr=Curr->next;
		}
	}
	else
		Drill=This_one;
	if(Drill)
	{	switch (Drill->Ch.last_dir)
		{	case KEY_UP:
				Drill->Ch.x= (Drill->Ch.x+Maze->rows-1)%Maze->rows;
				break;
			case KEY_LEFT:
				Drill->Ch.y= (Drill->Ch.y+Maze->columns-1)%Maze->columns;
				break;
			case KEY_DOWN:
				Drill->Ch.x= (Drill->Ch.x+1)%Maze->rows;
				break;
			case KEY_RIGHT:
				Drill->Ch.y= (Drill->Ch.y+1)%Maze->columns;
				break;
		}	
	}
}

/* Restituisce 1 se il proiettile Drill urta altri proiettili, 0 altrimenti */
int Bullet_Cross(CHARACTER_LIST Bullets, MAZE Maze, CHARACTER_LIST Drill)
{	int cross=0;
	CHARACTER_LIST List=Bullets;
	while(List && !cross)
	{	if(Drill!=List && Drill->Ch.x==List->Ch.x && Drill->Ch.y==List->Ch.y)
		{	if(List->Ch.last_dir!=Drill->Ch.last_dir)
				cross=1;
			else
			{	Single_Bul_Movement(Maze, Bullets, List);
				cross=Bullet_Cross(Bullets, Maze, List);
				List=List->next;
			}
		}
		else
			List=List->next;
	}
	return cross;
}

/* Stampa i proiettili */
void Display_Bullets(WINDOW *win, CHARACTER_LIST Fired_Drills)
{	CHARACTER_LIST Curr=Fired_Drills;
	wattron(win, COLOR_PAIR(3));
	while(Curr)
	{	switch (Curr->Ch.last_dir)
		{	case KEY_UP:
				wmove(win, window_row(Curr->Ch.x), window_column(Curr->Ch.y));
				waddch(win, '^');
				waddch(win, '^');
				break;
			case KEY_RIGHT:
				wmove(win, window_row(Curr->Ch.x), window_column(Curr->Ch.y));
				waddch(win, '=');
				waddch(win, '>');
				break;
			case KEY_DOWN:
				wmove(win, window_row(Curr->Ch.x), window_column(Curr->Ch.y));
				waddch(win, 'v');
				waddch(win, 'v');
				break;
			case KEY_LEFT:
				wmove(win, window_row(Curr->Ch.x), window_column(Curr->Ch.y));
				waddch(win, '<');
				waddch(win, '=');
				break;
		}
		Curr=Curr->next;
	}
	//wrefresh(win);
}

/* Restituisce il valore rappresentante per tasti diversi che hanno
	la stessa funzionalità nella Guard Mode */
int Guard_Game_redirect(int ch)
{	/*Directed fire*/
	if(ch=='w' || ch=='W')
		return 'w';
	else if(ch=='a' || ch=='A')
		return 'a';
	else if(ch=='s' || ch=='S')
		return 's';
	else if(ch=='d' || ch=='D')
		return 'd';
	/*Turn key*/
	if(ch==CTRL_UP || ch==MAIUSC_UP || ch==ALT_UP || ch==CTRL_MAIUSC_UP)
		return CTRL_UP;
	else if(ch==CTRL_LEFT || ch==MAIUSC_LEFT || ch==ALT_LEFT || ch==CTRL_MAIUSC_LEFT)
		return CTRL_LEFT;
	else if(ch==CTRL_DOWN || ch==MAIUSC_DOWN || ch==ALT_DOWN || ch==CTRL_MAIUSC_DOWN)
		return CTRL_DOWN;
	else if(ch==CTRL_RIGHT || ch==MAIUSC_RIGHT || ch==ALT_RIGHT || ch==CTRL_MAIUSC_RIGHT)
		return CTRL_RIGHT;
	/*Action Key*/
	else if(ch=='f' || ch=='F' || ch=='z' || ch=='Z')
		return 'f';
	else if(ch=='g' || ch=='G' || ch=='x' || ch=='X')
		return 'g';
	else if(ch=='g' || ch=='G' || ch=='x' || ch=='X')
		return 'g';
	else
		return ch;
}

/* Restituisce 1 se il tasto è associato a un comando di fuoco */
int Is_Fire_Key(int ch)
{	if(ch=='f' || ch=='w' || ch=='a' || ch=='s' || ch=='d')
		return 1;
	else
		return 0;
}

/* Restituisce la direzione associata al comando di fuoco */
int Fire_direction(int ch, int last_dir)
{	if(ch=='f')
		return last_dir;
	else if(ch=='w')
		return KEY_UP;
	else if(ch=='a')
		return KEY_LEFT;
	else if(ch=='s')
		return KEY_DOWN;
	else if(ch=='d')
		return KEY_RIGHT;
	else
		return ' ';
}

/* Aggiorna i messaggi per informare il giocatore circa le sue munizioni,
	armi sparse, nemici in vita e moente restanti */
void Update_Message(int tot_weapon, int tot_enemy, int tot_coins, int drill_ammo, int ghost_walk, int color)
{	if(color==2)
		clear();
	else
	{	char *msg=NULL;
		msg = malloc(sizeof(char)*strlen("000 armi sparse.."));
		sprintf(msg, "%d armi sparse..", tot_weapon);
		show_message(3, 7, msg, 1);
		free(msg);
		msg = malloc(sizeof(char)*strlen("000 nemici in giro"));
		sprintf(msg, "%d nemici in giro", tot_enemy);
		show_message(3, 50, msg, 1);
		free(msg);
		msg = malloc(sizeof(char)*strlen("000 monete da raccogliere ("));
		sprintf(msg, "%d monete da raccogliere (", tot_coins);
		show_message(4, 4, msg, 1);
		show_message(4, 4+strlen(msg), " @", 6);
		show_message(4, 4+strlen(msg)+2, ")", 1);
		free(msg);
		msg = malloc(sizeof(char)*strlen("000 Colpi Drill ("));
		sprintf(msg, "%d Colpi Drill (", drill_ammo);
		show_message(5, 5, msg, 1);
		show_message(5, 5+strlen(msg), "<=", 6);
		show_message(5, 5+strlen(msg)+2, ")", 1);
		free(msg);
		msg = malloc(sizeof(char)*strlen("000 Passi fantasma ("));
		sprintf(msg, "%d Passi fantasma (", ghost_walk);
		show_message(5, 40, msg, 1);
		show_message(5, 40+strlen(msg), "\",", 6);
		show_message(5, 40+strlen(msg)+2, ")", 1);
		free(msg);
	}
	refresh();
}

/* Stampa a video le regole della difficoltà selezionata */
void Guard_Rules(int difficolta, int *min_delay, int *max_delay, int *tot_enemy, int max_enemy, int *ghost, int *drill_ammo, int *ghost_walk, int *default_trouble, int *far_off)
{	/*Icone*/
	show_message(1, 5, "Tu", 1);
	show_message(1, 15, "Guardie 00", 1);
	show_message(1, 30, "Monete", 1);
	show_message(1, 45, "Drill Ammo", 1);
	show_message(1, 65, "Ghost Walk", 1);
	show_message(1, 8, "o0", 3);
	show_message(1, 37, " @", 6);
	show_message(1, 56, "<=", 6);
	show_message(1, 76, "\",", 6);
	/*Scopo e oggetti*/
	show_message(2, 30, "Raccogli le monete sparse per vincere!", 1);
	show_message(3, 5, "Drill Ammo:", 3);
	show_message(3, 20, "Spara con 'f' o 'z' nell'ultima direzione presa o indirizza con 'w''a''s''d'.", 1);
	show_message(4, 24, "questi colpi distruggono il primo ostacolo che incontrano, sia esso un muro, una guardia o tu!", 1);
	show_message(6, 5, "Ghost Walk:", 3);
	show_message(6, 20, "Premi 'g' o 'x' per godere del vantaggio di poter attraversare muri, proiettili e persone..", 1);
	show_message(7, 24, "ma solo per un passo: una volta consumato tornerai nel tuo stato corporeo.", 1);
	if(difficolta==1)
	{	show_message(10, 25, "Difficoltà 1:", 1);
		show_message(10, 45, "\"Chi vive, quando vive, non si vede: vive\"", 3);
		show_message(12, 20, "L'antico potere dell' invisibilita degli spettri ti permette di non essere visto.", 1);
		show_message(13, 20, "Finché resterai in Ghost Mode le guardie non si accorgeranno della tua presenza!", 1);
		show_message(15, 30, "Inizi con un Drill Ammo e un Ghost Walk.", 1);
	}
	else if(difficolta==2)
	{	show_message(10, 25, "Difficoltà 2:", 1);
		//				Μ   η	   δ	   ὲ	  ν	  ἄ\u03B3 γ	    α	    ν. 	«Mēden agān»
		show_message(10, 45, "\"M\u03B7\u03B4\u1F72\u03BD \u03B1\u03B3\u03B1\u03BD\"	«Meden agan»", 3);
		show_message(12, 20, "Queste guardie incantate vedono oltre la semplice prigione di carne; fiutando", 1);
		show_message(13, 20, "l'odore della paura potrano percepire la tua presenza anche in Ghost Mode!", 1);
		show_message(15, 30, "Inizi solo con un Drill Ammo", 1);
	}
	else if(difficolta==3)
	{	show_message(10, 25, "Difficoltà 3:", 1);
		show_message(10, 45, "\"Sapere Aude\"", 3);
		show_message(12, 20, "Mosse da un sentimento comune le guardie collaboreranno", 1);
		show_message(13, 20, "per raggiungere il loro bersaglio!", 1);
		show_message(15, 30, "Non hai armi in partenza.", 1);
	}
	else if(difficolta==4)
	{	show_message(10, 25, "Difficoltà 4:", 1);
		show_message(10, 45, "\"Ad Maiora\"", 3);
		show_message(12, 20, "Risvegliate da un antico potere, queste guardie vedono più lontano di quelle", 1);
		show_message(13, 20, "che hai fin ora incontrato.", 1);
		show_message(14, 20, "Il loro raggio di visione passa da 2 a 3 (3 -> 5 per la vista frontale).", 1);
	}
	else
	{	show_message(10, 25, "Difficoltà 5:", 1);
		show_message(10, 45, "\"Wir sind gewohnt dass die Menschen verhöhnen was sie nicht verstehen\"", 3);
		show_message(12, 20, "Poteri più antichi dell' uomo pervadono queste mura: Ogni cella del labirinto", 1);
		show_message(13, 20, "contiene una moneta o un oggetto; L'aria, densa di un potere arcano, libera il vero", 1);
		show_message(14, 20, "potere del Ghost Walk: trasportandoti nel limbo fra questo mondo e quello dei morti.", 1);
		show_message(15, 20, "Nemmeno la vista incantata delle guardie può giungere fin lì.. ma nemmeno la tua può", 1);
		show_message(16, 20, "scorgere ciò che accade nella realtà terrena!", 1);
		show_message(17, 20, "Le guardie sincronizzano la loro strategia e avranno più probabilità di accerchiare il giocatore", 1);
		show_message(18, 30, "Potresti iniziare in Ghost Mode!", 1);
	}
	int key;
	show_message(21, 35, "PREMERE UN TASTO PER PROSEGUIRE", 3);
	show_message(23, 100, "premi ~ per opzioni avanzate", 1);
	do{	key = getch();
		if(key=='~')
			GG_Settings(difficolta, min_delay, max_delay, tot_enemy, max_enemy, ghost, drill_ammo, ghost_walk, default_trouble, far_off);
	}while(key==-1 || key=='~');
	clear();
}

/* Impostazioni di Delay per Guard Game */
void GG_Settings(int difficolta, int *min_delay, int *max_delay, int *tot_enemy, int max_enemy, int *ghost, int *drill_ammo, int *ghost_walk, int *default_trouble, int *far_off)
{	int ask;
	//Min Delay
	show_message(25, 15, "Premere ESC per mantenere il valore corrente!", 1);
	ask = Ask_Integer_Between(26, 2, "Reimpostare Minimo delay Guard Game (14-18)", 14, 18);
	if(ask!=UNDO)
		*(min_delay)=ask;
	//Max Delay
	ask = Ask_Integer_Between(27, 2, "Reimpostare Massimo delay Guard Game (19-25)", 19, 25);
	if(ask!=UNDO)
		*(max_delay)=ask;
	//Tot Enemy
	char *msg= malloc(sizeof(char)*strlen("Reimpostare numero di guardie (min 000; attualmente 000; massimo 000)"));
	sprintf(msg, "Reimpostare numero di guardie (min 1; attualmente %d; massimo %d)", *tot_enemy, max_enemy);
	ask = Ask_Integer_Between(28, 2, msg, 1, max_enemy);
	if(ask!=UNDO)
		*(tot_enemy)=ask;
	//ghost
	ask = Ask_Boolean(29, 2, "Partire in Ghost Mode (1-Si  0-No)", "Ci tengo alla mia forma", "I nostri corpi ci tradiscono");
	if(ask!=UNDO)
		*(ghost)=ask;
	//Dril Ammo iniziali
	ask = Ask_Integer(30, 2, "Drill Ammo iniziali?");
	if(ask!=UNDO)
		*(drill_ammo)=ask;
	//Ghost Walk iniziali
	ask = Ask_Integer(31, 2, "Ghost Walk iniziali?");
	if(ask!=UNDO)
		*(ghost_walk)=ask;
	//Sempre nei guai
	if(difficolta>2)
	{	ask = Ask_Boolean(32, 2, "Sempre inseguito?", "Ti prego, No!", "Chi me l'ha fatto fare?");
		if(ask!=UNDO)
			*(default_trouble)=ask;
	}
	//Distanza %
	ask = Ask_Integer_Between(33, 2, "Percentuale di distanziazione (1-100)", 1, 100);
	if(ask!=UNDO)
		*(far_off)=ask;
	show_message(25, 15, "Premere ESC per mantenere il valore corrente!", 2);
	show_message(26, 2, "Reimpostare Minimo delay Guard Game (14-18) 000", 2);
	show_message(27, 2, "Reimpostare Massimo delay Guard Game (19-25) 000", 2);
	show_message(28, 2, msg, 2);
	show_message(28, 2+strlen(msg), "  0000", 2);
	free(msg);
	show_message(29, 2, "Partire in Ghost Mode (1-Si  0-No) Ci tengo alla mia forma I nostri corpi ci tradiscono", 2);
	show_message(30, 2, "Drill Ammo iniziali?   0000", 2);
	show_message(31, 2, "Ghost Walk iniziali?   0000", 2);
	show_message(32, 2, "Sempre inseguito?   Chi me l'ha fatto fare?", 2);
	show_message(33, 2, "Percentuale di distanziazione (1-100)   000", 2);
	refresh();
}
