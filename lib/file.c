#include "file.h"

#define BUFFER_SIZE 4098

#ifndef MAC_OS
  #define MAC_OS 0
#endif

/**********************************************************************/

/* Returns the file extension
 */
const char *get_filename_ext(const char *filename) {                                                  
    const char *dot = strrchr(filename, '.');                                                         
    if(!dot || dot == filename) return "";                                                            
    return dot + 1;                                                                                   
}   

/**********************************************************************/

/* Changes to a working directory, where everything will be read
 * from and written to:
 */ 

int file_chdir(char* dir){

	//printf("change to: %s\n", dir);
	
	//char* oldwd = getcwd(NULL,0);
	char *tmp = malloc(sizeof(char)*4096);
	char* oldwd = getcwd(tmp,4096);
	if (!oldwd) die(__func__);
	if (chdir(dir) == -1) die(__func__);

	free(oldwd);
return 0;
}
/*******************************************************************/

//Open a file and returns a pointer
FILE* file_open(char *c_file, const char * c_mode){
	
	FILE* f_in;
	
	f_in = fopen(c_file, c_mode);
	if (!f_in) perror ("file_open");

	fseek(f_in, 0, SEEK_SET);
	
return f_in;
}


int file_close(FILE* f_in){
	
	fclose(f_in);
	if (!f_in) perror ("file_close");
	
return 0;
}

/**********************************************************************/
// read line by line
int_text load_multiple_txt(FILE* f_in, char *c_file, int_text *k) {

	size_t sum=0;
	int_text r = 0;
	int8 sentinel = SIGMA;
	int8 separator = 0;
	
	FILE *f_out=NULL;
	
	char c_aux[FILE_NAME] = "";
	sprintf(c_aux, "partition/%s.%d.bin", c_file, r);
	
	f_out = fopen(c_aux, "wb");		
	if (!f_out) perror ("write_sequence");
	
	ssize_t size = 0;			
	int_text i;
 	for(i=0; i<*k; i++){
		
		if (feof(f_in)){
			//return 0;
			*k = i;
			break;		
		}
		
    #if MAC_OS
  		char c_buffer[BUFFER_SIZE];
      fgets(c_buffer, BUFFER_SIZE, f_in);
      size=strlen(c_buffer);
		  if(feof(f_in)){
			  i--;
			  continue;
      }
    #else
		  size_t len = 0;
  		unsigned char *c_buffer = NULL; len=0;
	  	size = getline((char **)&c_buffer, &len, f_in); // read line  
		  if(size<=1){
		  	i--;
		  	free(c_buffer);		
		  	continue;
		  }
    #endif
    
    #if DEBUG
    printf("==> %s", c_buffer);
    #endif
		
		c_buffer[size-1] = 0;
		sum += size;

		if(sum>RAM_USE/WORKSPACE && i){

			fwrite(&sentinel, sizeof(int8), 1, f_out);			
			fclose(f_out);
	
			r++;
			sum=size;
			
			sprintf(c_aux, "partition/%s.%d.bin", c_file, r);
			
			f_out = fopen(c_aux, "wb");		
			if (!f_out) perror ("write_sequence");
		}
		
		size_t j;
		for(j=0; j<size-1;j++)
			if(c_buffer[j]<SIGMA-2 && c_buffer[j]>0)
				fwrite(&c_buffer[j], sizeof(int8), 1, f_out);
			else sum--;	
		fwrite(&separator, sizeof(int8), 1, f_out);

		//fwrite(c_buffer, sizeof(int8), size, f_out);

    #if MAC_OS==0
		  free(c_buffer);
    #endif
		
		if (feof(f_in)){
			//return 0;
			*k = i;
			break;		
		}
	}

	r++;
	fwrite(&sentinel, sizeof(int8), 1, f_out);			
	fclose(f_out);
	
return r;
}

// read sequences separeted by '@' line
int_text load_multiple_fastq(FILE* f_in, char *c_file, int_text *k){

	size_t sum=0;
	int_text r = 0;
	int8 sentinel = SIGMA;
	
	FILE *f_out=NULL;
	
	char c_aux[FILE_NAME] = "";
	sprintf(c_aux, "partition/%s.%d.bin", c_file, r);
	
	f_out = fopen(c_aux, "wb");		
	if (!f_out) perror ("write_sequence");
	
	ssize_t size = 0;			
	int i;
 	for(i=0; i<*k; i++){

		if (feof(f_in)){
			*k = i;
			break;		
		}
		

    #if MAC_OS
  		char buf[BUFFER_SIZE];
      fgets(buf, BUFFER_SIZE, f_in);
		  if(feof(f_in)){
		  	*k = i;
		  	break;		
      }
    #else
		  size_t len = 0;
		  char *buf = NULL;
		  size = getline(&buf, &len, f_in); // @'s line
      if (size <= 1){ //number of strings less than k
		  	free(buf);
		  	*k = i;
		  	break;		
		  }
		  free(buf);
    #endif

		/**/
    #if MAC_OS
      char c_buffer[BUFFER_SIZE];
      fgets(c_buffer, BUFFER_SIZE, f_in);
      size=strlen(c_buffer);
    #else
      char *c_buffer = NULL; len=0;
		  size = getline(&c_buffer, &len, f_in); // read line
    #endif
	  c_buffer[size-1] = 0;

    #if DEBUG
    printf("==> %s\n", c_buffer);
    #endif

    sum += size;

		if(sum>RAM_USE/WORKSPACE && i){

			fwrite(&sentinel, sizeof(int8), 1, f_out);			
			fclose(f_out);
	
			r++;
			sum=size;
			
			sprintf(c_aux, "partition/%s.%d.bin", c_file, r);
			
			f_out = fopen(c_aux, "wb");		
			if (!f_out) perror ("write_sequence");
		}
		
		fwrite(c_buffer, sizeof(int8), size, f_out);

    #if MAC_OS
      fgets(buf, BUFFER_SIZE, f_in);
      fgets(buf, BUFFER_SIZE, f_in);
    #else
		  buf=NULL; len=0;
		  getline(&buf, &len, f_in); // +'s line
		  free(buf);
		  buf=NULL; len=0;
		  getline(&buf, &len, f_in); // @'s line

		  free(buf);
		  free(c_buffer);	
    #endif
	}

	r++;
	fwrite(&sentinel, sizeof(int8), 1, f_out);			
	fclose(f_out);

return r;
}

// read sequences separeted by '>' line
int_text load_multiple_fasta(FILE* f_in, char *c_file, int_text *k){

	size_t sum=0;
	int_text r = 0;
	int8 sentinel = SIGMA;
	
	FILE *f_out=NULL;
	
	char c_aux[FILE_NAME] = "";
	sprintf(c_aux, "partition/%s.%d.bin", c_file, r);
	
	f_out = fopen(c_aux, "wb");		
	if (!f_out) perror ("write_sequence");
	
  size_t len = 0;

  #if MAC_OS
    char buf[BUFFER_SIZE];
    fgets(buf, BUFFER_SIZE, f_in);
  #else
  	char *buf = NULL;
  	getline(&buf, &len, f_in);// first sequence '>'
  	free(buf);
  #endif

	int i;
 	for(i=0; i<*k; i++){
		
		if (feof(f_in)){
			*k = i;
			break;		
		}
		
		size_t nalloc = BUFFER_SIZE;
		char *c_buffer = (char*) malloc(nalloc*sizeof(char));

		size_t p=0;
    #if MAC_OS
      char buf[BUFFER_SIZE+1];
      while(fgets(buf, BUFFER_SIZE, f_in)!=NULL){
      len = strlen(buf);
    #else
		  buf=NULL; len=0;
  		while(getline(&buf, &len, f_in)!=-1){
    #endif

			  if(buf[0] == '>'){
			  	break;
			  }

			  if(p+len>nalloc){
			  	nalloc += len+128;
			  	c_buffer= (char*) realloc(c_buffer, sizeof(char) * nalloc);
			  }

			  strcpy(&c_buffer[p], buf);
        #if MAC_OS
			    p+=strlen(buf);
        #else
			    p+=strlen(buf)-1;
        #endif


			  //breaks the string larger than the available memory
			  if(p>RAM_USE/WORKSPACE){
			  //printf("%d\t%zu: pruned string\n", i, p);
          #if MAC_OS
  			    char buf[BUFFER_SIZE];	
	  		  	while(fgets(buf, BUFFER_SIZE, f_in)!=NULL){
          #else
  			  	buf=NULL; len=0;
	  		  	while(getline(&buf, &len, f_in)!=-1){
          #endif
			  	    if(buf[0] == '>') break;
		  	  	}
			  break;
			  }
		}//end while

    #if MAC_OS
      p--;
    #else
  		free(buf);
    #endif

		c_buffer[p++] = 0;
		sum += p;

		if(sum>RAM_USE/WORKSPACE && i){

			fwrite(&sentinel, sizeof(int8), 1, f_out);			
			fclose(f_out);
	
			r++;
			sum=p;
			
			sprintf(c_aux, "partition/%s.%d.bin", c_file, r);
			
			f_out = fopen(c_aux, "wb");		
			if (!f_out) perror ("write_sequence");
		}
	

//removes symbol N
/*
		size_t j;
		for(j=0; j<p;j++)
			if(c_buffer[j]!='N') fwrite(&c_buffer[j], sizeof(int8), 1, f_out);
			else sum--;
*/
	  #if DEBUG
      printf("==> %s\n", c_buffer);
    #endif
		fwrite(c_buffer, sizeof(int8), p, f_out);
		free(c_buffer);
	}

	r++;
	fwrite(&sentinel, sizeof(int8), 1, f_out);			
	fclose(f_out);
	
return r;
}

/*******************************************************************/

size_t file_size(FILE* f_in){
	
	fseek(f_in, 0, SEEK_END);
	size_t length = (size_t) ftell (f_in);
	rewind(f_in);

return length;
}

/*******************************************************************/

int_text file_count_separators(FILE* f_in, int_suff length){
	
	int_text count = 0;
	char c;

	while((c = fgetc(f_in))!=EOF) if(c==0) count++;

	rewind(f_in);
		
return count;
}

/*******************************************************************/

void file_count_symbols(FILE* f_in, int_suff length, int_suff* COUNT){
	
	int c;
	while((c = fgetc(f_in))!=EOF) COUNT[c]++;
	
	rewind(f_in);
}

/*******************************************************************/
//read file and convert it (*.bin)
/* .txt   - strings per line
 * .fasta - strings separated by '>' line
 * .fastq - strings separated by four lines
 */
size_t preprocessing(t_TEXT **Text, char *c_file, int_text *k, int_suff *COUNT){ 


	printf("\n");
	printf("### PREPROCESSING ###\n");

	/**/
	
	mkdir("partition/");
	int_text r=0;

	FILE* f_in = file_open(c_file, "rb");
	if(!f_in) return 0;

	const char *type = get_filename_ext(c_file);


	if(*k==0) *k=INT_MAX;

	if(strcmp(type,"txt") == 0)
		r = load_multiple_txt(f_in, c_file, k);

	else if(strcmp(type,"fasta") == 0)
		r = load_multiple_fasta(f_in, c_file, k);

	else if(strcmp(type,"fastq") == 0)
		r = load_multiple_fastq(f_in, c_file, k);
	else{
		printf("Error: file not recognized (.txt, .fasta, .fastq)\n");
		return 0;
	}

	printf("K = %d\n", *k);

	fclose(f_in);

	if(r==0){
		return 0;
	}
	/**/
	
	size_t sum = 0;
	size_t total = 0;
	*Text = (t_TEXT*) malloc(sizeof(t_TEXT) * (r));
	
	int_text i;
	for(i=0; i<r; i++){//file_name \t size
			
		(*Text)[i].c_file = (char*) malloc(FILE_NAME*sizeof(char));
		//(*Text)[i].n_strings = 0;

		sprintf((*Text)[i].c_file, "partition/%s.%d.bin", c_file, i);
		
		FILE* f_in = file_open((*Text)[i].c_file, "r");
		
		(*Text)[i].length = file_size(f_in);
		
		file_count_symbols(f_in, (*Text)[i].length, COUNT);
		//(*Text)[i].n_strings = COUNT[0];
		(*Text)[i].n_strings = file_count_separators(f_in, (*Text)[i].length);
		
		(*Text)[i].n_start = sum;
		sum += (*Text)[i].n_strings;
		total += (size_t)(*Text)[i].length;

		fclose(f_in);
		
		#if DEBUG
			printf("%s \t%zu\t%d\t%d\n",(*Text)[i].c_file, (*Text)[i].length, (*Text)[i].n_strings, (*Text)[i].n_start);
		#endif

	}
	#if DEBUG
		printf("\n");
	#endif

	//printf("R = %d\nc_file = %s\n", r, c_file);
	*k = r;
	printf("PARTITIONS = %d\n", r);
	
return total;
}

/**********************************************************************/

int open_sequence(t_TEXT *Text, char *c_file){
				
	Text->f_in = fopen(Text->c_file, "rb");
	if (!Text->f_in) perror ("open_sequence");

	fseek (Text->f_in, 0, SEEK_END);

  	Text->length = (size_t) ftell(Text->f_in);
	rewind(Text->f_in);

	Text->length--;// remove #-suffix to be sorted
	
return 0;
}

void seek_sequence(FILE *File, size_t pos){
	
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
		Text->c_buffer[i] = SIGMA;

	fclose(Text->f_in);
}

/**********************************************************************/

void remove_file(char* c_file){
	
	char c_aux[500];
	
	strcpy (c_aux,"rm ");
	strcat (c_aux, c_file);
	
	system (c_aux);//remove .bin
}
/**********************************************************************/

void mkdir(char* c_file){
	
	char c_aux[500];
	
	strcpy (c_aux,"mkdir -p ");
	strcat (c_aux, c_file);
	
	system (c_aux);//remove .bin
}

/**********************************************************************/
