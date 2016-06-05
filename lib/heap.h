#ifndef HEAP_H
#define HEAP_H

#include "utils.h"
#include "defines.h"

#include <limits.h>
#include <assert.h>
#include <math.h>

//typedef struct heap_node heap_node;
typedef t_TEXT heap_node;

struct heap {
	
	heap_node** heap;
	int_lcp* 	lcp_son_dad;
	int_lcp* 	lcp_left_right;
	
	int	size;	
	FILE 		*f_out_ESA;	// pointer to the ESA output file
	FILE 		*f_out_BWT;	// pointer to the ESA output file
	
	FILE 		*f_in_ESA;	// pointer to the ESA input file
	FILE 		*f_in_seq;	// pointer to the sequence input file
	
	//inducao
	
	#if _INDUCING
		size_t		induced[SIGMA];
	#endif
	
	
	char		cSIGMA[SIGMA][FILE_NAME];
	FILE		*fSIGMA[SIGMA];//stores induced suffixes

	#if _OUTPUT_BUFFER
		t_GSA		*out_buffer;
		size_t	size_out_buffer;
	#endif

	#if _INDUCING
		#if INDUCED_BUFFER
			t_INDUCED	*induced_buffer[SIGMA];
			size_t	size_induced_buffer[SIGMA];
			size_t	inserted_induced_buffer[SIGMA];
		#endif //INDUCED_BUFFER
	
		//lcp
		int_lcp		lcp_induced[SIGMA];		
	#endif
	
	
};

typedef struct heap heap;


//heap* heap_alloc(int_text n);
heap* heap_alloc_induced(int k, char *c_file, int_text total, int_suff* COUNT, size_t n);
int heap_free(heap *h);

#define heap_size(h) ((h)->size)

//is_less
int is_less(heap *h, heap_node *node1, heap_node *node2);
int compare(heap *h, heap_node *node1, heap_node *node2, int_lcp* lcp);

int is_less_left_right(heap *h, heap_node *node1, heap_node *node2, int_lcp* lcp);
int is_less_down(heap *h, heap_node *node1, heap_node *node2, int_lcp* LCP, int_lcp lcp);

int heap_insert(heap *h, int key, heap_node* node, unsigned idx);
int heap_update(heap *h, int key, heap_node* node, unsigned idx);

int heap_delete_min(heap *h);
int heap_delete_min_last(heap *h);

int heap_pass_induced(heap *h, t_TEXT *Text, size_t *n, int8 alfa);
int heap_pass_induced_last(heap *h, t_TEXT *Text, size_t *pos, int8 alfa);

int load_buffer(heap *h, heap_node *node, int8** pt, int_lcp length);
int print_node(heap_node *node);

int heap_lcp(heap *h, int i_node);

#endif
