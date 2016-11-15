#ifndef CONFIG_H
#define CONFIG_H

/**********************************************************************/

#define MB	1048576

#ifndef MEMLIMIT
	#define MEMLIMIT 2048 //2GB of available RAM
#endif

#define RAM_USE MEMLIMIT*1024*1024LLU

#ifndef DEBUG
	#define DEBUG	0
#endif

#ifndef BWT
	#define	BWT	0
#else
	#define BWT_OUTPUT 1
#endif
	
#define	IO_VOLUME	1

//#define BLOCK_ESA_SIZE (1024*MB)/sizeof(t_ESA)	// size of ALL buffers (BLOCK_ESA_SIZE > 0)
#define BLOCK_ESA_SIZE 	(size_t)(0.9*RAM_USE)/sizeof(t_ESA)	// size of ALL buffers (BLOCK_ESA_SIZE > 0)

#define FILE_NAME	512
#define FILE_READ	512		// bytes read from disk once

/**********************************************************************/

#define _OUTPUT_BUFFER	1
//#define OUTPUT_SIZE 	(64*MB)/sizeof(t_GSA) 		// size of output buffer
#define OUTPUT_SIZE 	(size_t)(0.06*RAM_USE)/sizeof(t_GSA) 		// size of output buffer

/**/

#define _PREFIX_ASSY	1
#define _LCP_COMPARISON	1
#define _INDUCING	1

/**/

#define INDUCED_BUFFER	1
//#define INDUCED_SIZE 	(16*MB)/sizeof(t_INDUCED) // 8 bytes per entry 1MB //2048: size of induced buffer (16Mb) 
#define INDUCED_SIZE 	(size_t)(0.02*RAM_USE)/sizeof(t_INDUCED) // 8 bytes per entry 1MB //2048: size of induced buffer (16Mb) 

/**********************************************************************/

#define SIGMA 			255 	// size of \Sigma

#define C_BUFFER_SIZE 		10240	// 10000 size of composed suffix
#define C_OVERFLOW_SIZE 	10240		

#define PREFIX_SIZE 		10
	

/**********************************************************************/

#endif

