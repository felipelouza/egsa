#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "../src/egsa.h"
#include "../lib/utils.h"
#include "../lib/file.h"
#include "../lib/esa.h"
#include "../lib/defines.h"

/**********************************************************************/

int main(int argc, char **argv) {
	
	int_text K;
	size_t MEMLIMIT = 1;

	//argv[1] = DIR
	//argv[2] = DATABASE
	sscanf(argv[3], "%d", &K);//NUMBER OF STRINGS

	printf("SIGMA = %d\n", SIGMA);
	printf("DIR = %s\n", argv[2]);
	printf("INPUT = %s\n", argv[3]);
	printf("K = %d\n", K);
	MEMLIMIT = ULLONG_MAX;
	printf("MEMLIMIT = %.2lf MB\n", MEMLIMIT/pow(2,20));

	t_TEXT *Text;
        int_suff COUNT[SIGMA];
	int i;
        for(i=0; i<SIGMA;i++) COUNT[i]=0;
	file_chdir(argv[1]);
        preprocessing(&Text, argv[2], MEMLIMIT, &K, COUNT);

return 0;
}

