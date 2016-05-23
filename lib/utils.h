#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "defines.h"

#define WORKSPACE 22 //Phase 1 requeries 21.0 n bytes

typedef unsigned int int_text;
typedef unsigned int int_suff; 	//-2^31 to 2^31
typedef unsigned int int_lcp;
typedef unsigned char int8; //0 to 2^8

#define LCP_MAX 	UINT_MAX //SHRT_MAX

/**********************************************************************/

#pragma pack(1)

typedef struct{
	
	int_suff	sa;
	int_lcp		lcp;
	
	#if _PREFIX_ASSY
		int8 		prefix[PREFIX_SIZE];
	#endif
	int8		bwt;
	
	int_text	text;
	int_suff	sa_prime;
	
} t_ESA;

/**********************************************************************/

typedef struct{

	int_text	text;
	int_suff	suff;
	int_lcp 	lcp;
	
	#if BWT
		int8		bwt;
	#endif
	
} t_GSA;

/**********************************************************************/

typedef struct{

	int_text	text;	
	int_lcp 	lcp;
	
} t_INDUCED;

/**********************************************************************/

typedef struct { //Text struct
	
	char 		*c_file; 		// associated filename	
	FILE 		*f_in; 			// pointer to the text file
	
	FILE 		*f_ESA;			// pointer to the ESA input file	
	t_ESA 		*ESA;
	
	int8 		*c_buffer;		// buffer used for I/O	-> prefix
	int8	 	*c_overflow;	// buffer used for I/O
	
	int_suff	length;			// length of partition	

	//heap sort
	int_text	key;			// indetifier of sequence
	unsigned	u_idx; 			
	
	int_lcp		i_height;
	int_lcp		i_loaded;
	
	int_text	n_strings;	// number of concatenated strings
	int_text	n_start;	// # of the first string in CAT string
		
	size_t		*pos;		// start pos of each string 
	size_t 		block_esa_size;
	
} t_TEXT;

#pragma pack()


/**********************************************************************/

void print1(int8* a, unsigned n, const char *comment);
void print4(unsigned* a, unsigned n, const char *comment);

double time_stop(time_t t_time, clock_t c_clock);

#define swap(a,b) do { typeof(a) aux_a_b = (a); (a) = (b); (b) = aux_a_b; } while (0)

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define mod_aux(a, b) (a % b)

void die(const char* where);
void dies(const char* where, char* format, ...);

int print_int(int* A, int n);
int min_range(int* A, int l, int r);

/**********************************************************************/


int check_sa(int_suff *SA, size_t length, t_TEXT *Text);
int check_lcp_array(int8 *t, int_suff *SA, size_t length, int_lcp *lcp);

int check_bwt(t_TEXT *Text, size_t length, int_text n, t_TEXT *t_Aux);
int check_is_permutation(t_TEXT *Text, size_t length, int_text n, t_TEXT *t_Aux);

int check_gsa(t_TEXT *Text, size_t length, int_text n, t_TEXT *t_Aux);
int check_gsa_lcp(t_TEXT *Text, size_t length, int_text n, t_TEXT *t_Aux);

void check(t_TEXT *Text, int_text n, char* c_file, int_text total);

/**********************************************************************/

#endif
