/*
 * Felipe A. Louza, 29/06/2016
 * Remove header from .fasta file.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
/**********************************************************************/

int main(int argc, char **argv) {
	
        size_t K = 0;
        size_t N = 0;

	//argv[1] = INPUT
	if(argc>=3){
	        sscanf(argv[2], "%d", &N);//REMOVES 'N' SYMBOL
	}
	if(argc==4){
	        sscanf(argv[3], "%zu", &K);//NUMBER OF STRINGS
	}

	FILE *f_in=NULL;	
	f_in = fopen(argv[1], "r");		
	if (!f_in) perror ("INPUT");
	
	char *buf = NULL;
	size_t len = 0, k=0;
	
	getline(&buf, &len, f_in);// first sequence '>'
	free(buf);

	char *c_buffer;
	while(!feof(f_in)){
		
		len = 0;
		int nalloc = 2048;
		c_buffer = (char*) malloc(nalloc*sizeof(char));

		size_t p=0;
		while(getline(&buf, &len, f_in)!=-1){

			if(buf[0] == '>'){// break;

				size_t l = strlen(c_buffer);
				size_t i;
				for(i=0; i<l; i++) 
					if(N && c_buffer[i]=='N') continue; 
					else printf("%c", c_buffer[i]);
				//printf("%s", c_buffer);

                                if(++k==K) return 0;
				break;
			}

			if(p+len>nalloc){
				nalloc += 2048;
				c_buffer= realloc(c_buffer, sizeof(char) * nalloc);
			}
			strcpy(&c_buffer[p], buf);
			p+=strlen(buf)-1;
		}
		free(buf);
		free(c_buffer);
	}

	size_t l = strlen(c_buffer);
	size_t i;
	for(i=0; i<l; i++) 
		if(N && c_buffer[i]=='N') continue;
		else printf("%c", c_buffer[i]);
	//printf("%s", c_buffer);
	
	fclose(f_in);

return 0;
}

