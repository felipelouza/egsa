/***********************************************************************

egsa-1.2: External Generalized Suffix Array Construction Algorithm 
Louza, may 2016.

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

#include "src/gesa.h"
#include "lib/utils.h"
#include "lib/file.h"
#include "lib/esa.h"
#include "lib/defines.h"

void print_config(void);

/**********************************************************************/

int main(int argc, char **argv) {
	
	int_text K;
	unsigned PRE_OPTION, VALIDATE;
	size_t MEMLIMIT = 1;

	sscanf(argv[1], "%u", &PRE_OPTION);
	//argv[2] = DIR
	//argv[3] = DATABASE
	sscanf(argv[4], "%d", &K);//NUMBER OF STRINGS
	sscanf(argv[5], "%u", &MEMLIMIT);//AVAILABLE MEMORY
	sscanf(argv[6], "%u", &VALIDATE);

	printf("SIGMA = %d\n", SIGMA);
	printf("DIR = %s\n", argv[2]);
	printf("INPUT = %s\n", argv[3]);
	printf("K = %d\n", K);
	MEMLIMIT *= 1024*1024;
	printf("MEMLIMIT = %.2lf MB\n", MEMLIMIT/pow(2,20));
	printf("PRE = %d\n", PRE_OPTION);
	printf("CHECK = %d\n", VALIDATE);

	
	#if DEBUG
		print_config();
	#endif

	//c_dir, c_file, K, memlimit, check
	gesa(argv[2], argv[3], K, MEMLIMIT, VALIDATE);

return 0;
}

/**********************************************************************/

void print_config(void){
	
	printf("******************\n");
	
	printf("BLOCK_ESA_SIZE = %d\n", BLOCK_ESA_SIZE);
	printf("C_BUFFER_SIZE = [%d, %d]\n", C_BUFFER_SIZE, C_OVERFLOW_SIZE);

	printf("_OUTPUT_BUFFER = %d\n", _OUTPUT_BUFFER);
	#if _OUTPUT_BUFFER
	printf("OUTPUT_SIZE = %.2lf MB\n", OUTPUT_SIZE*sizeof(t_GSA)/pow(2,20));
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
			printf("INDUCED_SIZE = %.2lf MB\n", (INDUCED_SIZE*sizeof(t_INDUCED))/pow(2,20));
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

