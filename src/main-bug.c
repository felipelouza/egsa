/***********************************************************************

eGSA.v.2.0: External Generalized Suffix Array Construction Algorithm 
Louza, abr 2015.

Copyright (C) 2015 Felipe Louza <louza@ic.unicamp.br>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

***********************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "utils.h"
#include "file.h"
#include "esa.h"
#include "defines.h"

/**********************************************************************/

void print_config(void);

t_TEXT *Text;
size_t size; //sizeof generalized suffix array

time_t t_start, t_total;
clock_t c_start, c_total;

	
int main(int argc, char **argv) {
	
	unsigned PRE_OPTION, VALIDATE;
	int_text k, total;
	size_t MEMLIMIT;

	sscanf(argv[1], "%u", &PRE_OPTION);
	//argv[2] = DIR
	//argv[3] = DATABASE
	sscanf(argv[4], "%d", &k);//NUMBER OF STRINGS
	sscanf(argv[5], "%u", &MEMLIMIT);//AVAILABLE MEMORY
	sscanf(argv[6], "%u", &VALIDATE);

	printf("SIGMA = %d\n", SIGMA);
	printf("DIR = %s\n", argv[2]);
	printf("INPUT = %s\n", argv[3]);
	printf("K = %d\n", k);
	MEMLIMIT *= 1024*1024;
	printf("MEMLIMIT = %llu bytes\n", MEMLIMIT);
	printf("PRE = %d\n", PRE_OPTION);
	printf("CHECK = %d\n", VALIDATE);
	
	int_text r = 0;
	#if INPUT_CAT
		total = file_get_size(argv[2], argv[3], k);
		if(!total){
			perror("error: strings in INPUT < K");
			return 1;
		}

			r = (total*21.0)/MEMLIMIT + 1;		
		printf("PARTITIONS = %d/%d \n", r, total);
	#endif

	printf("\n");
	#if DEBUG
		print_config();
	#endif
	
	//preprocessing
	t_start = time(NULL);
	total = k;
	
	file_open_input(&Text, argv[2], argv[3], &k, &r);

	if(PRE_OPTION){
		preprocessing(Text, argv[2], argv[3], k);
	}

	time_stop(t_start, c_start);

	t_total = time(NULL);
	c_total = clock();

	//phase 1
	t_start = time(NULL);
	c_start =  clock();

	printf("\n### PHASE 1 ###\n");
	esa_build(Text, k, SIGMA, argv[2], argv[3]);

	time_stop(t_start, c_start);

	//phase 2
	t_start = time(NULL);
	c_start =  clock();
	
	printf("\n### PHASE 2 ###\n");	
	esa_merge(Text, k, &size, argv[2], argv[3], total);
	
	time_stop(t_start, c_start);

	//total
	printf("\n### TOTAL ###\n");
	
	double d_total = time_stop(t_total, c_total)/(double) size;
	
	printf("milisecond per byte = %.9lf\n", d_total * (double) pow(10,6));
	printf("size = %zu\n", size);
	
	//validate
	if(VALIDATE){	
		t_start = time(NULL);
		c_start =  clock();
		
		printf("\n### VALIDATION ###\n");
		check(Text, k, argv[2], argv[3], total);

		time_stop(t_start, c_start);
	}

	printf("\n");
	
	int i=0;
	for(; i < k; i++)
		free(Text[i].c_file);
	free(Text);

return 0;
}

/**********************************************************************/

void print_config(void){
	
	printf("******************\n");
	
	printf("BLOCK_ESA_SIZE = %d\n", BLOCK_ESA_SIZE);
	printf("C_BUFFER_SIZE = [%d, %d]\n", C_BUFFER_SIZE, C_OVERFLOW_SIZE);

	printf("_OUTPUT_BUFFER = %d\n", _OUTPUT_BUFFER);
	#if _OUTPUT_BUFFER
	printf("OUTPUT_SIZE = %d\n", OUTPUT_SIZE);
	#endif
	
	printf("\n##################");
		
	printf("\n_PREFIX_ASSY = %d\n", _PREFIX_ASSY);
	#if _PREFIX_ASSY
		printf("PREFIX = %d\n", PREFIX_SIZE);
	#endif
	
	printf("\n_LCP_COMPARISON = %d\n", _LCP_COMPARISON);

	printf("\n_INDUCING = %d\n", _INDUCING);
	#if _INDUCING		
		printf("INDUCED_BUFFER = %d\n", INDUCED_BUFFER);
		#if INDUCED_BUFFER
			printf("INDUCED_SIZE = %d\n", INDUCED_SIZE);
		#endif
	#endif
	
	printf("##################\n");

	printf("\n");
	printf("sizeof(t_TEXT) = %lu\n", sizeof(t_TEXT));	
	printf("sizeof(heap_node) = %lu\n", sizeof(heap_node));	
	printf("\n");

	printf("sizeof(t_ESA) = %lu\n", sizeof(t_ESA));
	printf("sizeof(t_GSA) = %lu\n", sizeof(t_GSA));
	printf("[text, suff, lcp] = [%lu, %lu, %lu] bytes\n", sizeof(int_text), sizeof(int_suff), sizeof(int_lcp));

	printf("******************\n");
	
	printf("\n");
}

/**********************************************************************/

