#ifndef ESA_H
#define ESA_H

#include "utils.h"
#include "heap.h"
#include "defines.h"

/**********************************************************************/
 
int esa_malloc(t_TEXT *Text);
int esa_open(t_TEXT *Text, char *ext);
int esa_close(t_TEXT *Text); 

/**********************************************************************/

void esa_seek(FILE *File, size_t pos);
void esa_read(t_ESA *ESA, FILE *File, size_t block_esa_size);

void esa_read_sa(unsigned *SA, size_t n, FILE *File);
void esa_read_gsa(t_GSA *GSA, size_t n, FILE *File);

int esa_read_all(t_TEXT *Text);

int esa_print(t_TEXT *Text, size_t size);
int esa_print_suff(int_suff *SA, int_lcp *LCP, t_TEXT *Text, unsigned size);
int esa_print_gsa(t_GSA *GSA, size_t size);

int esa_merge(t_TEXT *Text, int_text k, size_t *size, char* c_file, int_text total, int_suff* COUNT, size_t n);

/**********************************************************************/

void esa_write_induced(heap *h, heap_node *node, int8 alfa, int_lcp lcp);


#if _INDUCING
void induce(heap *h, heap_node *node, int_lcp lcp);
#endif

int esa_write_all(int_suff* SA, int_lcp* LCP, t_TEXT *Text, char *c_file);
int esa_build(t_TEXT *Text, int_text n, int sigma, char* c_file);

/**********************************************************************/


#endif
