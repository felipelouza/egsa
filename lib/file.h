#ifndef FILE_H
#define FILE_H

#include<math.h>
#include<unistd.h>

#include "utils.h"
#include "defines.h"


/**********************************************************************/

int 		preprocessing(t_TEXT **Text, char *c_file, size_t mem_limit, int_text *k, int_suff *COUNT);

int 		file_close(FILE* f_in);
void		mkdir(char* c_file);

/**********************************************************************/

int			open_sequence(t_TEXT *Text, char *c_file);
void 		close_sequence(t_TEXT *Text);
void		seek_sequence(FILE *File, size_t pos);

void 	   	load_sequence(t_TEXT *Text);

int8 		read_sequence(t_TEXT *Text);

/**********************************************************************/

void		remove_file(char* c_file);
int 		file_chdir(char* dir);

#endif
