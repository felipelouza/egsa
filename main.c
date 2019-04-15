// vim: noai:ts=2:sw=2
/***********************************************************************

egsa: External Generalized Suffix Array Construction Algorithm 

Authors: Felipe A. Louza* and Guilherme P. Telles
*contact: louza@usp.br

Copyright (C) 2018 Felipe Louza <louza@usp.br>

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

#include "src/egsa.h"
#include "lib/utils.h"
#include "lib/file.h"
#include "lib/esa.h"
#include "lib/defines.h"

void print_config(void);

void usage(char *name){
  printf("\n\tUsage: %s [options] FILE K\n\n",name);
  puts("Computes GESA (GSA+LCP) for the first K sequences of a");
  puts("collection using algorithm eGSA from Louza et al. [ALMOB, 2017]. ");
  puts("Sequences from FILE are extracted according to FILE's");
  puts("extension; currently supported extensions are: .txt .fasta .fastq\n");
  puts("Available options:");
  puts("\t-h    this help message");
  puts("\t-b    output BWT (ext: .bwt)");
  puts("\t-c    check SA and LCP");
  puts("\t-v    verbose output (more v's for more verbose)\n");
  exit(EXIT_FAILURE);
}

/**********************************************************************/

int main(int argc, char **argv) {
	
  extern char *optarg;
  extern int optind, opterr, optopt;
  int c;
  
	int_text K=0;
	unsigned VALIDATE=0, VERBOSE=0, COMPUTE_BWT=0;
  char *c_dir=NULL, *c_file=NULL, *c_input=NULL;
//	size_t MEMLIMIT = 1;

  while ((c=getopt(argc, argv, "vcbh")) != -1) {
    switch (c) 
      {
      case 'v':
        VERBOSE++; break;
      case 'c':
        VALIDATE=1; break;          // validate output
      case 'b':
        COMPUTE_BWT+=1; break;      // output BWT  
      case 'h':
        usage(argv[0]); break;      // show usage and stop
      case '?':
        exit(EXIT_FAILURE);
      }
  }

  if(optind+2==argc) {
    c_input=argv[optind++];
    K = (int_text) atoi(argv[optind++]);
  }
  else  usage(argv[0]);
  
  c_file= strrchr(c_input, '/');
	if(c_file){
		c_file++;
		c_dir = strndup(c_input, strlen(c_input)-strlen(c_file));
	}
	else{
		c_file = c_input;
		c_dir = "./";
	}
  
  printf("SIGMA = %d\n", SIGMA);
  printf("DIR = %s\n", c_dir);
  printf("INPUT = %s\n", c_file);
  printf("K = %d\n", K);
  printf("MEMLIMIT = %.2lf MB\n", RAM_USE/pow(2,20));
  printf("CHECK = %d\n", VALIDATE);
  printf("COMPUTE_BWT = %d\n", COMPUTE_BWT);
  printf("WORKSPACE = %d.n bytes\n", WORKSPACE);
  #if MAC_OS
    printf("MAC_OS == %d\n", MAC_OS);
  #endif
    
  if(VERBOSE){
    print_config();
  }
      
	//c_dir, c_file, K, check
	egsa(c_dir, c_file, K, VALIDATE, VERBOSE, COMPUTE_BWT);

return 0;
}

/**********************************************************************/

void print_config(void){
	
	printf("******************\n");

	printf("BLOCK_ESA_SIZE = %.2lf MB\n", (BLOCK_ESA_SIZE*sizeof(t_ESA))/pow(2,20));
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

