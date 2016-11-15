#include "egsa.h"

/**********************************************************************/
int egsa(char *c_dir, char* c_file, int_text K, unsigned VALIDATE){

	time_t t_start, t_total;
	clock_t c_start, c_total;
	
	int_text total;
	size_t n=1;

	file_chdir(c_dir);
	
	//preprocessing
	t_start = time(NULL);
	c_start =  clock();
	total = K;

	int_suff COUNT[SIGMA];
	int i;
	for(i=0; i<SIGMA;i++) COUNT[i]=0;
 
	t_TEXT *Text;
	size_t size; //sizeof generalized suffix array

	n = preprocessing(&Text,c_file, &K, COUNT);
	if(n==0) return 1;		
	
	printf("TOTAL = %zu bytes\t%.2lf MB\n", n, n/pow(2,20));

	time_stop(t_start, c_start);

	t_total = time(NULL);
	c_total = clock();

	//phase 1
	t_start = time(NULL);
	c_start =  clock();

	printf("\n### PHASE 1 ###\n");
	esa_build(Text, K, SIGMA, c_file);

	time_stop(t_start, c_start);

	//phase 2
	t_start = time(NULL);
	c_start =  clock();
	
	printf("\n### PHASE 2 ###\n");	
	esa_merge(Text, K, &size, c_file, total, COUNT, n);
	
	time_stop(t_start, c_start);

	//total
	printf("\n### TOTAL ###\n");
	
	double d_total = time_stop(t_total, c_total)/size;
	
	printf("milisecond per byte = %.9lf\n", d_total*(double) pow(10,6));
	fprintf(stderr,"%.9lf\n", d_total*(double) pow(10,6));	
	printf("size = %zu\n", size);
	
	//validate
	if(VALIDATE){	
		t_start = time(NULL);
		c_start =  clock();
		
		printf("\n### VALIDATION ###\n");
		check(Text, K, c_file, total);

		time_stop(t_start, c_start);
	}

	printf("\n");
	
	for(i=0; i < K; i++)
		free(Text[i].c_file);
	free(Text);

return 0;
}

/**********************************************************************/

