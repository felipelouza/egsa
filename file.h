#ifndef FILE_H
#define FILE_H

#include<math.h>

#include "utils.h"
#include "defines.h"

/**********************************************************************/

int preprocessing(t_TEXT *Text, char* c_dir, char *c_file, int_text k);

int			file_open_input(t_TEXT **Text, char *c_dir, char *c_file, int_text *n, int_text *r);
void 		close(FILE* File);
inline void mkdir(char* c_file);

/**********************************************************************/

int_text	file_get_size(char *c_dir, char *c_file, int_text n);
int 		file_partition(char *c_dir, FILE *f_in, int_text n, int_text r);

int 		write_file(t_TEXT *Text, char *c_dir, char* c_file, int n);
int 		read_file(t_TEXT *Text, char *c_dir, char* cFile, int_text n) ;

int 		open_sequence(t_TEXT *Text, char *c_dir, char *c_file);
inline void close_sequence(t_TEXT *Text);
inline void seek_sequence(FILE *File, size_t pos);

int			file_write_sequence(t_TEXT *Text, char *c_dir, char* c_file);
void 		load_sequence(t_TEXT *Text);

int8 		read_sequence(t_TEXT *Text);

/**********************************************************************/

int 		file_open_fasta(t_TEXT *Text, char* c_file, size_t begin);
int 		file_load_fasta(t_TEXT *Text);

/**********************************************************************/

inline void	remove_file(char* c_file);

int8 map(int8 c_aux);

#endif
