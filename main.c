#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "presence.h"




int main()
{
	char today_date[25];
	char TODAY_PRESENCE_FILE_PATH[50];

	get_date(today_date);
	sprintf(TODAY_PRESENCE_FILE_PATH, "EXPORT/%s.csv", today_date);



	//view_student_list(TODAY_PRESENCE_FILE_PATH);
	//view_present_student(TODAY_PRESENCE_FILE_PATH);
	//enter_pc_state(TODAY_PRESENCE_FILE_PATH);
	//search_information(TODAY_PRESENCE_FILE_PATH);


	while(1)
		init_program();


	return 0;
}



