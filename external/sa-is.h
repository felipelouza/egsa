// This is the sample code for the SA-IS algorithm presented in our article:
// G. Nong, S. Zhang and W. H. Chan, Two Efficient Algorithms for Linear Time Suffix Array Construction, 
// IEEE Transactions on Computers, Vol. 60, No. 10, Oct. 2011. 
// which draft can be retrieved at: http://code.google.com/p/ge-nong/

#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <stdio.h>

const int EMPTY=0xffffffff;
unsigned char mask[]={0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
#define tget(i) ( (t[(i)/8]&mask[(i)%8]) ? 1 : 0 )
#define tset(i, b) t[(i)/8]=(b) ? (mask[(i)%8]|t[(i)/8]) : ((~mask[(i)%8])&t[(i)/8])

#define chr(i) (cs==sizeof(int)?((int*)s)[i]:((unsigned char *)s)[i])
#define isLMS(i) (i>0 && tget(i) && !tget(i-1))

clock_t start, finish;
double  duration;


void timer_start();
void timer_finish(char *str);

// compute the head or end of each bucket
void getBuckets(unsigned int *s, int *bkt, int n, int K, int cs, int end);

// compute SAl
void induceSAl(unsigned char *t, int *SA, unsigned int *s, int *bkt, 
               int n, int K, int cs, int level);

// compute SAs
void induceSAs(unsigned char *t, int *SA, unsigned int *s, int *bkt, 
               int n, int K, int cs);

// find the suffix array SA of s[0..n-1] in {0..K}^n;
// require s[n-1]=0 (the virtual sentinel!), n>=2;
// use a space of at most 6.25n+(1) for a constant alphabet;
// level starts from 0.
void SA_IS(unsigned int *s, int *SA, int n, int K, int cs, int level);

