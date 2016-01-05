#ifndef FILE_H
#define FILE_H

#include<math.h>
#include<unistd.h>

#include "utils.h"
#include "defines.h"


/**********************************************************************/

int 		preprocessing(t_TEXT *Text, char *c_file, int_text k);

int 		file_open_input(t_TEXT **Text, char *c_file, int_text *n, int_text *r);
void		file_close(FILE* File);
void		mkdir(char* c_file);

/**********************************************************************/

int_text	file_get_size(char *c_file, int_text n);
int 		file_partition(FILE *f_in, int_text n, int_text r);

int		open_sequence(t_TEXT *Text, char *c_file);
void 		close_sequence(t_TEXT *Text);
void		seek_sequence(FILE *File, size_t pos);

int  	   	file_write_sequence(t_TEXT *Text, char* c_file);
void 	   	load_sequence(t_TEXT *Text);

int8 		read_sequence(t_TEXT *Text);

/**********************************************************************/

int 		file_open_fasta(t_TEXT *Text, char* c_file, size_t begin);
int 		file_load_fasta(t_TEXT *Text);

/**********************************************************************/

void		remove_file(char* c_file);
int8 		map(int8 c_aux);
int 		file_chdir(char* dir);

#endif
