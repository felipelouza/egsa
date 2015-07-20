#ifndef CONFIG_H
#define CONFIG_H

/**********************************************************************/

#ifndef DEBUG
	#define DEBUG	0
#endif


#ifndef BWT
	#define	BWT	0
#endif

#define BLOCK_ESA_SIZE 	2048 		// (10MB) size of buffer (BLOCK_ESA_SIZE > 0)

#define FILE_NAME	500
#define FILE_READ	256		// bytes read from disk once

/**********************************************************************/

#define _OUTPUT_BUFFER	1
#define OUTPUT_SIZE 	2048 		// size of output buffer

/**/

#define _PREFIX_ASSY	1
#define _LCP_COMPARISON	1
#define _INDUCING	1

/**/

#define INDUCED_BUFFER	1
#define INDUCED_SIZE 	2048 		// size of induced buffer (16Mb) 

/**********************************************************************/

#if MODE == 1
	#define DNA			1
#endif

#if MODE == 2
	#define READ			0
	#define PROTEIN			1
#endif

#if DNA

	#define SIGMA 			4 	// size of \Sigma

	#define C_BUFFER_SIZE 		10000	// size of composed suffix
	#define C_OVERFLOW_SIZE 	10000	
	
	#define PREFIX_SIZE 		1024
	
	#define INPUT_CAT		0		
	
#elif READ

	#define SIGMA 			4 	// size of \Sigma

	#define C_BUFFER_SIZE 		128	// size of composed suffix
	#define C_OVERFLOW_SIZE 	128		
	
	#define PREFIX_SIZE 		10
	
	#define INPUT_CAT		1	//concatenated input  (1 file with K sequences)

#elif PROTEIN

	#define SIGMA 			21 	// size of \Sigma

	#define C_BUFFER_SIZE 		128	// size of composed suffix
	#define C_OVERFLOW_SIZE 	128		
	
	#define PREFIX_SIZE 		10
	
	#define INPUT_CAT		1	//concatenated input  (1 file with K sequences)

#endif


/**********************************************************************/

#endif

