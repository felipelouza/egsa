// This is (a modification of) SACA-K algorithm, which can be retrieved at:
// http://code.google.com/p/ge-nong/ 
// Our version of SACA-K, called gSACA-K, maintain the theoretical bounds of the
// original algorithm to construct the generalized suffix array.

#ifndef GSACA_K_H
#define GSACA_K_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

//#include "lib/utils.h"

#define m64 0

#if m64
	typedef int64_t  int_t;
	typedef uint64_t uint_t;
	#define PRIdN	 PRId64
#else
	typedef int32_t  int_t;
	typedef uint32_t uint_t;
	#define PRIdN	 PRId32
#endif

typedef uint32_t int_text;



int_t SACA_K(int_t *s, uint_t *SA,
  uint_t n, unsigned int K,
  uint_t m, int cs, int level);

int_t gSACA_K(unsigned char *s, uint_t *SA,
  uint_t n, unsigned int K,
  uint_t m, int cs, int level, unsigned char separator);

#endif
