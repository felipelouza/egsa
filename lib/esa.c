#include "esa.h"

/**********************************************************************/
int esa_open(t_TEXT *Text, char *ext){
	
	char c_aux[FILE_NAME];
	sprintf(c_aux, "%s.%s", Text->c_file, ext);	

	Text->f_ESA = fopen(c_aux, "rb");//rb
	if (!Text->f_ESA) perror ("esa_open(f_ESA)");
	
	fseek(Text->f_ESA , 0, SEEK_SET);	

return 0;
}

int esa_malloc(t_TEXT *Text){
	
	Text->ESA = (t_ESA*) malloc (sizeof(t_ESA) * (Text->block_esa_size + 1)); //BLOCK_ESA_SIZE
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

void esa_read(t_ESA *ESA, FILE *File, size_t block_esa_size) {

	fread(ESA, sizeof(t_ESA), block_esa_size, File);	
	
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
	
	size_t i = end - size;
	for(i = 0; i < size; i++){
		
		printf("%zu\t|%d\t(%d)\t", i, Text->ESA[i].sa, Text->ESA[i].lcp);
		
		#if _PREFIX_ASSY
			size_t j;
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
			printf("%zu\t|%u, %u \t(%u) \t(%c)\n", i, GSA[i].text, GSA[i].suff, GSA[i].lcp, GSA[i].bwt);
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
			#if IO_VOLUME
				node->io_write++;
			#endif
		}	
		
		h->out_buffer[h->size_out_buffer].text = node->ESA[node->u_idx].text;
		h->out_buffer[h->size_out_buffer].suff = node->ESA[node->u_idx].sa_prime;
				
		#if BWT
			h->out_buffer[h->size_out_buffer].bwt = node->ESA[node->u_idx].bwt;
			#if BWT_OUTPUT
				fwrite(&node->ESA[node->u_idx].bwt, sizeof(int8), 1, h->f_out_BWT);
				#if IO_VOLUME
					node->io_write++;
				#endif
			#endif
		#endif
		
		h->out_buffer[h->size_out_buffer++].lcp = lcp;
	
	#else

		fwrite(&node->ESA[node->u_idx].text, sizeof(int_text), 1, h->f_out_ESA);
		#if IO_VOLUME
			node->io_write++;
		#endif
		fwrite(&node->ESA[node->u_idx].sa_prime, sizeof(int_suff), 1, h->f_out_ESA);			
		#if IO_VOLUME
			node->io_write++;
		#endif
		fwrite(&lcp, sizeof(int_lcp), 1, h->f_out_ESA);
		#if IO_VOLUME
			node->io_write++;
		#endif
		
		#if BWT
			fwrite(&node->ESA[node->u_idx].bwt, sizeof(int8), 1, h->f_out_ESA);
			#if IO_VOLUME
				node->io_write++;
			#endif
			#if BWT_OUTPUT
				fwrite(&node->ESA[node->u_idx].bwt, sizeof(int8), 1, h->f_out_BWT);
				#if IO_VOLUME
					node->io_write++;
				#endif
			#endif
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
	
		if(h->inserted_induced_buffer[node->ESA[node->u_idx].bwt] == h->size_induced_buffer[node->ESA[node->u_idx].bwt]){//induced_buffer
		
			h->inserted_induced_buffer[node->ESA[node->u_idx].bwt] = 0;
			fwrite(h->induced_buffer[node->ESA[node->u_idx].bwt], sizeof(t_INDUCED), h->size_induced_buffer[node->ESA[node->u_idx].bwt], h->fSIGMA[node->ESA[node->u_idx].bwt]);
			#if IO_VOLUME
				node->io_write++;
			#endif
		}	
		h->induced_buffer[node->ESA[node->u_idx].bwt][h->inserted_induced_buffer[node->ESA[node->u_idx].bwt]].text = node->key;
		h->induced_buffer[node->ESA[node->u_idx].bwt][h->inserted_induced_buffer[node->ESA[node->u_idx].bwt]++].lcp = lcp;
		
	#else

		fwrite(&node->key, sizeof(int_text), 1, h->fSIGMA[node->ESA[node->u_idx].bwt]);
		#if IO_VOLUME
			node->io_write++;
		#endif
		fwrite(&lcp, sizeof(int_lcp), 1, h->fSIGMA[node->ESA[node->u_idx].bwt]);
		#if IO_VOLUME
			node->io_write++;
		#endif

	#endif
	
	h->lcp_induced[node->ESA[node->u_idx].bwt] = LCP_MAX;
	h->induced[node->ESA[node->u_idx].bwt]++;
}

#endif	//_INDUCING

/**********************************************************************/

int esa_write_all(int_suff* SA, int_lcp* LCP, t_TEXT *Text, char *c_file) {
	
	char c_aux[500];
	
	FILE *f_out;
	sprintf(c_aux, "%s.esa", Text->c_file);
	f_out = fopen(c_aux, "wb");//wb
	
	if(!f_out) perror("esa_write_all_induced");
	
	#if _PREFIX_ASSY
		int_suff begin=0;
		int_lcp  i_height = 0;
	#endif
	
	int8 bwt = 0;
	size_t i = 0;

	size_t pos = 0;
	int_text d = 0; 
	int_suff *D = (int_suff*) malloc((Text[i].length+1)*sizeof(int_suff));

	D[0] = d++;
	for(i = 1; i < Text->length; i++){

		if(Text->c_buffer[i]==0){
			D[i] = d++;
			pos = i;
		}
		else {
			D[i] = pos;
		}
	}

	i = 1;
	Text->length++;

	for(; i < Text->length; i++){
		
		#if _PREFIX_ASSY
			i_height += PREFIX_SIZE;		
			if(LCP[i] < i_height) i_height = LCP[i];
			begin = SA[i] + i_height;
		#endif
			
		
		#if _INDUCING
			if(Text->c_buffer[SA[i]] > Text->c_buffer[SA[i]+1]){
				
				LCP[i+1] = 0;
				
				#if _PREFIX_ASSY
					begin = SA[i];
					i_height = PREFIX_SIZE;
				#endif
			}
		#endif
			
		//write the node into the file
		fwrite(&SA[i], sizeof(int_suff), 1, f_out);
		#if IO_VOLUME
			Text->io_write++;
		#endif
		fwrite(&LCP[i], sizeof(int_lcp), 1, f_out);
		#if IO_VOLUME
			Text->io_write++;
		#endif
		
		#if _PREFIX_ASSY
			fwrite(&Text->c_buffer[begin], sizeof(int8), PREFIX_SIZE, f_out);
			#if IO_VOLUME
				Text->io_write++;
			#endif
		#endif
		
		//bwt
		bwt = 0;
		if(SA[i] > 0) bwt = Text->c_buffer[SA[i] - 1];	
		fwrite(&bwt, sizeof(int8), 1, f_out);
		#if IO_VOLUME
			Text->io_write++;
		#endif
		
		int_text d;
		int_suff sa;

		if(Text->c_buffer[SA[i]]==0){
			d = D[SA[i]]-1;
			sa = SA[i]-D[SA[i]-1]-1;
		}
		else{
			d = D[D[SA[i]]];
			sa = SA[i]-D[SA[i]]-1;	
		}
		if(!d) sa++;
		d += Text->n_start;

		fwrite(&d, sizeof(int_text), 1, f_out);
		#if IO_VOLUME
			Text->io_write++;
		#endif
		fwrite(&sa, sizeof(int_suff), 1, f_out);
		#if IO_VOLUME
			Text->io_write++;
		#endif
	}
	
	//extra node [n-1, 0, 500] SENTINEL
	t_ESA aux_gsa;
	aux_gsa.sa = Text->length-1; //FELIPE
	aux_gsa.lcp = 0;
	
	aux_gsa.text = 0;
	aux_gsa.sa_prime = 1;
	
	#if _PREFIX_ASSY
		for(i=0; i < PREFIX_SIZE; i++)
			aux_gsa.prefix[i] = SIGMA;
	#endif
	aux_gsa.bwt = 0;
	
	fwrite(&aux_gsa, sizeof(t_ESA), 1, f_out);
	#if IO_VOLUME
		Text->io_write++;
	#endif
	if(fclose(f_out)==EOF) printf("error closing file %s.\n\n\n", c_aux); 
	
	free(D);
	
return 0;
}

/**********************************************************************/

int esa_build(t_TEXT *Text, int_text k, int sigma, char* c_file){

	int_suff *SA;
	int_lcp *LCP;
	int_suff j = 0;
	
	int_text i;
	for(i = 0; i < k; i++){
	
		open_sequence(&Text[i], c_file);	//open bin		
		load_sequence(&Text[i]);			//load sequence	
		Text[i].key = i;
		#if IO_VOLUME
			Text[i].io_write = 0;
		#endif
		
		Text[i].block_esa_size = BLOCK_ESA_SIZE/k;
		
		#if DEBUG
			printf("T_%d\t%zu\n", i, Text[i].length);	
		#endif
			
		SA  = (int_suff*) malloc((Text[i].length+3) * sizeof(int_suff));
		if(!SA) perror("esa_build");
		
		LCP = (int_lcp*) malloc((Text[i].length+3) * sizeof(int_lcp));
		if(!LCP) perror("esa_build");

		for(j=0;j<Text[i].length+3;j++) SA[j] = LCP[j] = 0;
		
		/**************************************************************/
		#if DEBUG
			Text[i].c_buffer[j]=0;
		#endif

		gSACA_K((unsigned char*)Text[i].c_buffer, (int*)SA, Text[i].length+1, SIGMA, Text[i].length+1, sizeof(char), 0, 1); // 5n bytes

		//computes lcp in 13n bytes
		lcp_PHI(Text[i].c_buffer, SA, Text[i].length+1, LCP); // 13n bytes

		/**************************************************************/
		//validate
		#if DEBUG
		
		if(!check_sa(SA, Text[i].length+1, &Text[i]))
			printf("isNotSorted!!\n");
		else
			printf("isSorted!!\n");
			
		check_lcp_array(Text[i].c_buffer, SA, Text[i].length, LCP);
		printf("media(LCP) = %d\n", lcp_media(LCP, Text[i].length));
		
		#endif //DEBUG
		/**************************************************************/
		
		Text[i].c_buffer[Text[i].length] = SIGMA; 
		
		#if DEBUG
			esa_print_suff(SA, LCP, &Text[i],  min(Text[i].length+1,20));
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

int esa_merge(t_TEXT *Text, int_text k, size_t *size, char* c_file, int_text total, int_suff* COUNT, size_t n){
	
	*size = 0;
	
	//load all partition (beginning)
	heap *H = heap_alloc_induced(k, c_file, total, COUNT, n);
	int_text j = 0;
	for(; j < k; j++){
		
		Text[j].key = j;
		#if IO_VOLUME
			Text[j].io_read = 0;
		#endif
		Text[j].block_esa_size = BLOCK_ESA_SIZE/k;
		
		open_sequence(&Text[j], c_file);//open bin

		esa_open(&Text[j], "esa");				
		esa_malloc(&Text[j]);
		esa_read(Text[j].ESA, Text[j].f_ESA, Text[j].block_esa_size);
		
		#if DEBUG
			printf("T_%d\t%zu\n", j, Text[j].length);
			esa_print(&Text[j], 5);	printf("\n");
		#endif
		
		Text[j].u_idx = Text[j].i_height = Text[j].i_loaded = 0; 
	
		*size += Text[j].length;
	}

	//initialize the heap
	for(j = 0; j < k; j++){
		H->lcp_son_dad[j] = H->lcp_left_right[j] = 0;
		heap_insert(H, j, &Text[j], 0);		
	}
	
	heap_lcp(H, 0);
	printf("\n");
	printf("INDUCING:\n");
	printf("alfa\tTOTAL\tINDUCED\t\%%:\n");

	#if _INDUCING
		size_t induced_suffixes = 0;
		int8 alfa = 0;
	#endif
	
	size_t total_induced_suffixes = 0;
	#if _INDUCING
		#if DEBUG
		      size_t ant = 0;
		#endif
	#endif
	
	size_t i;
	for(i=0; i<*size; i++){
		//printf("%d)\n", i);
		#if _INDUCING
			if(H->heap[0]->c_buffer[0] > alfa){
			
				#if DEBUG
		      		printf("%c)\t%zu\t%zu\t%.2lf\n", alfa, i-ant, induced_suffixes, (100.0)*(induced_suffixes/(double) (i-ant)));
		      		ant = i;
				#endif

				induced_suffixes = 0;						
				alfa = H->heap[0]->c_buffer[0];	
				
				//RMQ
				for(j = 1; j < SIGMA; j++) H->lcp_induced[j] = 0;
							
				if(H->induced[alfa]){//induceds > 0
				
					induced_suffixes = heap_pass_induced(H, Text, &i, alfa);
					
					H->size = 0;
					for(j = 0; j < k; j++){
						H->lcp_son_dad[j] = H->lcp_left_right[j] = 0;
						heap_update(H, j, &Text[j], Text[j].u_idx);
					}
										
					heap_lcp(H, 0);
					if(H->heap[0]->c_buffer[0] > alfa){
						i--; continue;
					}
					
					H->lcp_son_dad[0] = 1;
					
				}
				total_induced_suffixes += induced_suffixes;
			}
		#endif	//_INDUCING
		
		heap_delete_min(H);			
	}
	#if _INDUCING
		#if DEBUG
			printf("%c)\t%zu\t%zu\t%.2lf\n", alfa, i-ant, induced_suffixes, (100.0)*(induced_suffixes/(double) (i-ant)));
		#endif
	#endif
	
	printf("ALL)\t%zu\t%zu\t%.2lf\n", i, total_induced_suffixes, (100.0)*(total_induced_suffixes/(double) (i)));
	
	#if _OUTPUT_BUFFER
		fwrite(H->out_buffer, sizeof(t_GSA), H->size_out_buffer, H->f_out_ESA);//fflush out_buffer
		#if IO_VOLUME
			Text[0].io_write++;
		#endif
	#endif
	
	#if IO_VOLUME
		size_t io_read_sum=0;
		size_t io_write_sum=0;
		for(j=0;j < k; j++){
			io_read_sum+=Text[j].io_read;
			io_write_sum+=Text[j].io_write;
		}
		size_t io_sum=io_write_sum+io_read_sum;
		
		printf("I/O)\t%zu\n", io_sum);
		printf("read\t%zu\n", io_read_sum);
		printf("write\t%zu\n", io_write_sum);
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

