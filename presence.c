#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include "presence.h"




int startx = 0;
int starty = 0;

char *choices[] = {
	"View student list",
	"View present student",
	"Search information about a student",
	"Enter PC state for a student",
	"Exit"
};

int n_choices = sizeof(choices) / sizeof(char *);


void init_program()
{
	WINDOW *menu_win;
		int highlight = 1;
		int choice = 0;
		int c;
	extern int n_choices;
	extern int startx, starty;
	
		initscr();
		clear();
		noecho();
		cbreak();
		startx = (COLS - WIDTH) / 2;
		starty = (LINES - HEIGHT) / 2;

		init_own_color();

		bkgd(COLOR_PAIR(1));
		menu_win = newwin(HEIGHT, WIDTH, starty, startx);
		keypad(menu_win, TRUE);
		refresh();
		print_menu(menu_win, highlight);
		while(1){
			c = wgetch(menu_win);
			switch(c)
			{
				case KEY_UP:
					if (highlight == 1)
						highlight = n_choices;
					else
						--highlight;
					break;
				case KEY_DOWN:
					if (highlight == n_choices)
						highlight = 1;
					else 
						++highlight;
					break;
				case 10:
					choice = highlight;
					break;
				default:
					refresh();
					break;
			}
			print_menu(menu_win, highlight);
			if (choice != 0)
				break;
		}
		clrtoeol();
		refresh();
		endwin();
		
		switch_choices(choice);
}


/* For ncurses */
void switch_choices(int choice)
{
	system("clear");
	char today_date[25];
	char TODAY_PRESENCE_FILE_PATH[50];

	get_date(today_date);
	sprintf(TODAY_PRESENCE_FILE_PATH, "EXPORT/%s.csv", today_date);
	
	switch(choice)
	{
		case 1:
			view_student_list(TODAY_PRESENCE_FILE_PATH);
			break;
		case 2:
			view_present_student(TODAY_PRESENCE_FILE_PATH);
			break;
		case 3:
			search_information(TODAY_PRESENCE_FILE_PATH);
			break;
		
		case 4:
			enter_pc_state(TODAY_PRESENCE_FILE_PATH);
			break;
		case 5:
			exit(1);
			break;
		
	}
}

void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;

	x = 2;
	y = 2;
	
	box(menu_win, 0, 0);
	for(i = 0; i < n_choices; ++i){
		if (highlight == i + 1)
		{
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}

void init_own_color()
{
	start_color();
	if (has_colors() == FALSE)
	{
		endwin();
		printf("Erreur couleur");
		exit(1);
	}
	init_pair(1, COLOR_MAGENTA, COLOR_GREEN);

	short color_gray = 8;
	init_color(color_gray, 250, 250, 250);

	init_pair(2, COLOR_WHITE, color_gray);
}









/* The main struct for all student from the list */
student *allocate(int size)
{
	student *sdt = NULL;

	sdt = (student *)malloc(size * sizeof(student));
	if(!sdt){
		return NULL;
	}
	
	return sdt;
}

/* 4 main functions */
void  enter_pc_state (char *TODAY_PRESENCE_FILE_PATH)
{
	FILE *file = NULL;
	student *sdt = NULL;

	file = open_present_or_base_file(TODAY_PRESENCE_FILE_PATH, "r+");

	sdt = allocate(line_number(file));

	read_lines(file, sdt);
	get_data(sdt, line_number(file), TODAY_PRESENCE_FILE_PATH);	 
	
	fclose(file);
}

void view_student_list (char *TODAY_PRESENCE_FILE_PATH)
{
	initscr();
	clear();
	keypad(stdscr, TRUE);
	bkgd(COLOR_PAIR(2));

	int i;
	FILE *file = NULL;	
	student *sdt;
	
	file = open_file(TODAY_PRESENCE_FILE_PATH, "r+");
	sdt = allocate(line_number(file));
	
	printw("\n\tSTUDENT LIST : \n");
	read_lines(file, sdt);
	
	i = 0;
	while (i < line_number(file)){
		printw("\t\t%-*d %-*s\n", 4, i + 1, 50, sdt[i].name);
		++i;
	}
	
	fclose(file);

	printw("\n\n ==> Type a key to exit ...");
	refresh();
	getch();
	endwin();
}
void view_present_student (char *TODAY_PRESENCE_FILE_PATH)
{
	initscr();
	clear();
	echo();
	bkgd(COLOR_PAIR(2));

	FILE *file = NULL;
	int i = 0;
	char line[MAXNAME];
	char other_date[25];
	char other_file_path[50];

	printw("\n\n\tEnter the date (format : {YYYY-MM-DD} ) : ");

	scanw("%s", other_date);
	while(validate_date_format(other_date) != 1)
	{
		printw("\tERROR !! \n\t\tVerify the date format {YYYY-MM-DD}  : ");
		scanw("%s", other_date);
	} 
	sprintf(other_file_path, "EXPORT/%s.csv", other_date);

	file = open_file(other_file_path, "r+");

	clear();
	printw("\n\n\tSTUDENT PRESENT LIST (%s) : \n", other_date);

	while (fgets(line, MAXNAME - 1, file) != NULL){
		char name[MAXNAME] = "";
		char student_present[25] = "";
		char student_pc_state[25] = "";

		i++;
		get_name(line, name);
		get_present(line, student_present);
		get_pc_state(line, student_pc_state);
		
		if (!strstr(student_present, "ABSENT"))
			printw("\t\t%-*s ; %-*d %-*s ==> Present (%s)\n", 10, student_present, 3, i,50, name, student_pc_state);

	}
	
	fclose(file);
	
	printw("\n\n ==> Type a key to exit ...");

	refresh();
	getch();
	endwin();
}

void search_information (char *TODAY_PRESENCE_FILE_PATH)
{
	initscr();
	clear();
	echo();
	keypad(stdscr, TRUE);
	bkgd(COLOR_PAIR(2));


	char name[256];

	FILE *file = NULL;
	file = open_file(TODAY_PRESENCE_FILE_PATH, "r+");


	printw("\n\n\tWrite the name of student : ");

	getstr(name);

	int i = 0;
	char line[MAXNAME];
	char line_original[MAXNAME];
	while (fgets(line, MAXNAME - 1, file) != NULL)
	{
		toLowerCase(name);
		strcpy(line_original, line);
		toLowerCase(line);
		if (strstr(line, name))
		{
			printw("\t\t%s \n", line_original);
		}
		++i;
	}
	
	rewind(file);
	fclose(file);

	printw("\n\n ==> Type a key to exit ...");
	refresh();
	getch();
	endwin();
}


/* File actions */
void write_lines(student *sdt, int nbr_lines, char *TODAY_PRESENCE_FILE_PATH)
{
	
	FILE *fd = NULL;
	fd = open_file(TODAY_PRESENCE_FILE_PATH, "w+");
	int i=0;
	
	for (i=0; i<nbr_lines; i++)
	{
		if (sdt[i].get_machine == NO)
			fprintf(fd, "%s%s;pc : NON ;\n", sdt[i].time,sdt[i].name); 
		else if (sdt[i].get_machine == YES)
			fprintf(fd, "%s%s;pc : OUI ; \n", sdt[i].time,sdt[i].name); 
		else if (sdt[i].get_machine == PERS)
			fprintf(fd, "%s%s;pc : PERSO ;\n", sdt[i].time,sdt[i].name);
		else
			fprintf(fd, "ABSENT !!%s ;;\n", sdt[i].name);
	}
	
	
	fclose(fd);
}

void read_lines(FILE *file, student *sdt)
{
	int i = 0;
	char line[MAXNAME];
	while (fgets(line, MAXNAME - 1, file) != NULL){
		char student_pc_state[25] = "";
		char student_present[25] = "";

		get_name(line, sdt[i].name);
		get_pc_state(line, student_pc_state);
		get_present(line, student_present);

		if (strlen(student_pc_state) > 2)
		{
			if (strstr(student_pc_state, "OUI"))
				sdt[i].get_machine = YES;
			else if (strstr(student_pc_state, "NO"))
				sdt[i].get_machine = NO;
			else if (strstr(student_pc_state, "PERSO"))
				sdt[i].get_machine = PERS;
			strcpy(sdt[i].time, student_present);
		}
		else
			sdt[i].get_machine = 0;
		++i;
	}
	rewind(file);

}

int line_number(FILE *file)
{
	int c, line;	
	
	line = 0;
	while((c = fgetc(file)) != EOF){
		if(c == '\n')
			++line;
	}
	
	rewind(file);
	return line;
}

FILE *open_file(char *path, char* mode)
{
    FILE *file = NULL;
    
    file = fopen(path, mode);
    if (file == NULL)
    {
        printf("File '%s' not found\n\n====> Exiting\n\n", path);			
		exit(1);
	}
    return file;
}

FILE *open_present_or_base_file(char *path, char* mode)
{
    FILE *file = NULL;
    
    file = fopen(path, mode);
    if (file == NULL)
    {
		file = open_file("BASE.csv", "r+");
		if (file == NULL)
		{
			printf("File Opening error (present or base file)");
			exit(1);
		}
	}
    return file;
}

/*Get Objects*/
void get_data(student *sdt, int nbr_lines, char *TODAY_PRESENCE_FILE_PATH)
{
	int num;
	char machine;
	char PC[5] ;
	
	initscr();
	echo();
	bkgd(COLOR_PAIR(2));

	printw(" \n");

	while (1){
			char c;

			printw(" \tStudent n° and PC : (numero Y/N/P)  :  ");
			scanw("%d %s", &num, &machine);

			if (machine == 'x') break;
			
			while((machine != 'P' && machine != 'N' && machine != 'Y') || (num > nbr_lines || num <= 0) ){
				printw("\t\tERREUR ::  Student n° and PC : (numero Y/N/P)  :  ");
				scanw("%d %s", &num, &machine);
			}
			
			if (sdt[num-1].get_machine != 0)
					c = 'y';
					
			if (c == 'y'){
				if (sdt[num-1].get_machine==NO)
					strcpy(PC,"NO");
				if (sdt[num-1].get_machine==YES)
					strcpy(PC,"YES");
				if (sdt[num-1].get_machine==PERS)
					strcpy(PC,"PERS");
			}

			switch(machine){
				case 'Y':
					sdt[num-1].get_machine = YES;
					break;
				case 'N':
					sdt[num-1].get_machine = NO;
					break;
				case 'P':
					sdt[num-1].get_machine = PERS;
				break;
			}

		get_time(sdt[num-1].time);

		// if already defined
		if (c == 'y'){	
			printw("\t\t%d %s => %s already defined\n\t\t\tRetry ? y/n : ", num, sdt[num-1].name, PC);
			scanw("%s", &c);
			if (c == 'y')
			{
				write_lines(sdt, nbr_lines, TODAY_PRESENCE_FILE_PATH);
			    write_lines_of_html(sdt, nbr_lines );
			}
			c = 'n';
		}
		else {
			printw("\t\t%-*d %s => defined to %s \n",4, num, sdt[num-1].name, (machine=='Y')?"YES":((machine=='N')?"NO":"PERSO"));
			write_lines(sdt, nbr_lines, TODAY_PRESENCE_FILE_PATH);
			write_lines_of_html(sdt, nbr_lines );
		}
		
	}
	endwin();
}

void get_name(char line[], char name[])
{
	int i, j;
	int comma = 0;
	
	for(j = 0, i = 0; i < strlen(line); i++, j++){		
		if (line[i] ==';'){	
			comma++;
			j = 0;
		}
	
		if (comma == 1 )
			name[j] = line[i];	
	}
}

void get_time(char *sdt_time)
{
	system("date +\%R > time.txt");
	FILE *time_file = open_file("time.txt", "r");
	fscanf(time_file, "%s", sdt_time);
	fclose(time_file);
	system("rm time.txt");
}

void get_date(char *date)
{
	system("date +\%F > date.txt");
	FILE *date_file = open_file("date.txt", "r");
	fscanf(date_file, "%s", date);
	fclose(date_file);
	system("rm date.txt");
}

void get_present (char line[], char get[])
{
	int i;
	int comma = 0;
	
	for(i = 0; line[i] != ';'; i++){			
		if (comma == 0 ){
			get[i] = line[i];
			get[i+1] = '\0';
		}
	}
}
void get_pc_state(char line[], char pc_state[])
{
	int i, j;
	int comma = 0;
	
	for(j = 0, i = 0; line[i] != '\n'; i++, j++){		
		if (comma == 2)
			pc_state[j] = line[i];	
		if (line[i] ==';'){	
			comma++;
			j = -1;
		}
	}
}





void toLowerCase(char *c)
{
	while(*c)
	{
		*c = tolower((unsigned char)*c);
		c++;
	}
}

int validate_date_format(char *date)
{
	if (strlen(date) != 10)
		return 0;

	int i=0;
	int counter=0;
	for (i=0; i<strlen(date); i++)
	{
		if (date[i] == '-')
		{
			if (i != 4 && i != 7)
				return 0;
			counter++;
		}
	}

	if (counter != 2)
		return 0;

	return 1;
}

void write_lines_of_html(student *sdt, int nbr_lines )
{
	
	FILE *fd = NULL;
	char *DATE = "100000";
	/*get_date(DATE);
	strcat(DATE,".html");*/
	fd = open_file("date.html","w+");
	int i; 
	fprintf(fd,"<HTML>\n");
    fprintf(fd,"\t<HEAD>\n");
    fprintf(fd,"\t\t<TITLE> PRESENCE  %s </TITLE>\n",DATE);
    fprintf(fd,"\t</HEAD>\n");
    fprintf(fd,"\t<BODY>\n");
	fprintf(fd, "\t\t<TABLE border=1>\n");
	fprintf(fd, "<TR><TD>HEURE</TD> <TD> Nom et prenoms </TD><TD> PC </TD></TR>\n");
	for (i=0; i<nbr_lines; i++)
	{
		if (sdt[i].get_machine == NO){
			fprintf(fd, "<TR><TD>%d</TD><TD bgcolor=\"red\"> %s</TD> <TD> %s </TD><TD> NON ;</TD></TR>\n",i+1, sdt[i].time,sdt[i].name); 
		}
		else if (sdt[i].get_machine == YES){
			fprintf(fd, "<TR><TD>%d</TD><TD bgcolor=\"green\"> %s</TD> <TD> %s </TD><TD>OUI ;</TD></TR>\n",i+1, sdt[i].time,sdt[i].name); 
		}
		else if (sdt[i].get_machine == PERS){
			fprintf(fd, "<TR><TD>%d</TD><TD bgcolor=\"blue\"> %s</TD> <TD> %s </TD><TD> PERS ;</TD></TR>\n", i+1,sdt[i].time,sdt[i].name);
		}
		else{
			fprintf(fd, "<TR><TD>%d</TD><TD bgcolor=\"yellow\">ABSENT !!</TD> <TD> %s </TD><TD> PERS ;</TD></TR>\n",i+1,sdt[i].name);
			//fprintf(fd, "ABSENT !!%s ;;\n", sdt[i].name);
		}
	}
	fprintf(fd, "\t\t</TABLE>\n");
	fprintf(fd,"\t</BODY>\n");
	fprintf(fd,"</HTML>");
	fclose(fd);
}
