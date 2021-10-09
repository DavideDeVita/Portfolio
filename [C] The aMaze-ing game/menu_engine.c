#include <stdlib.h>
#include <curses.h>
#include <string.h> 
#include <time.h> 
#include <unistd.h> 
										// Davide De Vita
										// N86001908
#include "curse.h"

#define TAB_SIZE 5

int max_hor=30, max_ver=20, Haste=0;

/* Inserisce un Labirinto nella lista.
	Se si imposta Haste l'inserimento avverrà in testa, risparmiando tempo */
List_Maze Inserimento_Lista(List_Maze List, List_Maze Labirinto)
{	if(!Haste)
		List = Inserimento_fine_Lista(List, Labirinto);
	else
		List = Inserimento_testa_Lista(List, Labirinto);
return List;
}

/* Inverte il valore di Haste */
void Testa_o_Coda()
{	Haste = 1 - Haste;
}

/* Chiede all'utente un intero
	Premere INVIO senza alcun input restituisce 0 */
int Ask_Integer(int x, int y, char* msg)
{	int integer=0;
	int ch, i=0;
	show_message(x, y, msg, 3);
	move(x, y+strlen(msg)+2);
	do{	ch=getch();
		if((ch>=48 && ch<=57) || ch==DEL)
		{	if(ch>=48 && ch<=57)
			{	integer=(integer*10)+(ch-48);
				i++;
				attron(COLOR_PAIR(1));
				addch(ch);
				if(ch==48 && integer==0)
				{	i--;
					move(x, y+strlen(msg)+2+i);
				}
			}
			else if(ch == DEL)
			{	integer/=10;
				i--;
				if(i>=0)
				{	attron(COLOR_PAIR(2));
					move(x, y+strlen(msg)+2+i);
					addch(' ');
					move(x, y+strlen(msg)+2+i);
				}
			}
		}
	}while(ch!=ENTER && ch!=ESC);
	show_message(x, y, msg, 1);
	if(ch==ENTER)
		return integer;
	else
		return UNDO;
}

/* Chiede all'utente un intero tra min e max
	Premere INVIO senza alcun input restituisce 0 se possibile */
int Ask_Integer_Between(int x, int y, char* msg, int min, int max)
{	int ret=0;
	do{	ret=Ask_Integer(x, y, msg);
		if(ret<min || ret>max)
		{	show_message(x, y+strlen(msg), "       ", 2);
			if(ret!=UNDO)
				Error_Message("Valore fuori dal dominio");
			refresh();
		}
	}while( (ret<min || ret>max) && ret!=UNDO);
	return ret;
}

/* Chiede all' utente di selezionare una difficoltà fra 1 e 5 */
int Ask_Difficolta(int x, int y, int last)
{	show_message(x, y, "Difficoltà [1 -> 5]: ", 3);
	int i, ch;
	attron(COLOR_PAIR(1));
	for(i=1; i<=5; i++)
	{	if(last==i)
			attron(COLOR_PAIR(3));
		move(x+1, y+(2*i));
		addch(i-1+'1');
		if(last==i)
			attron(COLOR_PAIR(1));
	}
	refresh();
	i=last-1;
	Smile(x, y+strlen("Difficoltà [1 -> 5]: "), i);
	do{	ch=getch();
		if(Is_An_Option(ch))
		{	move(x+1, y+(2*(i+1)));
			addch(i+'1');
			attron(COLOR_PAIR(2));
			Smile(x, y+strlen("Difficoltà [1 -> 5]: "), i);
			if(ch==KEY_LEFT)
				i=(i+4)%5;
			else if(ch==KEY_RIGHT)
				i=(i+1)%5;
			else if(ch==KEY_HOME)
				i=0;
			else if(ch==KEY_END)
				i=4;
			else
				i=ch-'1';
			attron(COLOR_PAIR(3));
			move(x+1, y+(2*(i+1)));
			addch(i+'1');
			attron(COLOR_PAIR(1));
			Smile(x, y+strlen("Difficoltà [1 -> 5]: "), i);
		}
		
	}while(ch!=ESC && ch!=ENTER);
	if(ch==ENTER)
		return i+1;
	else
		return UNDO;
}

/* Verifica se l'input nella scelta della difficoltà è valido */
int Is_An_Option(int ch)
{	if(ch==KEY_LEFT || ch==KEY_RIGHT || ch==KEY_HOME || ch==KEY_END || (ch>'0' && ch<'6'))
		return 1;
	else
		return 0;
}

/* Mostra una simpatica faccina per descrivere la difficoltà scelta! */
void Smile(int x, int y, int d)
{	move(x, y);
	if(d==0)
		addstr("^_^");
	else if(d==1)
		addstr("u_u");
	else if(d==2)
		addstr("=_=");
	else if(d==3)
		addstr("o_O");
	else if(d==4)
		addstr("ç ç");
}

/* Chiede di scegliere fra due Opzioni */
int Ask_Boolean(int x, int y, char* msg, char* op_0, char *op_1)
{	int boolean=-1;
	int ch;
	char *max;
	if(strlen(op_1)>strlen(op_0))
		max=op_1;
	else
		max=op_0;
	show_message(x, y, msg, 3);
	do{	ch=getch();
		if(ch=='0' || ch=='1' || ch==DEL)
		{	attron(COLOR_PAIR(2));
			mvaddstr(x, y+strlen(msg)+2, max);
			if(ch=='0' || ch=='1')
			{	boolean = ch-48;
				attron(COLOR_PAIR(1));
				if(ch=='0')
					mvaddstr(x, y+strlen(msg)+2, op_0);
				else
					mvaddstr(x, y+strlen(msg)+2, op_1);
			}
			if(ch == DEL)
				boolean=-1;
		}
	}while( (ch!=ENTER || boolean==-1) && ch!=ESC); //Exit when ESC OR (ENTER AND !-1)
	show_message(x, y, msg, 1);
	if(ch==ENTER)
		return boolean;
	else
		return UNDO;
}

/* Chiede una stringa */
char* Ask_for_string(int x, int y, char *msg)
{	int buf_size=10, i=0, j;
	char *buffer=NULL;
	int c;
	buffer=(char *)malloc(sizeof(char)*buf_size);
	show_message(x, y, msg, 3);
	move(x, y+strlen(msg)+2);
	do{	c=getch();
		if( (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || (c=='.' || c=='_' || c=='/') )
		{	if(i == buf_size)
			{	buf_size*=2;
				buffer=ReCalloc(buffer, i, buf_size);
			}
			buffer[i++] = c;
			attron(COLOR_PAIR(1));
			addch(c);
		}
		else if(c==DEL)
		{	i--;
			if(i>=0)
			{	attron(COLOR_PAIR(2));
				move(x, y+strlen(msg)+2+i);
				addch(' ');
				move(x, y+strlen(msg)+2+i);
				attron(COLOR_PAIR(1));
			}
		}
	}while( (c!=ENTER || i==0) && c!=ESC);
	//Fine lettura
	if(c!=ESC)
	{	if(i<4)
		{	for (j=i; j<4; j++)
				buffer[j]='\0';
		}
		else
		{	if(i == buf_size)
			{	buf_size+=1;
				buffer=realloc(buffer, sizeof(char)*buf_size);
			}
			buffer[i]='\0';
		}
	}
	else
	{	free(buffer);
		return NULL;
	}
return buffer;
}

					/* Colors */
/* Chiede di scegliere un colore diverso da ex */
short choose_color(int x, int y, char *msg1, char* msg2, int ex)
{	char *Options[8] = {"Nero", "Rosso", "Verde", "Giallo", "Blu", "Magenta", "Cyan", "Bianco"};
	int i, ret=-1;
	char *str=NULL;
	show_message(x, y, msg1, 1);
	for(i=0; i<8; i++)
	{	if(i!=ex)
		{	str=malloc(sizeof(char)*(strlen(Options[i])+2));
			sprintf(str, "%d %s", i, Options[i]);
			show_message(x+i+1, y+2, str, 1);
			free(str);
			str=NULL;
		}
	}
	do{	i=getch();
		if(i>='0' && i<'8' && (i-'0')!=ex)
		{	if(ret>-1)
			{	str=malloc(sizeof(char)*(strlen(Options[ret])+strlen(msg2)+1));
				sprintf(str, "%s%s", msg2, Options[ret]);
				show_message(x+9, y, str, 2);
				free(str);
			}
			ret=i-'0';
			str=malloc(sizeof(char)*(strlen(Options[ret])+strlen(msg2)+1));
			sprintf(str, "%s%s", msg2, Options[ret]);
			show_message(x+9, y, str, 1);
			show_message(x+10, y+4, "Sconsigliato se si desidera giocare", (ret==0 || ret==7) ? 1 : 2);
			free(str);
		}
	}while( (i!=ENTER || (ret<0 || ret>7 || ret==ex)) && i!=ESC);
	if(i==ESC)
		return UNDO;
	else
		return (short)ret;
}

/* Genera automaticamente colori differenti per le celle visitate e i percorsi */
void Path_n_Visited_color(short wall, short way)
{	int b=-1, f=-1;
	while(b==-1 || b==wall || b==way)
		b=(rand()%6)+1;
	while(f==-1 || f==wall || f==way || f==b)
		f=(rand()%6)+1;
	init_pair(6, f, b);	/* Path */
	init_pair(7, f, f);	/* Visited */
}

/* Routine di inizializzazione della finestra per la scelta di una cella */
Location Ask_for_Location(MAZE Maze, Location Last, int remember)
{	WINDOW *Maze_Win;
	Location Loc=NULL;
	Maze_Win = Make_Maze_Win();
	Display_Maze(Maze_Win, Maze, 1);
	Loc = Select_Location(Maze_Win, Maze, Last, remember);
	wclear(Maze_Win);
	wrefresh(Maze_Win);
	delwin(Maze_Win);
	return Loc;
}

/* Permette all'utente di scegliere una cella del labirinto muovendosi su di esso */
Location Select_Location(WINDOW *win, MAZE Maze, Location Last, int memory)
{	Location L = NULL;
	int x, y, ch=-3;
	if(Last)
		x=Last->riga,	y=Last->colonna;
	else
		x=0,	y=0;
	int ex_x=x, ex_y=y;
	do{	/*selected one*/
		if(ch!=-1)
		{	wmove(win, window_row(x), window_column(y));
			wattron(win, COLOR_PAIR(3));
			if(Maze->map[x][y]==WAY)
			{	waddch(win, '-');
				waddch(win, '-');
			}
			else
			{	waddch(win, '<');
				waddch(win, '>');
			}
			wrefresh(win);
		}
		/*move or end*/
		ch = getch();
		ch=redirect(ch);
		switch (ch)
		{	case KEY_DOWN:
				x=(x+1)%Maze->rows;
				break;
			case KEY_UP:
				x=(x-1+Maze->rows)%Maze->rows;
				break;
			case KEY_LEFT:
				y=(y-1+Maze->columns)%Maze->columns;
				break;
			case KEY_RIGHT:
				y=(y+1)%Maze->columns;
				break;
			case KEY_HOME:
				y=0;
				break;
			case KEY_END:
				y=Maze->columns-1;
				break;
			case CTRL_HOME:
				x=0;
				y=0;
				break;
			case CTRL_END:
				x=Maze->rows-1;
				y=Maze->columns-1;
				break;
			case KEY_PPAGE:
				x=0;
				break;
			case KEY_NPAGE:
				x=Maze->rows-1;
				break;
			case '\t':
				if(y >= Maze->columns-TAB_SIZE)
					y=Maze->columns-1;
				else
					y+=TAB_SIZE;
				break;
			case KEY_BTAB:
				if(y<TAB_SIZE)
					y=0;
				else
					y-=TAB_SIZE;
				break;
		}
		/*clear the ex*/
		if(x!=ex_x || y!=ex_y)
		{	wmove(win, window_row(ex_x), window_column(ex_y));
			if(Last && memory && Last->riga==ex_x && Last->colonna==ex_y)
			{	wattron(win, COLOR_PAIR(1));
				//wattron(win, COLOR_PAIR(6));
				waddch(win, '>');
				waddch(win, '<');
			}
			else
			{	if(Maze->map[ex_x][ex_y]==WAY)
				wattron(win, COLOR_PAIR(4));
				else
					wattron(win, COLOR_PAIR(5));
				waddch(win, ' ');
				waddch(win, ' ');
			}
			ex_x=x;
			ex_y=y;
		}
	}while(ch!=ESC && ch!=ENTER && ch!=' ');
	if(ch==ENTER || ch==' ')
		L= New_Loc(x, y);
	return L;
}

/* Redirezione Standard dei tasti 'w''a''s''d' come frecce */
int redirect(int ch)
{	if(ch=='s' || ch=='S')
		return KEY_DOWN;
	else if(ch=='a' || ch=='A')
		return KEY_LEFT;
	else if(ch=='w' || ch=='W')
		return KEY_UP;
	else if(ch=='d' || ch=='D')
		return KEY_RIGHT;
	else
		return ch;
}

/* Utilissima funzione che prende in giro il giocatore se perde più volte consecutive! */
void Kid_on_me(int loss)
{	if(loss==1)
		Sentence_Quote("You look frustrated about something.\nguess i'm pretty good at my job, huh?");
	else if(loss==2)
		Sentence_Quote("Hmm. that expression...\nthat's the expression of someone who's died twice in a row.\nsuffice to say, you look really... unsatisfied.\nall right.\nhow 'bout we make it a third?");
	else if(loss==3)
		Sentence_Quote("hmm. that expression...\nthat's the expression of someone who's died thrice in a row.\n...\nhey, what comes after \"thrice\", anyway?\nwanna help me find out?");
	else if(loss==4)
		Sentence_Quote("hmm. that expression...\nthat's the expression of someone who's died quice in a row.\nquice? frice?\nwelp, won't have to use it again anyways.");
	else if(loss==5)
		Sentence_Quote("hmm. that expression...\nthat's the expression of someone who's died five times in a row.\nconvenient, huh? that's one for each finger.\nbut soon...\nyou'll need a cool mutant hand to count all of your deaths.");
	else if(loss==6)
		Sentence_Quote("hmm. that expression...\nthat's the expression of someone who's died six times in a row.\nthat's the number of fingers on a mutant hand.\nbut soon...\nyou'll need to find a mutant hand with even more fingers.");
	else if(loss==7)
		Sentence_Quote("hmm. that expression...\nthat's the expression of someone who's died seven times in a row.\nhey, that's good. seven's supposed to be a lucky number.\nwho knows, maybe you'll hit the jackpot...\nand that number will multiply tenfold.");
	else if(loss==8)
		Sentence_Quote("hmm. that expression...\nthat's the expression of someone who's died eight times in a row.\nthat's the number of fingers on a spider.\nbut soon...\nwait, don't spiders have legs?");
	else if(loss==9)
		Sentence_Quote("hmm. that expression...\nthat's the expression of someone who's died seven times in a row.\nnope, wait, that's definitely nine, sorry.\nor was it ten?");
	else if(loss==10)
		Sentence_Quote("hmm. that expression...\nthat's the expression of someone who's died ten times in a row.\nhey, congrats! the big one-oh!\nenjoy your party 'cause is not gonna last any longer");
	else if(loss==11)
		Sentence_Quote("hmm. that expression...\nthat's the expression of someone who's died eleven times in a row.\nwell, give or take.\nthere's nuance to this stuff.\ndon't think i'll be able to count very well from here.\ncount for me, ok?\nwe'll start from 12.");
	else if(loss==18)
		Sentence_Quote("hmm. that might be a beginning...\nbut i can do better");
	else if(loss==24)
		Sentence_Quote("hmm. this is a story I've already heard\nshall we go further?");
	else if(loss==30)
		Sentence_Quote("hmm. this is a good point to stop\n Isn't it?");
	else if(loss>18 && loss<30)
	{	show_message(5, 40, "Questa supplica quasi intenerisce un cuore che non possiedo", 8);
		refresh();
	}
	else
		Sentence_Quote("let's just get to the point.");
}

/* Stampa indentata in diagonale della quote */
void Sentence_Quote(const char* quote)
{	int i, x=5, y=40;
	attron(COLOR_PAIR(9));
	move(x, y);
	for(i=0; i<strlen(quote); i++)
	{	if(quote[i]!='\n')
			addch(quote[i]);
		else
		{	x++;
			y+=5;
			move(x, y);
		}
	}
}
/* Other Logic */
/* Crea una WINDOW per la visualizzazione di un Labirinto.
	Questa viene creata di dimensione adatta a contenere il più grande labirinto nella Lista */
WINDOW *Make_Maze_Win()
{	WINDOW *Maze_Win;
	Maze_Win = newwin(max_ver+2, (2*max_hor)+4, 5, MAIN_LIMIT+10);
	nodelay(Maze_Win ,TRUE); /* Keyboard reads will be non-blocking in window */
	keypad(Maze_Win, TRUE); /* Allow control keys to be read (i.e., function and arrow keys) */
	return Maze_Win;
}

/* Calcola la massima altezza e larghezza dei labirinti contenuti nella Lista */
void Update_limits(List_Maze List)
{	List_Maze nodo=List;
	max_ver=20,	max_hor=30;
	while(nodo)
	{	if(nodo->Maze->rows>max_ver)
			max_ver = nodo->Maze->rows;
		if(nodo->Maze->columns>max_hor)
			max_hor = nodo->Maze->columns;
		nodo=nodo->link;
	}
}

/* Controlla se è necessario aggiornare i limiti rispetto a due dimensioni */
void May_Update_limits(int rows, int cols)
{	if(rows>max_ver)
		max_ver=rows;
	if(cols>max_hor)
		max_hor=cols;
}

/* Stampa un Labirinto e attende la ricezione di un tasto per continuare */
void Show_n_Wait(WINDOW *Maze_Win, MAZE Maze, Location Start, Location Destination)
{	int ch;
	Display_Maze(Maze_Win, Maze, 1);
	if(Start)
	{	wmove(Maze_Win, window_row(Start->riga), window_column(Start->colonna));
		wattron(Maze_Win, COLOR_PAIR(3));
		waddstr(Maze_Win, "<>");
		/*if Start then Destination*/
	}
	if(Destination)
	{	wmove(Maze_Win, window_row(Destination->riga), window_column(Destination->colonna));
		wattron(Maze_Win, COLOR_PAIR(1));
		waddstr(Maze_Win, "><");
	}
	wrefresh(Maze_Win);
	show_message(3, MAIN_LIMIT+2, "Premere INVIO, ESC o SPAZIO per tornare al Menù", 1);
	refresh();
	do{	ch=getch();
	}while(ch!=ESC && ch!=ENTER && ch!=' ');
	show_message(3, MAIN_LIMIT+2, "Premere INVIO, ESC o SPAZIO per tornare al Menù", 2);
	wclear(Maze_Win);
	wrefresh(Maze_Win);
	delwin(Maze_Win);
}

/* Pathfinder */
/* A seguito di una visita Stampa il numero di iterazioni fatte, la lunghezza del percorso cercato
	e mostra il percorso */
void Print_Path(WINDOW *Maze_Win, MAZE Maze, Map_info info, Adj_List Path, Location da, Location a)
{	Display_Maze(Maze_Win, Maze, 1);
	char *str=NULL, *steps=malloc(sizeof(char)*strlen("Numero di iterazioni: 00\n"));
	sprintf(steps, "Numero di iterazioni: %d", info.steps);
	//int character_dir=5; //5 STAY 	8 UP	 6 LEFT	2 DOWN	4 RIGHT	/* Removed */
	if(Path)
	{	str = malloc(sizeof(char)*(strlen("Lunghezza del percorso da [00,00] a [00,00] = 00")));
		sprintf(str, "Lunghezza del percorso da [%d,%d] a [%d,%d] = %d", da->riga, da->colonna, a->riga, a->colonna, info.dist[a->riga][a->colonna]);
	}
	else if(Exists_Location(Maze, da)!=1 || Exists_Location(Maze, a)!=1)
		str = Unexisting_Location_path(Maze, da, a);
	else
	{	str = malloc(sizeof(char)*(strlen("Non è stato trovato alcun percorso da [00,00] a [00,00]")));
		sprintf(str, "Non è stato trovato alcun percorso da [%d,%d] a [%d,%d]", da->riga, da->colonna, a->riga, a->colonna);
	}
	show_message(1, MAIN_LIMIT+2, str, 1);
	show_message(2, MAIN_LIMIT+15, steps, 1);
	free(str);
	free(steps);
	Display_Path(Maze_Win, Maze, Path, 3, MAIN_LIMIT+2, 15);
	wclear(Maze_Win);
	wrefresh(Maze_Win);
	delwin(Maze_Win);
}
