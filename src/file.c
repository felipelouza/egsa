#include "file.h"


//read fasta and convert it (sequence.bin)
int preprocessing(t_TEXT *Text, char* c_dir, char *c_file, int_text k){ 
	
	printf("### PREPROCESSING ###\n");
	
	size_t begin = 0;
	#if INPUT_CAT
		size_t sum = 0;
	#endif
	
	size_t total = 0;
	int_text i = 0;
	for(; i < k; i++){
		
		file_open_fasta(&Text[i], Text[i].c_file, begin);
		
		begin = file_load_fasta(&Text[i]);
		file_write_sequence(&Text[i], c_dir, c_file);
		
		#if INPUT_CAT
			Text[i].n_start = sum;
			sum += Text[i].n_strings;	
			printf("T_%d\t%d string(s)\t%d bytes\n", i, Text[i].n_strings, Text[i].length);		
		#else
			printf("T_%d\t%d bytes\n", i,Text[i].length);		
		#endif

		free(Text[i].c_buffer);
		total += Text[i].length;
	}

	printf("\n");
	printf("TOTAL = %lu bytes\n", total);

return 0;
}

/**********************************************************************/

int file_open_input(t_TEXT **Text, char *c_dir, char *c_file, int_text *k, int_text *r){
	
	char c_aux[FILE_NAME];
	sprintf(c_aux, "%s%s", c_dir, c_file);
	
	printf("input = %s\n", c_aux);
	
	FILE* f_aux = fopen(c_aux, "r");	
	if(!f_aux) perror ("open_input");
	fseek(f_aux, 0, SEEK_SET);	
	
	#if INPUT_CAT
		
		*r = file_partition(c_dir, f_aux, *k, *r);
		fclose(f_aux);	
		
		sprintf(c_aux, "%sall.in", c_dir);
		f_aux = fopen(c_aux, "r");
		if(!f_aux) perror ("open_input");
		fseek(f_aux, 0, SEEK_SET);	
	
		*k = *r;			
	#endif
	
	*Text = (t_TEXT*) malloc(sizeof(t_TEXT) * (*k));
	
	int_text i = 0;
	for(; i < *k; i++){//file_name \t size
			
		(*Text)[i].c_file = (char*) malloc(FILE_NAME*sizeof(char));
		
		#if INPUT_CAT
			(*Text)[i].n_strings = 0;
		#endif
				
		fscanf(f_aux, "%s\n", c_aux);
		sprintf((*Text)[i].c_file, "%s%s", c_dir, c_aux);
		
	}

	fclose(f_aux);
	printf("\n");
	
return 0;
}

/**********************************************************************/

int_text file_get_size(char *c_dir, char *c_file, int_text k){
	
	int_text size = 0;
	
	char c_in[FILE_NAME];
	sprintf(c_in, "%s%s", c_dir, c_file);
	
	FILE* f_in = fopen(c_in, "r");	
	if(!f_in) perror ("open_input");
	fseek(f_in, 0, SEEK_SET);	
	
	
	char c_out[FILE_NAME] = "";
	sprintf(c_out, "%s%s.%d.bin", c_dir, c_file, k);

	FILE* f_out = fopen(c_out, "wb");	
	if(!f_out) perror ("open_input");
	fseek(f_out, 0, SEEK_SET);		

	#if PROTEIN 
		
		size_t j = 1;

		char c_aux[FILE_READ];
		fgets(c_aux, FILE_READ, f_in);
                        
		while(fgets(c_aux, FILE_READ, f_in)){
		
                        int8 tmp;
			if(c_aux[0]=='>'){                                                      
	                        tmp = 0;
        	                fwrite(&tmp, sizeof(int8), 1, f_out);
				size++;
				
                        	if(j >= k) break;	
				j++;

				continue;
			}

                        size_t i = 0;
                        for(; i < strlen(c_aux)-1; i++){
                        			
                        	tmp = map(c_aux[i]);
                        	if(tmp){
					fwrite(&tmp, sizeof(int8), 1, f_out);
					size++;
				}
                        }
                        
	                //size+=strlen(c_aux)-1;
		}
		//j++;
		if(c_aux[0]!='>'){                                                      
			int8 tmp = 0;
			fwrite(&tmp, sizeof(int8), 1, f_out);
		}
	#elif READ	

		size_t j = 0;

		char c_aux[FILE_NAME];
		while(fgets(c_aux, FILE_READ, f_in)){
		
			fgets(c_aux, FILE_READ, f_in);
			
			int8 tmp;
			size_t i = 0;
			for(; i < strlen(c_aux)-1; i++){
						
				tmp = map(c_aux[i]);
                        	if(tmp){
					fwrite(&tmp, sizeof(int8), 1, f_out);
					size++;
				}
			}

			tmp = 0;
			fwrite(&tmp, sizeof(int8), 1, f_out);
			size++;
			
			//size+=strlen(c_aux)-1;

			fgets(c_aux, FILE_READ, f_in);
			fgets(c_aux, FILE_READ, f_in);
			
			if(++j >= k) break;
		}
	#endif

	#if INPUT_CAT	
	if(j<k){
		size =0;
		remove_file(c_out);
	}
	#endif
	
	fclose(f_in);
	fclose(f_out);
	
return size;
}

/**********************************************************************/

int file_open_fasta(t_TEXT *Text, char* c_file, size_t begin){ 

	
	Text->f_in = fopen(c_file, "r");
	if(!Text->f_in) perror ("open_fasta");
	
	fseek (Text->f_in, 0, SEEK_END);
	Text->length = ftell (Text->f_in);

	Text->c_buffer = (int8*) malloc((Text->length+PREFIX_SIZE) * sizeof(int8));

	fseek (Text->f_in , begin , SEEK_SET);	
	
return 0;
}

int file_load_fasta(t_TEXT *Text) { 

	char c_aux[FILE_READ];
	
	fgets(c_aux, FILE_READ, Text->f_in);//ignores first line
	Text->length = 0;
	
	size_t j = 0;

	
	#if PROTEIN
		Text->n_strings=1;
	#elif READ		
		Text->n_strings=0;
	#endif

	//read the sequence
	while(fgets(c_aux, FILE_READ, Text->f_in)){
		
		//printf("%s", c_aux);
		
		#if INPUT_CAT
		
			#if PROTEIN
				if(c_aux[0] == '>'){
					
					Text->c_buffer[j++] = 0;
					
					Text->n_strings++;
					continue;
				}
			#elif READ
		
				if(c_aux[0] == '+'){
					
					Text->c_buffer[j++] = 0;
					
					fgets(c_aux, FILE_READ, Text->f_in);
					fgets(c_aux, FILE_READ, Text->f_in);
				
					Text->n_strings++;
					continue;
				}
			#endif
			
		#endif
		
		size_t i = 0;	
		for(; i < strlen(c_aux) - 1; i++){
			
			int8 tmp = map(c_aux[i]);
			if(tmp)
				Text->c_buffer[j++] = tmp;				
		}
		
	}
	
	
	#if PROTEIN	
		Text->length = j;
	#else
		Text->length = j-1;
	#endif

	fclose(Text->f_in);
	//free(Text->c_file);
	
	/*	
	int i = 0;	
	for(; i < j+1; i++)
		printf("%d|", Text->c_buffer[i]);
	
	printf("\n");
	*/
	
return 0;
}

/**********************************************************************/

int file_partition(char *c_dir, FILE *f_in, int_text k, int_text r){
	
	char c_in[FILE_NAME];
	sprintf(c_in, "%sall.in", c_dir);
	
	FILE* f_all = fopen(c_in, "w");	
	if(!f_all) perror ("open_input");
	fseek(f_all, 0, SEEK_SET);


	char c_aux[FILE_READ]; //, c_tmp[FILE_READ];
	//size_t min = 10000, max = 0, mean = 0;

	sprintf(c_aux, "%spartition/", c_dir);
	mkdir(c_aux);

	//int line = 0;
	int_text i = 0;	
	int_text total = 0;
	for(; i< r && total < k; i++){
	
		sprintf(c_in, "%spartition/%d.fasta", c_dir, i);
		
		FILE* f_aux = fopen(c_in, "w");	
		if(!f_aux) perror ("partition");
		fseek(f_aux, 0, SEEK_SET);
		
		sprintf(c_in, "partition/%d.fasta", i);
		fprintf(f_all,"%s\n", c_in);
		
		//size_t length = 0;

		#if PROTEIN
			int_text j=0;
			if(c_aux[0]=='>'){
				fprintf(f_aux,"%s", c_aux);
				j=1;
			}

			while(fgets(c_aux, FILE_READ, f_in)){

				if(c_aux[0]=='>'){	
					total++;
					if((j>=(k/r)+(k%r>0)) || (total > k)){
						break;
					}
					j++;
				}
				fprintf(f_aux,"%s", c_aux);
			}

		#elif READ	
			int_text j=0;
			while(fgets(c_aux, FILE_READ, f_in)){
						
				fprintf(f_aux,"%s", c_aux);
			
				fgets(c_aux, FILE_READ, f_in);
				fprintf(f_aux,"%s", c_aux);
			
				fgets(c_aux, FILE_READ, f_in);
				fprintf(f_aux,"%s", c_aux);
			
				fgets(c_aux, FILE_READ, f_in);
				fprintf(f_aux,"%s", c_aux);
			
				total++;
				j++;
				if((j>=(k/r)+(k%r>0)) || (total >= k)){
					break;
				}
			}
		#endif
		fclose(f_aux);
	}	
	fclose(f_all);
	
	/*
	printf("(max, min, mean) = (%u, %u, %u)\n", max, min, mean/k);
	printf("length = %.2lf MB\n", mean/pow(2,20));	
	*/
return i;	
}

/**********************************************************************/

int open_sequence(t_TEXT *Text, char *c_dir, char *c_file){
	
	char c_aux[500];
	
	sprintf(c_aux, "%s.bin", Text->c_file);
		
	Text->f_in = fopen(c_aux, "rb");
	if (!Text->f_in) perror ("open_sequence");

	fseek (Text->f_in, 0, SEEK_END);
    
    	Text->length = ftell(Text->f_in);
	Text->length--;// remove #-suffix to be sorted
	
return 0;
}

inline void seek_sequence(FILE *File, size_t pos){
	
	fseek (File, pos, SEEK_SET);
}

int8 read_sequence(t_TEXT *Text){
	
	int8 aux = -1;
	fread (&aux, sizeof(int8), 1, Text->f_in);
	
return aux;
}

void load_sequence(t_TEXT *Text) {//load .bin file

	Text->c_buffer = (int8*) malloc((Text->length+PREFIX_SIZE+1) * sizeof(int8));
	if(!Text->c_buffer) perror("load_sequence");
	
	fread (Text->c_buffer, sizeof(int8), Text->length+1, Text->f_in);
	
	size_t i =  Text->length+1;
	for(; i< Text->length+PREFIX_SIZE+1;i++)
		Text->c_buffer[i] = SIGMA+1;
	
	fclose(Text->f_in);
}

int file_write_sequence(t_TEXT *Text, char *c_dir, char* c_file) {
	
	char c_aux[FILE_NAME] = "";
	sprintf(c_aux, "%s.bin", Text->c_file);
	
	FILE *f_out;
	
	f_out = fopen(c_aux, "wb");
		
	if (!f_out) perror ("write_sequence");	
	Text->c_buffer[Text->length] = 0;
	
	Text->c_buffer[++Text->length] = SIGMA+1;
	fwrite(Text->c_buffer, sizeof(int8), Text->length+1, f_out);
		
	fclose(f_out);	

return 0;
}

/**********************************************************************/

inline void remove_file(char* c_file){
	
	printf("removing: %s\n", c_file);
	
	char c_aux[500];
	
	strcpy (c_aux,"rm ");
	//strcat (c_aux, c_dir);
	strcat (c_aux, c_file);
	
	system (c_aux);//remove .bin

}
/**********************************************************************/

inline void mkdir(char* c_file){
	
	printf("mkdir: %s\n", c_file);
	
	char c_aux[500];
	
	strcpy (c_aux,"mkdir -p ");
	//strcat (c_aux, c_dir);
	strcat (c_aux, c_file);
	
	system (c_aux);//remove .bin

}

/**********************************************************************/

int8 map(int8 c_aux){
	

	#if DNA || READ
	switch (c_aux) {
		case 65: return 1; break;//A
		case 67: return 2; break;//C
		case 71: return 3; break;//G
		case 84: return 4; break;//T
		default: return 0; break;//N
	}
	
	#elif PROTEIN
	
	switch (c_aux) {
		
		
		case 65: return 1; break;//A
		//B
		case 67: return 2; break;//C
		case 68: return 3; break;//D
		
		case 69: return 4; break;//E
		case 70: return 5; break;//F
		case 71: return 6; break;//G
		
		case 72: return 7; break;//H
		case 73: return 8; break;//I
		//J
		case 75: return 9; break;//K
		
		case 76: return 10; break;//L
		case 77: return 11; break;//M
		case 78: return 12; break;//N
		//O
		case 80: return 13; break;//P
		case 81: return 14; break;//Q
		case 82: return 15; break;//R
		
		case 83: return 16; break;//S
		case 84: return 17; break;//T
		//U
		case 86: return 18; break;//V
		
		case 87: return 19; break;//W
		case 88: return 20; break;//X
		case 89: return 21; break;//Y
		//Z
		
		default: return 0; break;//N
	}	
	
	#endif		
}

/**********************************************************************/
