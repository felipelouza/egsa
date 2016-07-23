/*

Indexed binary heap.  Heap property is minimum on c_buffers.  
Keys in the heap must be distinct in the range [0,n). 
Weights may be updated through suffixes.

g.p.telles, jan 2012.

*/

#include "heap.h"

/**********************************************************************/
heap* heap_alloc_induced(int k, char *c_file, int_text total, int_suff* COUNT, size_t n) {
   
	int i;
	
	heap* h = (heap*) malloc(sizeof(heap));
	h->heap = (heap_node**) malloc(k * sizeof(heap_node*));
	
	if(!h) perror("heap_alloc_induced(h)");
	if(!h->heap) perror("heap_alloc_induced(h->heap)");
	
	h->lcp_son_dad = (int_lcp*) malloc((k+2) * sizeof(int_lcp));
	h->lcp_left_right = (int_lcp*) malloc((k+2) * sizeof(int_lcp));
	
	for(i=0; i < k+2; i++)
		h->lcp_son_dad[i] = h->lcp_left_right[i] = 0;
	
	
	if(!h->lcp_son_dad) perror("heap_alloc_induced(h->lcp_son_dad)");
	if(!h->lcp_left_right) perror("heap_alloc_induced(h->lcp_left_right)");
	
	#if _OUTPUT_BUFFER
		h->out_buffer = (t_GSA*) malloc( (OUTPUT_SIZE+1) * sizeof(t_GSA));
		if(!h->out_buffer) perror("heap_alloc_induced(h->out_buffer)");	
		h->size_out_buffer = 0;
	#endif
	
	#if _INDUCING
		#if INDUCED_BUFFER	
			for(i = 1; i < SIGMA; i++){//**
				
				h->size_induced_buffer[i] = ((double)COUNT[i]/n)*(INDUCED_SIZE)+1;
				h->inserted_induced_buffer[i] = 0;			
				h->induced_buffer[i] = (t_INDUCED*) malloc(h->size_induced_buffer[i]*sizeof(t_INDUCED));
			}
		#endif
	#endif
	
	h->size = 0;
	mkdir("tmp/");
	
	for(i = 0; i < SIGMA; i++){
			
		sprintf(h->cSIGMA[i], "tmp/%s.%d.esa.tmp", c_file, i);
			
		h->fSIGMA[i] = fopen(h->cSIGMA[i], "wb");
		if(!h->fSIGMA[i]) perror("heap_alloc_induced(h->fSIGMA)");
		fseek (h->fSIGMA[i] , 0 , SEEK_SET);
		
		#if _INDUCING
			h->induced[i] = 0;
			h->lcp_induced[i] = LCP_MAX;
		#endif
	}
	
	//GESA output
	char c_out_esa[FILE_NAME];
	sprintf(c_out_esa, "%s.%d.gesa", c_file, total);
	
	h->f_out_ESA = fopen(c_out_esa, "wb");
	if(!h->f_out_ESA) perror("heap_alloc_induced(h->f_out_ESA)");
	
	fseek(h->f_out_ESA , 0 , SEEK_SET);	

	#if BWT
		#if BWT_OUTPUT
		char c_out_bwt[FILE_NAME];
		sprintf(c_out_bwt, "%s.%d.bwt", c_file, total);

		h->f_out_BWT = fopen(c_out_bwt, "w");
	        if(!h->f_out_BWT) perror("heap_alloc_induced(h->f_out_BWT)");
	
	        fseek(h->f_out_BWT, 0 , SEEK_SET);
		#endif
	#endif
	
return h;
}

/**********************************************************************/

int heap_free(heap *h) {

	free(h->heap);
	
	int i = 0;
	for( ; i < SIGMA; i++){
		
		fclose(h->fSIGMA[i]);
	}
	
	free(h->lcp_left_right);
	free(h->lcp_son_dad);
	
	#if _OUTPUT_BUFFER
		free(h->out_buffer);
	#endif
	
	fclose(h->f_out_ESA);
	#if BWT
		#if BWT_OUTPUT
		fclose(h->f_out_BWT);
		#endif
	#endif	

	#if _INDUCING
		#if INDUCED_BUFFER	
			for(i = 1; i < SIGMA; i++){
				free(h->induced_buffer[i]);
			}
		#endif
	#endif

	free(h);
	
return 0;
}

/**********************************************************************/
void heapfy_up(heap *h, int i_son) {

int i_dad = (int) floor((i_son-1.0)/2.0);

	while (i_son > 0 && is_less(h, h->heap[i_son], h->heap[i_dad])) {
		
		//swap pointers on heap
		heap_node *auxn = h->heap[i_son];
		h->heap[i_son] = h->heap[i_dad];
		h->heap[i_dad] = auxn;
		
		i_son = i_dad;
		i_dad = (int) floor((i_son-1.0)/2.0);
		
	}
}

/**********************************************************************/
int heapfy_down(heap *h, int i_node) {


	//int_text last = h->size-1;

	int l = 2*i_node+1;//left child
	int r = l+1;//right child
	int i_son;
	
	// Selects the childs of i with smaller c_buffer:
	if (r < h->size){
		
		#if _LCP_COMPARISON
			if(h->lcp_son_dad[l] != h->lcp_son_dad[r])
				i_son = (h->lcp_son_dad[l] > h->lcp_son_dad[r]) ? l : r;
			else
				i_son = (is_less_left_right(h, h->heap[l], h->heap[r], &h->lcp_left_right[l])) ? l : r;
		#else
			int_lcp aux = 0;
			i_son = (is_less_left_right(h, h->heap[l], h->heap[r], &aux)) ? l : r;
		#endif
		
		
	}
	else if (l == h->size-1)
		i_son = l;
	else
		return 0;//no child

	
	int_lcp lcp_son_ant = h->lcp_son_dad[i_son];

	heap_node *aux;
	// Swaps down:
	while (is_less_down(h, h->heap[i_son], h->heap[i_node], &h->lcp_son_dad[i_son], h->lcp_son_dad[i_node])) {
		
		//#if _LCP_COMPARISON
	
		h->lcp_son_dad[i_node] = lcp_son_ant;
		
		if(i_son == l && r < h->size){
		
			h->lcp_son_dad[r] =  h->lcp_left_right[l];			
			compare(h, h->heap[l], h->heap[r], &h->lcp_son_dad[r]);			
		}
		else if(i_son == r){
			
			h->lcp_son_dad[l] =  h->lcp_left_right[l];
			compare(h, h->heap[l], h->heap[r], &h->lcp_son_dad[l]);
		}
		
		h->lcp_left_right[l] = min(h->lcp_son_dad[i_son], h->lcp_left_right[l]);
		
		//#endif

		aux = h->heap[i_node];
		h->heap[i_node] = h->heap[i_son];
		h->heap[i_son] = aux;

		i_node = i_son;
		l = 2*i_node+1;
		r = l+1;

		if (r < h->size){
			
			#if _LCP_COMPARISON
				if(h->lcp_son_dad[l] != h->lcp_son_dad[r])
					i_son = (h->lcp_son_dad[l] > h->lcp_son_dad[r]) ? l : r;
				else
					i_son = (is_less_left_right(h, h->heap[l], h->heap[r], &h->lcp_left_right[l])) ? l : r;
			#else
				i_son = (is_less_left_right(h, h->heap[l], h->heap[r], &h->lcp_left_right[l])) ? l : r;
			#endif
				
		}
		else if (l == h->size-1){
			i_son = l;
		}
		else
			return 0;
		
		lcp_son_ant = h->lcp_son_dad[i_son];
	}
		
	#if _LCP_COMPARISON
		if(i_son == l && r < h->size){
			
			h->lcp_son_dad[r] = min(h->lcp_left_right[l], h->lcp_son_dad[l]);
			compare(h, h->heap[r], h->heap[i_node], &h->lcp_son_dad[r]);
		}
		else if(i_son == r){
			
			h->lcp_son_dad[l] = min(h->lcp_left_right[l], h->lcp_son_dad[r]);
			compare(h, h->heap[l], h->heap[i_node], &h->lcp_son_dad[l]);
		}
	#endif
	

	
return 0;	
}

int heap_lcp(heap *h, int i_node) {//updates lcp

	int l = 2*i_node+1;//left child
	int r = l+1;//right child
	
	if (l < h->size){
		
		compare(h, h->heap[i_node], h->heap[l], &h->lcp_son_dad[l]);		
		heap_lcp(h, l);		
	}
	if (r < h->size){
		
		compare(h, h->heap[i_node], h->heap[r], &h->lcp_son_dad[r]);
		heap_lcp(h, r);
	}
	
	return 0;//nao tem filho
}

/**********************************************************************/

int heap_insert(heap *h, int key, heap_node* node, unsigned u_idx) {
	
	//insert key in the end of heap
	h->heap[h->size] = node;

	#if _PREFIX_ASSY
		memcpy(node->c_buffer, node->ESA[u_idx].prefix, PREFIX_SIZE);
		node->i_loaded = node->i_height = PREFIX_SIZE;
		node->c_buffer[PREFIX_SIZE] = SIGMA;
	#else
		load_buffer(h, node, &node->c_buffer, 0);
		node->c_buffer[1] = SIGMA;
	#endif
	
	h->size++;
	
return 0;
}

int heap_update(heap *h, int key, heap_node* node, unsigned u_idx) {
	
	//insert key in the end of heap
	h->heap[h->size] = node;
	
	if(!node->ESA[u_idx].lcp){

	#if _PREFIX_ASSY
		memcpy(node->c_buffer , node->ESA[u_idx].prefix, PREFIX_SIZE);
		node->i_loaded = node->i_height = PREFIX_SIZE;
		node->c_buffer[PREFIX_SIZE] = SIGMA;
	#else
		load_buffer(h, node, &node->c_buffer, 0);
		node->c_buffer[1] = SIGMA;
	#endif
		
	}
	
	heapfy_up(h, h->size++);

return 0;
}

/*******************************************************************************/

int heap_delete_min(heap *h) {//outputs min
	
	heap_node *node = h->heap[0];
	
	#if _INDUCING
		int j;
		for(j=1; j<SIGMA; j++)//RMQ
			if(h->lcp_induced[j] > h->lcp_son_dad[0]) h->lcp_induced[j] = h->lcp_son_dad[0];
	#endif
			
	esa_write_induced(h, node, node->c_buffer[0], h->lcp_son_dad[0]);

		
	if(++node->u_idx == node->block_esa_size){ //BLOCK_ESA_SIZE
		node->u_idx = 0;
		esa_read(node->ESA, node->f_ESA, node->block_esa_size);
		#if IO_VOLUME
			node->io_read++;
		#endif
	}
	
	if(node->ESA[node->u_idx].lcp < node->i_height)
		node->i_height = node->ESA[node->u_idx].lcp;
		
	#if _PREFIX_ASSY
	
		if(node->i_height < C_BUFFER_SIZE){
			
			int_lcp aux = PREFIX_SIZE;		
			if(node->i_height + PREFIX_SIZE > C_BUFFER_SIZE) aux = C_BUFFER_SIZE - node->i_height;

			memcpy(node->c_buffer + node->i_height, node->ESA[node->u_idx].prefix, aux);
			node->c_buffer[node->i_height+aux] = SIGMA;
			
			node->i_height += aux;
		}

		node->i_loaded = node->i_height;
	#else
		
		#if _LCP_COMPARISON
			seek_sequence(node->f_in, (size_t) node->ESA[node->u_idx].sa + (size_t) node->i_height);
			fread (&node->c_buffer[node->i_height], sizeof(int8), C_BUFFER_SIZE - node->i_height, node->f_in);
			#if IO_VOLUME
				node->io_read++;
			#endif
			
			node->c_buffer[C_BUFFER_SIZE] = SIGMA;
			node->i_loaded = C_BUFFER_SIZE-1;			
		#else
			seek_sequence(node->f_in, (size_t) node->ESA[node->u_idx].sa);
			fread (&node->c_buffer[0], sizeof(int8), C_BUFFER_SIZE, node->f_in);
			#if IO_VOLUME
				node->io_read++;
			#endif
			
			node->c_buffer[C_BUFFER_SIZE] = SIGMA;
			node->i_loaded = C_BUFFER_SIZE-1;				
		#endif
		
	#endif
			
	
	//int j1; for(j1=0;j1<C_BUFFER_SIZE;j1++) printf("%d|", node->c_buffer[j1]); printf("\n\n");
	
	#if _PREFIX_ASSY
	#endif
	

	h->lcp_son_dad[0] = node->ESA[node->u_idx].lcp;
	
	#if _LCP_COMPARISON
		if(h->lcp_son_dad[0] <= h->lcp_son_dad[1] || h->lcp_son_dad[0] <= h->lcp_son_dad[2])
	#endif
		heapfy_down(h, 0);

return 0;
}

/**********************************************************************/

#if _INDUCING

int heap_pass_induced(heap *h, t_TEXT *Text, size_t *pos, int8 alfa) {

	//induced_buffer
	#if INDUCED_BUFFER
		fwrite(h->induced_buffer[alfa], sizeof(t_INDUCED), h->inserted_induced_buffer[alfa], h->fSIGMA[alfa]);
		#if IO_VOLUME
			Text[0].io_read++;
		#endif
	#endif //INDUCED_BUFFER
	
	*(pos) += h->induced[alfa];
	
	fclose(h->fSIGMA[alfa]);
	
	h->fSIGMA[alfa] = fopen(h->cSIGMA[alfa], "rb");
	if(!h->fSIGMA[alfa]) perror("heap_pass_induced");
	
	fseek (h->fSIGMA[alfa] , 0 , SEEK_SET);
	t_INDUCED induced;
	
	fread(&induced, sizeof(t_INDUCED), 1, h->fSIGMA[alfa]);		
	#if IO_VOLUME
		Text[0].io_read++;
	#endif
	induced.lcp = 0;
	
	size_t i;
	for(i=0; i < h->induced[alfa]; i++){
		
		int j;
		for(j=1; j< SIGMA; j++)//RMQ
			if(h->lcp_induced[j] > induced.lcp)
				h->lcp_induced[j] = induced.lcp;
			
		esa_write_induced(h, &Text[induced.text], alfa, h->lcp_induced[alfa]);
		
		
		h->lcp_induced[alfa] = LCP_MAX;
		
		if(++Text[induced.text].u_idx == Text[induced.text].block_esa_size){ //BLOCK_ESA_SIZE){
			Text[induced.text].u_idx = 0;
			esa_read(Text[induced.text].ESA, Text[induced.text].f_ESA, Text[induced.text].block_esa_size);
			#if IO_VOLUME
				Text[induced.text].io_read++;
			#endif
		}

		if(!feof(h->fSIGMA[alfa]))
		fread(&induced, sizeof(t_INDUCED), 1, h->fSIGMA[alfa]);			
		#if IO_VOLUME
			Text[0].io_read++;
		#endif
	}
	
return h->induced[alfa];
}
#endif //_INDUCING

/*******************************************************************************/

int load_buffer(heap *h, heap_node *node, int8** pt, int_lcp length){
	//printf("load_buffer\n");
	//loads buffer (c_buffer)
	seek_sequence(node->f_in, (size_t) node->ESA[node->u_idx].sa + (size_t) length);
	
	if(C_BUFFER_SIZE - (int) length > 0){
		
		fread (*pt, sizeof(int8), C_BUFFER_SIZE - length, node->f_in);
		#if IO_VOLUME
			node->io_read++;
		#endif
		node->c_buffer[C_BUFFER_SIZE] = SIGMA;
		node->i_loaded = C_BUFFER_SIZE-1;
	}
	else{//if the first buffer is full
	
		fread (node->c_overflow, sizeof(int8), C_OVERFLOW_SIZE, node->f_in);
		#if IO_VOLUME
			node->io_read++;
		#endif
		node->c_overflow[C_OVERFLOW_SIZE] = SIGMA;
		*pt = node->c_overflow;
		
	}

return 0;	
}


/**********************************************************************/

int is_less_down(heap *h, heap_node *node_son, heap_node *node_dad, int_lcp* lcp_AB, int_lcp lcp_AD){
	
	#if _LCP_COMPARISON	
		if(lcp_AD < (*lcp_AB)){
			
			(*lcp_AB) = lcp_AD;
			return 1;
		}
		else if(lcp_AD > (*lcp_AB)){
			return 0;
		}
			
		int_lcp min = min(node_dad->i_loaded, node_son->i_loaded);
		if((*lcp_AB) > min){
			(*lcp_AB) = min;
		}
		
	#else
		(*lcp_AB) = 0;
	#endif

	if(*lcp_AB) (*lcp_AB)--;
	
	int8 *p1 = &node_son->c_buffer[*lcp_AB];
	int8 *p2 = &node_dad->c_buffer[*lcp_AB];
	
	if((*p1) == SIGMA && (*p2) == SIGMA){
		return 0;
	}			
	
	while(*p1 && *p2){
			
		if(*p1 < *p2) return 1;
		else if (*p1 > *p2) return 0;
		
		(*lcp_AB)++; 
	
		p1++; p2++;

		if(*p1 == SIGMA){
			load_buffer(h, node_son, &p1, *lcp_AB);
		}
		if(*p2 == SIGMA){
			load_buffer(h, node_dad, &p2, *lcp_AB);
		}
	}
	
	if(!(*p1) && !(*p2)){
		//	(*lcp_AB)++;
		return (node_son->key < node_dad->key ? 1: 0);
	}
	else{
		return ((*p1) < (*p2) ? 1: 0);
	}
}

int is_less_left_right(heap *h, heap_node *node_left, heap_node *node_right, int_lcp* lcp_BD){

	
	#if _LCP_COMPARISON
		int_lcp min = min(node_left->i_loaded, node_right->i_loaded);
		
		if((*lcp_BD) > min){
			(*lcp_BD) = min;
		}
	#else
		(*lcp_BD) = 0;
	#endif

	if(*lcp_BD) (*lcp_BD)--;

	int8 *p1 = &node_left->c_buffer[*lcp_BD];
	int8 *p2 = &node_right->c_buffer[*lcp_BD];
	
	if((*p1) == SIGMA && (*p2) == SIGMA){
		return 0;
	}
	
	while(*p1 && *p2){
	
		if(*p1 < *p2) return 1;
		else if (*p1 > *p2) return 0;

		(*lcp_BD)++; 
		
		p1++; p2++;

		if(*p1 == SIGMA){
			load_buffer(h, node_left, &p1, *lcp_BD);
		}
		if(*p2 == SIGMA){
			load_buffer(h, node_right, &p2, *lcp_BD);
		}
	}
		
	if(!(*p1) && !(*p2)){
		return (node_left->key < node_right->key ? 1: 0);
	}
	else{
		return ((*p1) < (*p2) ? 1: 0);
	}
}

int compare(heap *h, heap_node *node1, heap_node *node2, int_lcp* lcp){//[left, right], [son, dad]
	
	#if _LCP_COMPARISON
		int_lcp min = min(node1->i_loaded, node2->i_loaded);

		if((*lcp) > min){
			(*lcp) = min;
		}
	#else
		(*lcp) = 0;
	#endif
	
	if(*lcp) (*lcp)--;
	
	int8 *p1 = &node1->c_buffer[*lcp];
	int8 *p2 = &node2->c_buffer[*lcp];
	
	if((*p1) == SIGMA && (*p2) == SIGMA){
		return 0;
	}

	while(*p1 && *p2){
	
		if(*p1 != *p2) return 0;
		
		(*lcp)++; 
		p1++; p2++;

		if(*p1 == SIGMA){
			load_buffer(h, node1, &p1, *lcp);
		}
		if(*p2 == SIGMA){
			load_buffer(h, node2, &p2, *lcp);
		}
	}
	//if(!(*p1) && !(*p2)){
		//(*lcp)++;
	//}
	
	return 0;
}

int is_less(heap *h, heap_node *node1, heap_node *node2){
	
	int_lcp i = 0;
	
	int8 *p1 = &node1->c_buffer[i];
	int8 *p2 = &node2->c_buffer[i];

	while(*p1 && *p2){
	
		if(*p1 < *p2) return 1;
		else if (*p1 > *p2) return 0;
		
		i++;
		p1++; p2++;

		if(*p1 == SIGMA){
			load_buffer(h, node1, &p1, i);
		}
		if(*p2 == SIGMA){
			load_buffer(h, node2, &p2, i);
		}
	}
	
	if(!(*p1) && !(*p2)){
		return (node1->key < node2->key ? 1: 0);
	}
	else{
		return ((*p1) < (*p2) ? 1: 0);
	}
}

/**********************************************************************/

int print_node(heap_node *node){

	printf("\n[%d, %d] (lcp = %d) [%d] mem: ", node->key, node->ESA[node->u_idx].sa, node->ESA[node->u_idx].lcp,  node->ESA[node->u_idx].bwt);
	
	int8 *pt = node->c_buffer;
	
	size_t i = 0;
	
	for( ; i < 2*C_BUFFER_SIZE+2; i++){
		
		if(i < C_BUFFER_SIZE){
			printf("|%d| ", *(pt++));
		}
		if(i == C_BUFFER_SIZE){
			printf("-%d- ", *(pt++));
			pt = node->c_overflow;
		}		
		if(i > C_BUFFER_SIZE){
			printf(" %d  ", *(pt++));
		}
	}

	printf("\n");

return 0;	
}

/**********************************************************************/
