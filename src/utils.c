#include "utils.h"

/**********************************************************************/
void print1(int8* a, unsigned n, const char *comment) {//1 byte
	
	printf("%s\n", comment);
	unsigned i;
	
	for (i = n-5;  i < n;  i++) {
		printf("%d, %d\n", i, a[i]);	
	}
	
printf("\n");
}

void print4(unsigned* a, unsigned n, const char *comment) {//4 bytes
	
	printf("%s:\n", comment);
	unsigned i;
	for (i = 0;  i < n;  i++) {
		printf("%d\n", a[i]);
	}
}


/**********************************************************************/

double time_stop(time_t t_time, clock_t c_clock){

	double aux1 = (clock() - c_clock) / (double)(CLOCKS_PER_SEC);
	double aux2 = difftime (time(NULL),t_time);
	
	printf("CLOCK = %lf TIME = %lf\n", aux1, aux2);
	
	return aux2;
}

/**********************************************************************/

void die(const char* where) {

	printf("Error at %s: %s.\n",where,errno ? strerror(errno) : "errno not set");
	exit(errno?errno:1);
}
 
void dies(const char* where, char* format, ...) {

	int err = errno;
	va_list val;

	printf("Error at %s: %s.\n",where,errno ? strerror(errno) : "errno not set");
	va_start(val,format);
	vprintf(format,val);
	va_end(val);
	printf("\n");

	exit(err ? err : 1);
}
/**********************************************************************/
int print_int(int* A, int n){

	int i;
	for(i=0; i<n; i++)	
		printf("%d) %d\n", i, A[i]);

	printf("\n");

return 0;
}
/**********************************************************************/
int min_range(int* A, int l, int r){


	if(r>l)return 0;

	printf("[l, r] = [%d, %d]\n", l, r);

	int min = INT_MAX;
	int i;
	for(i=l; i<=l; i++)
		min = (A[i]<min?A[i]:min);

	printf("min = %d\n", min);

return min;
}
/**********************************************************************/

void check(t_TEXT *Text, int_text n, char *c_dir, char* c_file, int_text total){
	
	size_t length = 0;
	
	int_text k = 0;
	for(; k < n; k++){
		
		open_sequence(&Text[k], c_dir, c_file);	//open bin		
		seek_sequence(Text[k].f_in, 0);			//seek
		
		load_sequence(&Text[k]);				//load sequence	
		
		#if INPUT_CAT
		
			Text[k].pos = (size_t*) malloc((Text[k].n_strings+1)*sizeof(size_t));
		
			size_t i = 0, j = 1; //, previous = 0;
			for(; i < Text[k].length; i++){
				
				if(Text[k].c_buffer[i] == 0)
					Text[k].pos[j++] = i+1;
			}
			Text[k].pos[0] = 0;
				
			/*
			printf("n_strings[%d] = %d\n", k, Text[k].n_strings);
			printf("start[%d] = %d\n", k, Text[k].n_start);
			*/
		#endif
		
		length += Text[k].length;
	}
	
	
	t_TEXT *t_Aux = (t_TEXT*) malloc(sizeof(t_TEXT));
	
	t_Aux->c_file = (char*) malloc(sizeof(char)*FILE_NAME);
	sprintf(t_Aux->c_file, "%s%s.%d", c_dir, c_file, total);
	
	esa_open(t_Aux, "gesa");
	esa_malloc(t_Aux);	
	
	if(!check_gsa(Text, length, n, t_Aux)) printf("isNotSorted!!\n");
	else printf("isSorted!!\n");

	esa_close(t_Aux);
	
	
	esa_open(t_Aux, "gesa");
	esa_malloc(t_Aux);	
	
	if(!check_gsa_lcp(Text, length, n, t_Aux)) printf("isNotLCP!!\n");
	else printf("isLCP!!\n");
	
	esa_close(t_Aux);
	
	/**/
	#if BWT
		esa_open(t_Aux, "gesa");
		esa_malloc(t_Aux);		
		
		if(!check_bwt(Text, length, n, t_Aux)) printf("isNotBWT!!\n");
		else printf("isBWT!!\n");
			
		esa_close(t_Aux);
	#endif
	/**/

	esa_open(t_Aux, "gesa");
	esa_malloc(t_Aux);		
	
	if(!check_is_permutation(Text, length, n, t_Aux)) printf("isNotPermutation!!\n");
	else printf("isPermutation!!\n");


	for(k = 0; k < n; k++){
		free(Text[k].c_buffer);
		
		#if INPUT_CAT
			free(Text[k].pos);
		#endif
	}

	esa_close(t_Aux);
	free(t_Aux->c_file);
	free(t_Aux);
	
}

/**********************************************************************/

int sleq(int8 *s1, int8 *s2) {

	if(!s1[0] && !s2[0]) {
		return 1;
	}
	
	if (s1[0] < s2[0]) return 1;
	if (s1[0] > s2[0]){
		printf("$%d, %d$ (zero)\n", s1[0], s2[0]);
		return 0;
	}
	
	int temp = sleq(s1+1, s2+1);
	
	if(!temp)
		printf("$%d, %d$\n", s1[0], s2[0]);
	
return temp;
} 

#if INPUT_CAT
size_t get_text(t_TEXT *Text, int_text n, int_text text){
	
	int i = 0;
	for(i = 0;i<n; i++){
		if(text < Text[i].n_start){
			return i-1;
		}
	}

return i-1;
}

size_t get_suff(t_TEXT *Text, int_text key, int_text text, int_text suff){
	
	
	return Text[key].pos[text-Text[key].n_start] + suff;
}
#endif
/***********************************************************************/

int check_gsa_lcp(t_TEXT *Text, size_t length, int_text n, t_TEXT *t_Aux){
	
	size_t k = 0;
	size_t lcp_media = 0;
	size_t lcp_max = 0;
	
	size_t pos;
	
	t_GSA *GSA = (t_GSA*) malloc((BLOCK_ESA_SIZE+1) * sizeof(t_GSA));	
	if(!GSA) perror("is_LCP_GSA");
	
	esa_read_gsa(GSA, BLOCK_ESA_SIZE+1, t_Aux->f_ESA);	
	pos = ftell(t_Aux->f_ESA);
	esa_seek(t_Aux->f_ESA, pos - (unsigned) sizeof(t_GSA));
	
	//esa_print_gsa(GSA, 20);

	size_t text[2];
	size_t suff[2];
	
	for(; k < length/BLOCK_ESA_SIZE+1; k++){
	
		unsigned i;
		for (i = 0;  i < BLOCK_ESA_SIZE;  i++) {
			
			if(i+(k*BLOCK_ESA_SIZE) == length-1){
				
				//printf("k = %d\n", k);	
				printf("mean LCP = %zu\nLCP max = %zu\n", lcp_media/length, lcp_max);
				free(GSA);
				return 1;
			}
			
			if(GSA[i+1].lcp>lcp_max); lcp_max = GSA[i+1].lcp;
			
			lcp_media += GSA[i+1].lcp;
			
			int_lcp h = 0;

			#if INPUT_CAT
				
				text[0] = get_text(Text, n, GSA[i].text);
				suff[0] = get_suff(Text, text[0], GSA[i].text, GSA[i].suff);

				
				text[1] = get_text(Text, n, GSA[i+1].text);
				suff[1] = get_suff(Text, text[1], GSA[i+1].text, GSA[i+1].suff);
			#else
			
				text[0] = GSA[i].text;
				suff[0] = GSA[i].suff;
				
				text[1] = GSA[i+1].text;
				suff[1] = GSA[i+1].suff;
			#endif
						

			for(h=0; h<length ;h++){
				
				if(Text[text[0]].c_buffer[suff[0] + h] == 0 && Text[text[1]].c_buffer[suff[1] + h] == 0){
					//++h; 
					break;
				}
				
				if(Text[text[0]].c_buffer[suff[0] + h] != Text[text[1]].c_buffer[suff[1] + h]) 
					break;					
					
				if(!Text[text[0]].c_buffer[suff[0] + h] && !Text[text[1]].c_buffer[suff[1] + h]){
					h++;
					break;
				}
			}
					
			if(GSA[i+1].lcp!=h) {
			
				printf("&%zu) [%u,%u], [%u, %u] -> [%d, %d]&\n", i+(k*BLOCK_ESA_SIZE), text[0], suff[0], text[1], suff[1], suff[0], suff[1]);
				printf("i = %zu - %zu\n", i+(k*BLOCK_ESA_SIZE), length-1);
				printf("lcp = %u, h = %u\n", GSA[i+1].lcp, h);
				
				int_lcp j = 0;
				for(; j<h+10;j++){
					printf("%d) %d != %d\n", j, Text[text[0]].c_buffer[suff[0] + j], Text[text[1]].c_buffer[suff[1] + j]);				
				}
				
				free(GSA);				
				
				return 0;
			}
		}
		esa_read_gsa(GSA, BLOCK_ESA_SIZE+1, t_Aux->f_ESA);
		pos = ftell(t_Aux->f_ESA);	
		esa_seek(t_Aux->f_ESA, pos - sizeof(t_GSA));
	}

printf("mean LCP = %zu\n", lcp_media/length);

return 1; 		
}


/**********************************************************************/

int check_gsa(t_TEXT *Text, size_t length, int_text n, t_TEXT *t_Aux) {

	printf("length = %zu\n", length);

	size_t k = 0;
	size_t pos;
	
	t_GSA *GSA = (t_GSA*) malloc((BLOCK_ESA_SIZE+1) * sizeof(t_GSA));	
	if(!GSA) perror("is_sorted_GSA");
	
	esa_read_gsa(GSA, BLOCK_ESA_SIZE+1, t_Aux->f_ESA);	
	pos = ftell(t_Aux->f_ESA);
	esa_seek(t_Aux->f_ESA, pos - sizeof(t_GSA));
	
	esa_print_gsa(GSA, 20);
	
	size_t text[2];
	size_t suff[2];
	int sum = 0;
	for(; k < length/BLOCK_ESA_SIZE+1; k++){
		
		unsigned i;
		for (i = 0;  i < BLOCK_ESA_SIZE;  i++) {
			
			if(i+(k*BLOCK_ESA_SIZE) == length-1){
				
				free(GSA);
				return 1;
			}
				
			//printf("&%zu) [%u,%u], [%u, %u]&\n", i+(k*BLOCK_ESA_SIZE), GSA[i].text, GSA[i].suff, GSA[i+1].text, GSA[i+1].suff);
			
			#if INPUT_CAT
				text[0] = get_text(Text, n, GSA[i].text);
				suff[0] = get_suff(Text, text[0], GSA[i].text, GSA[i].suff);

				text[1] = get_text(Text, n, GSA[i+1].text);
				suff[1] = get_suff(Text, text[1], GSA[i+1].text, GSA[i+1].suff);
			
			#else
			
				text[0] = GSA[i].text;
				suff[0] = GSA[i].suff;
				
				text[1] = GSA[i+1].text;
				suff[1] = GSA[i+1].suff;
			#endif

			if(GSA[i].suff == 0)
			sum++;
			
			if (!sleq(Text[text[0]].c_buffer+suff[0], Text[text[1]].c_buffer + suff[1])){

				printf("&%zu) [%u,%u], [%u, %u]&\n", i+(k*BLOCK_ESA_SIZE), GSA[i].text, GSA[i].suff, GSA[i+1].text, GSA[i+1].suff);
				
				printf("&%zu) [%u,%u], [%u, %u]&\n", i+(k*BLOCK_ESA_SIZE), text[0], suff[0], text[1], suff[1]);
				printf("i = %zu - %zu\n", i+(k*BLOCK_ESA_SIZE), length-1);
				
				free(GSA);			
				return 0;
			}
		}
		esa_read_gsa(GSA, BLOCK_ESA_SIZE+1, t_Aux->f_ESA);
		pos = ftell(t_Aux->f_ESA);	
		esa_seek(t_Aux->f_ESA, pos - sizeof(t_GSA));
	}
	
return 1;  
}

/**********************************************************************/

int check_is_permutation(t_TEXT *Text, size_t length, int_text n, t_TEXT *t_Aux){


	size_t i, k;
	
	t_GSA *GSA = (t_GSA*) malloc((BLOCK_ESA_SIZE) * sizeof(t_GSA));	
	if(!GSA) perror("is_permutation_GSA");
	
	
	int_text y;
	for(y = 0; y < n; y++)
		for (i = 0;  i < Text[y].length;  i++) 
			Text[y].c_buffer[i] = 0;//zera todos
	
	esa_read_gsa(GSA, BLOCK_ESA_SIZE, t_Aux->f_ESA);
	
	size_t text[1];
	size_t suff[1];
	
	for(k = 0; k < length/BLOCK_ESA_SIZE+1; k++){
	
		for (i = 0;  i < BLOCK_ESA_SIZE;  i++) {
			
			if(i+(k*BLOCK_ESA_SIZE) == length)	
				break;
			
			#if INPUT_CAT
				
				text[0] = get_text(Text, n, GSA[i].text);
				suff[0] = get_suff(Text, text[0], GSA[i].text, GSA[i].suff);
			#else
			
				text[0] = GSA[i].text;
				suff[0] = GSA[i].suff;
			#endif
			
			Text[text[0]].c_buffer[suff[0]] = 1;
			
			
		}
		
		esa_read_gsa(GSA, BLOCK_ESA_SIZE, t_Aux->f_ESA);
	}
	
	for(y = 0; y < n; y++)
		for (i = 0;  i < Text[y].length;  i++){ 
			//printf("%d) %d\n", i, Text[0].c_buffer[i]);
			if (!Text[y].c_buffer[i])
				return 0;
		}

	free(GSA);

return 1;
}

/**********************************************************************/
#if BWT
int check_bwt(t_TEXT *Text, size_t length, int_text n, t_TEXT *t_Aux) {


	size_t k = 0;
	size_t pos;
	
	t_GSA *GSA = (t_GSA*) malloc((BLOCK_ESA_SIZE+1) * sizeof(t_GSA));	
	if(!GSA) perror("is_sorted_GSA");
	
	esa_read_gsa(GSA, BLOCK_ESA_SIZE+1, t_Aux->f_ESA);	
	pos = ftell(t_Aux->f_ESA);
	esa_seek(t_Aux->f_ESA, pos - sizeof(t_GSA));
	
	size_t text[1];
	size_t suff[1];
	
	for(; k < length/BLOCK_ESA_SIZE+1; k++){
		
		unsigned i;
		for (i = 0;  i < BLOCK_ESA_SIZE;  i++) {
			
			if(i+(k*BLOCK_ESA_SIZE) == length-1){
				
				free(GSA);
				return 1;
			}

			#if INPUT_CAT				
				text[0] = get_text(Text, n, GSA[i].text);
				suff[0] = get_suff(Text, text[0], GSA[i].text, GSA[i].suff);
			#else			
				text[0] = GSA[i].text;
				suff[0] = GSA[i].suff;				
			#endif


			if (Text[text[0]].c_buffer[suff[0]-1] != GSA[i].bwt){
				
				printf("&%zu) |%u|%u|, (%u)&\n", i+(k*BLOCK_ESA_SIZE), Text[text[0]].c_buffer[suff[0]-1], Text[text[0]].c_buffer[suff[0]], GSA[i].bwt);
				printf("i = %zu - %zu\n", i+(k*BLOCK_ESA_SIZE), length-1);
				
				free(GSA);			
				return 0;
			}
		}
		esa_read_gsa(GSA, BLOCK_ESA_SIZE+1, t_Aux->f_ESA);
		pos = ftell(t_Aux->f_ESA);	
		esa_seek(t_Aux->f_ESA, pos - sizeof(t_GSA));
	}
	
return 1;
}
#endif
/**********************************************************************/

int check_sa(int_suff *SA, size_t length, t_TEXT *Text) {

	
	unsigned i;
	#if INPUT_CAT
		i = 1;
	#else
		i = 0;
	#endif
	
	for (;  i < length-1;  i++) {
		//printf("%d) %d, %d&\n", i, SA[i], SA[i+1]);
		if (!sleq(Text->c_buffer+SA[i], Text->c_buffer+SA[i+1])){
			printf("#%d) %d, %d&\n", i, SA[i], SA[i+1]);
			return 0;
		}
	}
	
return 1;  
}

int check_lcp_array(int8 *t, int_suff *SA, size_t length, int_lcp *lcp){
	
  size_t i,j,k;
  int_lcp h=0;

	for(i=1;i<length;i++) {
		
		j = SA[i-1]; k=SA[i];	
		for(h=0;j+h<length && k+h<length;h++){
			
			if(t[j+h]== 0 && t[k+h]==0){
				//++h; 
				break;
			}
			if(t[j+h]!=t[k+h]) break;
			
		}		
		if(lcp[i]!=h) {
			fprintf(stdout,"isNotLCP! Incorrect LCP value: lcp[%zu]=%d!=%d\n",
			i,lcp[i],h);
			
			for(h=0;j+h<length && k+h<length;h++){
				printf("%d == %d\n", t[j+h], t[k+h]);
				if(t[j+h]!=t[k+h]) break;
			}
			
			return 0;
		}
	}

	fprintf(stdout,"LCP array OK!\n");
	return 1;
}
/***********************************************************************/
