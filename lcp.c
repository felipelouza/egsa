#include "lcp.h"

void find_inverse(int_suff* SA, int_suff **ISA, int_suff n){
	
	(*ISA) = (int_suff*) malloc(n * sizeof(int_suff));
	
	int_suff i;
	for(i = 0; i < n; i++)
		((*ISA)[SA[i]]) = i;

//return ISA;
}

int lcp_kasai(const unsigned char* T, int_suff* SA, int_suff n, int_lcp* LCP, int_suff *ISA){
	
	
	int_suff l = 0;
	int_suff j, k, i;
	
	for(i = 0; i < n; i++){

		k = ISA[i];
	
		if(!k) {
			LCP[k] = 0;
		}
		else{
			
			j = SA[k-1];
			while(T[i+l] == T[j+l] && !(T[i+l] == 0 && T[j+l]==0)){
				l++; 	
			}
			LCP[k] = l;
			
			/**
			if(T[i+l] == 0 && T[j+l]==0){
				LCP[k]++;
			}
			/**/
			
			if(l>0) l--;
		}
	}	

return 0;
}

/***********************************************************************/

int_lcp lcp_media(int_lcp* LCP, size_t length){
	
	size_t media = 0;
	size_t i;
	
	for(i = 0; i < length; i++)
		media += LCP[i];

return (int_lcp) media/length;
}

/**********************************************************************/
