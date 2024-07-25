#define MAXNAME 80
#define WIDTH 60
#define HEIGHT 20






void init_program();

void init_own_color();








/* The main struct for all student from the list */
typedef struct
{
		char name[MAXNAME];
		char time[sizeof "JJ/MM/AAAA HH:MM:SS"];
		short get_machine;
}student;
student *allocate(int size); 


/* For the get machine state */
enum GET_MACHINE {NO=1, YES, PERS };	/* NO = 1, YES = 2, PERS = 3 */


/* 4 main functions */
void view_present_student(char *TODAY_PRESENCE_FILE_PATH);
void view_student_list(char *TODAY_PRESENCE_FILE_PATH);
void enter_pc_state(char *TODAY_PRESENCE_FILE_PATH);
void search_information(char *TODAY_PRESENCE_FILE_PATH);


/* File actions */
int line_number(FILE *file);
FILE *open_file(char *path, char* mode);
FILE *open_present_or_base_file(char *path, char* mode);
void read_lines(FILE *file, student *sdt);
void write_lines(student *sdt, int nbr_lines, char *TODAY_PRESENCE_FILE_PATH);




/*Get Objects*/

void get_name(char line[], char name[]);
void get_data(student *sdt, int nbr_lines, char *TODAY_PRESENCE_FILE_PATH);
void get_time(char *sdt_date);
void get_date(char *date);
void get_present (char line[], char get[]);
void get_pc_state(char line[], char pc_state[]);



void toLowerCase(char *c);
int validate_date_format(char *date);






void switch_choices(int choice);
void print_menu(WINDOW *menu_win, int highlight);
void write_lines_of_html(student *sdt, int nbr_lines );
