#include "maze.h"
									//	Davide De Vita
									//	N 86/1908
int EXIT = 0;

//Parsing methods
/* Salvataggio su File */
void Save_Maze(char *filename, MAZE Maze)
{	FILE *fd = fopen(filename, "w");
	if(!fd)
	{	Error_Message("Invalid file\n");
		return;
	}
	//implicit else
	int i,j;
	fprintf(fd, "[%d-%d];\n", Maze->rows, Maze->columns);
	for(i=0; i<Maze->rows; i++)
	{	for(j=0; j<Maze->columns; j++)
		{	fprintf(fd, "%c", Maze->map[i][j]);
		}
		fprintf(fd, "\n");
	}
	fprintf(fd, ".");
	fclose(fd);
}

/* Lettura da File */
MAZE Load_Maze(char *filename)
{	FILE *fd = fopen(filename, "r");
	if(!fd)
	{	Error_Message("Invalid file\n");
		return NULL;
	}
	MAZE Maze = Read_Parameters(fd);
	if(Maze!=NULL)
	{	int i, j;
		char c='\0';
		for(i=0; i<Maze->rows; i++)
		{	for(j=0; j<Maze->columns; j++)
			{	c=fgetc(fd);
				Maze->map[i][j]=c;
			}
			c=fgetc(fd);
			if(c!='\n'){
				Invalid_format("Expected endline\n", Maze);
				if(EXIT) return NULL;
			}
		}
		c=fgetc(fd);
		if(c!='.'){
			Invalid_format("Expected dot as ending\n", Maze);
			if(EXIT) return NULL;
		}
	}
	return Maze;
}

/* Parametri [righe-colonne];\n 
		Restiturisce un labirinto con le seguenti caratteristiche */
MAZE Read_Parameters(FILE* fd)
{	char c;
	int rows=0, cols=0;
	Ignore_spaces(fd);
	c = fgetc(fd);
	if (c!='[')
	{	Invalid_format("Expected '['\n", NULL);
		if(EXIT) return NULL;
	}
	c = fgetc(fd);
	if(!is_digit(c))
	{	Invalid_format("Expected digit\n", NULL);
		if(EXIT) return NULL;
	}
	else
	{	Step_back(fd);
		rows = Read_num(fd);
		Ignore_spaces(fd);
		c = getc(fd);
		if (c!='-')
		{	Invalid_format("Expected '-'\n", NULL);
			if(EXIT) return NULL;
		}
		c = fgetc(fd);
		if(!is_digit(c))
		{	Invalid_format("Expected digit\n", NULL);
			if(EXIT) return NULL;
		}
		Step_back(fd);
		cols = Read_num(fd);
		Ignore_spaces(fd);
		c = getc(fd);
		if (c!=']')
		{	Invalid_format("Expected ']'\n", NULL);
			if(EXIT) return NULL;
		}
		Ignore_spaces(fd);
		c = getc(fd);
		if (c!=';')
		{	Invalid_format("Expected ';'\n", NULL);
			if(EXIT) return NULL;
		}
		c = getc(fd);
		if (c!='\n')
		{	Invalid_format("Expected endline\n", NULL);
			if(EXIT) return NULL;
		}
	}
	return Alloc_Maze(rows, cols, WALL);
}

void Step_back(FILE *fd)
{	fseek(fd, -1, SEEK_CUR);
}

/* Salta ogni occorrenza di SPAZIO TAB e INVIO */
void Ignore_spaces(FILE *fd)
{	char c;
	do	c = fgetc(fd);
	while(c==' ' || c == '\n' || c=='\t');
	Step_back(fd);
}

/* Restituisce 1 se c è un carattere numerico */
int is_digit(char c)
{	if(c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9')
		return 1;
	else
		return 0;
}

/* Legge e ritorna un valore numerico */
int Read_num(FILE *fd)
{	int cifra, ret=0;
	char c;
	c = fgetc(fd);
	while(is_digit(c))
	{	cifra = ctoi(c);
		ret = (ret*10)+cifra;
		c = fgetc(fd);
	}
	Step_back(fd);
	return ret;
}

/* da char a int */
int ctoi(char c)
{	if('0'<=c && c<='9')
		return c-48;
	else
		return -1;
}

/* Restituisce il corretto messaggio d 'errore riscontrato liberando la memoria occupata */
void Invalid_format(char *error_msg, MAZE free_me)
{	Error_Message(error_msg);
	if(free_me!=NULL)
		Free_Maze(free_me);
	//exit(-1);
	EXIT = 1;
}
