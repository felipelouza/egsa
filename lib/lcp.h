#ifndef LCP_H
#define LCP_H

#include <stdlib.h>
#include <stdio.h>
#include "utils.h"


void find_inverse(int_suff* SA, int_suff **ISA, int_suff n);

int lcp_kasai(const unsigned char* T, int_suff* SA, int_suff n, int_lcp* LCP);
int lcp_PHI(const unsigned char* T, int_suff* SA, int_suff n, int_lcp* LCP);

int_lcp lcp_media(int_lcp* LCP, size_t length);

#endif
