/*
 * Felipe A. Louza, 29/06/2016
 * Remove header from .fasta file.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**********************************************************************/

int main(int argc, char **argv) {
	
        size_t K;

	//argv[1] = DIR
	//argv[2] = DATABASE
        sscanf(argv[3], "%zu", &K);//NUMBER OF STRINGS
/**/
	char* oldwd = getcwd(NULL,0);
	if (!oldwd) return 1;
	if (chdir(argv[1]) == -1) return 1;
	free(oldwd);
/**/
	
	FILE *f_in=NULL;	
	f_in = fopen(argv[2], "r");		
	if (!f_in) perror ("write_sequence");

	
	char *buf = NULL;
	size_t len = 0, k=0;
	
	getline(&buf, &len, f_in);// first sequence '>'
	free(buf);

	char *c_buffer;
	while(!feof(f_in)){
		
		len = 0;
		int nalloc = 128;
		c_buffer = (char*) malloc(nalloc*sizeof(char));

		size_t p=0;
		while(getline(&buf, &len, f_in)!=-1){

			if(buf[0] == '>'){// break;
				printf("%s", c_buffer);
                                if(++k==K) return 0;
				break;
			}

			if(p+len>nalloc){
				nalloc += 128;
				c_buffer= realloc(c_buffer, sizeof(char) * nalloc);
			}
			strcpy(&c_buffer[p], buf);
			p+=strlen(buf)-1;

//			printf("%s", buf);
		}
		free(buf);
		free(c_buffer);
	}

	printf("%s", c_buffer);
	
	fclose(f_in);

return 0;
}

