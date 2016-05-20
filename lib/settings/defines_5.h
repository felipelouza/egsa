#ifndef CONFIG_H
#define CONFIG_H

/**********************************************************************/

#define MB	1048576

#ifndef DEBUG
	#define DEBUG	0
#endif

#ifndef BWT
	#define	BWT	0
#endif

#define BLOCK_ESA_SIZE 	(1024*MB)/sizeof(t_ESA)	// (10MB) size of buffer (BLOCK_ESA_SIZE > 0)

#define FILE_NAME	512
#define FILE_READ	512		// bytes read from disk once

/**********************************************************************/

#define _OUTPUT_BUFFER	1
#define OUTPUT_SIZE 	(64*MB)/sizeof(t_GSA) 		// size of output buffer

/**/

#define _PREFIX_ASSY	1
#define _LCP_COMPARISON	1
#define _INDUCING	0

/**/

#define INDUCED_BUFFER	1
#define INDUCED_SIZE 	(16*MB)/sizeof(t_INDUCED) // 8 bytes per entry 1MB //2048: size of induced buffer (16Mb) 

/**********************************************************************/

#define SIGMA 			255 	// size of \Sigma

#define C_BUFFER_SIZE 		128	// 10000 size of composed suffix
#define C_OVERFLOW_SIZE 	128		

#define PREFIX_SIZE 		10
	

/**********************************************************************/

#endif

