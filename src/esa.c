#include "esa.h"

/**********************************************************************/
int esa_open(t_TEXT *Text, char *ext){
	
	char c_aux[FILE_NAME];
	sprintf(c_aux, "%s.%s", Text->c_file, ext);	

	Text->f_ESA = fopen(c_aux, "rb");//rb
	if (!Text->f_ESA) perror ("esa_malloc(f_ESA)");
	
	fseek(Text->f_ESA , 0, SEEK_SET);	

return 0;
}

int esa_malloc(t_TEXT *Text){
	
	Text->ESA = (t_ESA*) malloc (sizeof(t_ESA) * (BLOCK_ESA_SIZE + 1));
	if (!Text->ESA) perror ("esa_malloc(ESA)");
	
	Text->c_buffer = (int8*) malloc(sizeof(int8) * (C_BUFFER_SIZE + 2));
	if (!Text->c_buffer) perror ("esa_malloc(C_buffer)");
	
	Text->c_overflow = (int8*) malloc(sizeof(int8) * (C_OVERFLOW_SIZE + 2));	
	if (!Text->c_overflow) perror ("esa_malloc(c_overflow)");

return 0;
}

int esa_close(t_TEXT *Text){
	
	
	fclose(Text->f_ESA);
	free(Text->ESA);
	
	free(Text->c_buffer);
	free(Text->c_overflow);
	
return 0;
}

/**********************************************************************/

void esa_seek(FILE *File, size_t pos) {
	
	fseek(File, pos, SEEK_SET);
}

void esa_read(t_ESA *ESA, FILE *File) {

	fread(ESA, sizeof(t_ESA), BLOCK_ESA_SIZE, File);	
	
}

void esa_read_sa(unsigned *SA, size_t n, FILE *File) {
	
	fread(SA, sizeof(unsigned), n, File);
}

void esa_read_gsa(t_GSA *GSA, size_t n, FILE *File) {
	
	fread(GSA, sizeof(t_GSA), n, File);
}

int esa_read_all(t_TEXT *Text) {
	
	Text->ESA = (t_ESA*) malloc (sizeof(t_ESA) * Text->length);
	if (!Text->ESA) perror ("esa_read_all");
	
return 0;
}

/**********************************************************************/

int esa_print(t_TEXT *Text, size_t size){
	
	printf("esa\n");
	
	size_t end = BLOCK_ESA_SIZE+1;
	if(Text->length < end) end = Text->length+1;
	
	size_t j, i = end - size;
	for(i = 0; i < size; i++){
		
		printf("%zu\t|%d\t(%d)\t", i, Text->ESA[i].sa, Text->ESA[i].lcp);
		
		#if _PREFIX_ASSY
			for(j = 0; j < PREFIX_SIZE && j < 10; j++){
				printf("%d ", Text->ESA[i].prefix[j]);
			}
		#endif
		
		printf("\t|%u|\n", Text->ESA[i].bwt);
	}
	
return 0;
}

int esa_print_suff(int_suff *SA, int_lcp *LCP, t_TEXT *Text, unsigned size){
	
	printf("i\t|SA\tLCP\n");
	printf("-\t---\t---\n");
	
	unsigned i = 0;		
	for(; i < size; i++){
		printf("%u\t|%u\t(%u)\t", i, SA[i], LCP[i]);
		
		size_t j = 0;
		for(; j < 10 && SA[i]+j <= Text->length; j++)
			printf("%d|", Text->c_buffer[SA[i]+j]);
		
		printf("\n");
	}

return 0;
}

int esa_print_gsa(t_GSA *GSA, size_t size){
	
	#if BWT
		printf("i\t|text, suff \t(lcp) \t(bwt)\n");
	#else
		printf("i\t|text, suff \t(lcp)\n");
	#endif
	
	size_t i = 0;		
	for(; i < size; i++){
		#if BWT
			printf("%zu\t|%u, %u \t(%u) \t(%d)\n", i, GSA[i].text, GSA[i].suff, GSA[i].lcp, GSA[i].bwt);
		#else
			printf("%zu\t|%u, %u \t(%u) \n", i, GSA[i].text, GSA[i].suff, GSA[i].lcp);
		#endif
	}
	
return 0;
}

/**********************************************************************/

void esa_write_induced(heap *h, heap_node *node, int8 alfa, int_lcp lcp) {
	
	#if _OUTPUT_BUFFER
	
		if(h->size_out_buffer == OUTPUT_SIZE){
		
			h->size_out_buffer = 0;
			fwrite(h->out_buffer, sizeof(t_GSA), OUTPUT_SIZE, h->f_out_ESA);
		}	
		
		#if INPUT_CAT
			h->out_buffer[h->size_out_buffer].text = node->ESA[node->u_idx].text;
			h->out_buffer[h->size_out_buffer].suff = node->ESA[node->u_idx].sa_prime;
		#else
			h->out_buffer[h->size_out_buffer].text = node->key;
			h->out_buffer[h->size_out_buffer].suff = node->ESA[node->u_idx].sa;
		#endif
				
		#if BWT
			h->out_buffer[h->size_out_buffer].bwt = node->ESA[node->u_idx].bwt;
		#endif
		
		h->out_buffer[h->size_out_buffer++].lcp = lcp;
	
	#else
	
		#if INPUT_CAT
			fwrite(&node->ESA[node->u_idx].text, sizeof(int_text), 1, h->f_out_ESA);
			fwrite(&node->ESA[node->u_idx].sa_prime, sizeof(int_suff), 1, h->f_out_ESA);			
		#else
			fwrite(&node->key, sizeof(int_text), 1, h->f_out_ESA);
			fwrite(&node->ESA[node->u_idx].sa, sizeof(int_suff), 1, h->f_out_ESA);
		#endif
		
		fwrite(&lcp, sizeof(int_lcp), 1, h->f_out_ESA);
		
		#if BWT
			fwrite(&node->ESA[node->u_idx].bwt, sizeof(int8), 1, h->f_out_ESA);
		#endif
	
	#endif //_OUTPUT_BUFFER


	#if _INDUCING
		if(node->ESA[node->u_idx].bwt > alfa)
			induce(h, node, h->lcp_induced[node->ESA[node->u_idx].bwt]+1);
	#endif	//_INDUCING
}

#if _INDUCING

void induce(heap* h, heap_node *node, int_lcp lcp){
	
	#if INDUCED_BUFFER
	
		if(h->size_induced_buffer[node->ESA[node->u_idx].bwt] == INDUCED_SIZE){//induced_buffer
		
			h->size_induced_buffer[node->ESA[node->u_idx].bwt] = 0;
			fwrite(h->induced_buffer[node->ESA[node->u_idx].bwt], sizeof(t_INDUCED), INDUCED_SIZE, h->fSIGMA[node->ESA[node->u_idx].bwt]);
		}	
		h->induced_buffer[node->ESA[node->u_idx].bwt][h->size_induced_buffer[node->ESA[node->u_idx].bwt]].text = node->key;
		h->induced_buffer[node->ESA[node->u_idx].bwt][h->size_induced_buffer[node->ESA[node->u_idx].bwt]++].lcp = lcp;
		
	#else

		fwrite(&node->key, sizeof(int_text), 1, h->fSIGMA[node->ESA[node->u_idx].bwt]);
		fwrite(&lcp, sizeof(int_lcp), 1, h->fSIGMA[node->ESA[node->u_idx].bwt]);

	#endif
	
	h->lcp_induced[node->ESA[node->u_idx].bwt] = LCP_MAX;
	h->induced[node->ESA[node->u_idx].bwt]++;
}

#endif	//_INDUCING

/**********************************************************************/

// Formar ESA para gravar em disco.. 
int esa_write_all(int_suff* SA, int_lcp* LCP, t_TEXT *Text, char *c_file) {
	
	char c_aux[500];
	
	FILE *f_out;
	sprintf(c_aux, "%s.esa", Text->c_file);
	f_out = fopen(c_aux, "wb");//wb
	
	if(!f_out) perror("esa_write_all_induced");
	
	int_suff inicio;
	int_lcp  i_height = 0;
	int8 bwt;
	
	size_t i = 0;
	
	#if INPUT_CAT
			
		int_suff *ISA = NULL;
		find_inverse(SA, &ISA, Text[i].length+1);

		size_t sum=0;
		
		size_t *size = (size_t*) malloc(Text->n_strings * sizeof(size_t));

		size_t j = 0, previous = 0;
		for(; i < Text->length; i++){

			if(Text->c_buffer[i] == 0){
				size[j++] = i - previous;
				previous = i;
			}
		}
	
		int_text *D  = (int_text*) malloc((Text->length+1)* sizeof(int_text));
		int_suff *SA_prime  = (int_suff*) malloc((Text->length+1)* sizeof(int_suff));	
			
		size_t offset=0;
		j = 0;
		for(i = 0; i < Text->length; i++){
		
			D[ISA[i]] = j + Text->n_start;
			SA_prime[ISA[i]] = i-offset;
			
			
			if(Text->c_buffer[i] == 0){
				j++;
				offset = i+1;
			}
		}
		
		free(ISA);
	#endif
	
	
	#if INPUT_CAT
		i = 1;
		Text->length++;
	#else
		i = 0;
	#endif
	
	for(; i < Text->length; i++){
		
		#if _PREFIX_ASSY
			i_height += PREFIX_SIZE;		
			if(LCP[i] < i_height) i_height = LCP[i];
		#endif
			
		inicio = SA[i] + i_height;
		
		#if _INDUCING
			if(Text->c_buffer[SA[i]] > Text->c_buffer[SA[i]+1]){
				
				LCP[i+1] = 0;
				inicio = SA[i];
				
				#if _PREFIX_ASSY
					i_height = PREFIX_SIZE;
				#endif
			}
		#endif
			
		//write the node into the file
		fwrite(&SA[i], sizeof(int_suff), 1, f_out);
		fwrite(&LCP[i], sizeof(int_lcp), 1, f_out);
		
		#if _PREFIX_ASSY
			fwrite(&Text->c_buffer[inicio], sizeof(int8), PREFIX_SIZE, f_out);
		#endif
		
		//bwt
		bwt = 0;
		if(SA[i] > 0) bwt = Text->c_buffer[SA[i] - 1];
		
		fwrite(&bwt, sizeof(int8), 1, f_out);
		
		
		#if INPUT_CAT
			fwrite(&D[i], sizeof(int_text), 1, f_out);
			fwrite(&SA_prime[i], sizeof(int_suff), 1, f_out);
			
			if(SA_prime[i]==0) sum++;
		#endif
	}
	//extra node [n, 0, 500]
	
	t_ESA aux_gsa;
	aux_gsa.sa = Text->length;
	aux_gsa.lcp = 0;
	
	#if INPUT_CAT
		aux_gsa.text = 0;
		aux_gsa.sa_prime = 1;
	#endif
	
	#if _PREFIX_ASSY
		for(i=0; i < PREFIX_SIZE; i++)
			aux_gsa.prefix[i] = SIGMA+1;
	#endif
	aux_gsa.bwt = 0;
	
	fwrite(&aux_gsa, sizeof(t_ESA), 1, f_out);

	//fflush(f_out);	
	if(fclose(f_out)==EOF) printf("error closing file %s.\n\n\n", c_aux); 
	
	#if INPUT_CAT
		free(SA_prime);
		free(D);
		free(size);
	#endif
	
return 0;
}

/**********************************************************************/

int esa_build(t_TEXT *Text, int_text k, int sigma, char* c_file){

	int_suff *SA;
	int_lcp *LCP;
	
	int_text i = 0;
	for(; i < k; i++){
	
		open_sequence(&Text[i], c_file);		//open bin		
		seek_sequence(Text[i].f_in, 0);			//seek

		#if DEBUG
			printf("T_%d\t%d\n", i, Text[i].length);	
		#endif
		
		load_sequence(&Text[i]);			//load sequence	
		
		Text[i].key = i;
		
		SA  = (int_suff*) malloc((Text[i].length+3) * sizeof(int_suff));
		if(!SA) perror("esa_build");
		
		LCP = (int_lcp*) malloc((Text[i].length+3) * sizeof(int_lcp));
		if(!LCP) perror("esa_build");
		
		
		#if INPUT_CAT
			unsigned j = 0;

			//computes gsa in 5n bytes
			for(j=0; j < Text[i].length; j++) Text[i].c_buffer[j]++;
			Text[i].c_buffer[j]=0;

			gSACA_K((unsigned char*)Text[i].c_buffer, (int*)SA, Text[i].length+1, SIGMA+2, Text[i].length+1, sizeof(char), 0, 1); // 5n bytes

			for(j=0; j < Text[i].length; j++) Text[i].c_buffer[j]--;
	
			//computes lcp in 13n bytes
			lcp_PHI(Text[i].c_buffer, SA, Text[i].length+1, LCP); // 13n bytes
		#else
		
			//computes sa+lcp in 9n bytes
			sais_lcp(Text[i].c_buffer, SA, LCP, Text[i].length);
		#endif

		/**************************************************************/
		//validate
		#if DEBUG
		
		if(!check_sa(SA, Text[i].length, &Text[i]))
			printf("isNotSorted!!\n");
		else
			printf("isSorted!!\n");
			
		check_lcp_array(Text[i].c_buffer, SA, Text[i].length, LCP);
		printf("media(LCP) = %d\n", lcp_media(LCP, Text[i].length));
		
		#endif //DEBUG
		/**************************************************************/
		
		Text[i].c_buffer[Text[i].length] = SIGMA+1; //depois do $, colocar # ($<A<C<G<T<#)
		
		#if DEBUG
			esa_print_suff(SA, LCP, &Text[i],  20);
		#endif		
		//write on disk
		esa_write_all(SA, LCP, &Text[i], c_file);

		/**************************************************************/
		
		free(SA);
		free(LCP);
		free(Text[i].c_buffer);
	}
	
return 0;
}


/**********************************************************************/

int esa_merge(t_TEXT *Text, int_text k, size_t *size, char* c_file, int_text total){//size = soma de todos os tamanhos
	
	*size = 0;
	
	//load all partition (beginning)
	heap *H = heap_alloc_induced(k, c_file, total);
	int_text j = 0;
	for(; j < k; j++){
		
		Text[j].key = j;
		
		open_sequence(&Text[j], c_file);//open bin

		esa_open(&Text[j], "esa");				
		esa_malloc(&Text[j]);
		esa_read(Text[j].ESA, Text[j].f_ESA);
		
		#if DEBUG
			printf("T_%d\t%d\n", j, Text[j].length);
			esa_print(&Text[j], 5);	printf("\n");
		#endif
		
		Text[j].u_idx = Text[j].i_height = Text[j].i_loaded = 0; 
	
		*size += Text[j].length;
	}

	printf("size = %zu\n", *size); 

	//initialize the heap
	for(j = 0; j < k; j++){
		H->lcp_son_dad[j] = H->lcp_left_right[j] = 0;
		heap_insert(H, j, &Text[j], 0);		
	}
	
	heap_lcp(H, 0);
	printf("\n");
	printf("INDUCING:\n");
	printf("alfa\tTOTAL\tINDUCED\t\%:\n");

	size_t induced_suffixes = 0;
	size_t ant = 0;

	int8 alfa = 0;
	size_t i = 0;
	for(; i < *size; i++){
		
		#if _INDUCING
			if(H->heap[0]->c_buffer[0] > alfa){
			
				printf("%d)\t%d\t%d\t%.2lf\n", alfa, i-ant, induced_suffixes, (100.0)*(induced_suffixes/(double) (i-ant)));
						
				ant = i;		
				alfa = H->heap[0]->c_buffer[0];	
				
				//RMQ
				j = 1;
				for(; j < SIGMA+1; j++) H->lcp_induced[j] = 0;
							
				if(H->induced[alfa]){//induceds > 0
					

					induced_suffixes = heap_pass_induced(H, Text, &i, alfa);
					
					H->size = 0;
					for(j = 0; j < k; j++){
						
						H->lcp_son_dad[j] = H->lcp_left_right[j] = 0;
						heap_update(H, j, &Text[j], Text[j].u_idx);
					}
					
					if(H->heap[0]->c_buffer[0] > alfa){
						i--; continue;
					}
					
					H->lcp_son_dad[0] = 1;
					heap_lcp(H, 0);
				}
			}
		#endif	//_INDUCING
		
		heap_delete_min(H);				
	}
	printf("%d)\t%d\t%d\t%.2lf\n", alfa, i-ant, induced_suffixes, (100.0)*(induced_suffixes/(double) (i-ant)));
	
	#if _OUTPUT_BUFFER
		fwrite(H->out_buffer, sizeof(t_GSA), H->size_out_buffer, H->f_out_ESA);//fflush out_buffer
	#endif
	
	i = 0;
	for(; i < k; i++){
		esa_close(&Text[i]);
		fclose(Text[i].f_in);
	}
	heap_free(H);
	
return 0;	
}

/**********************************************************************/

int print_delete(heap *H, size_t i, size_t size, int_text n){
		
	printf("\n#####%zu/%zu######", i, size);
	
	int_text k = 0;
	for(; k < n; k++)
		print_node(H->heap[k]);
		
	for(k = 0; k < n; k++)
			printf("heap_son[%d] = %d\n", k, H->lcp_son_dad[k]);
	
	printf("-\n");
	for(k = 0; k < n; k++)
			printf("heap_left[%d] = %d\n", k, H->lcp_left_right[k]);

return 0;
}

/**********************************************************************/

