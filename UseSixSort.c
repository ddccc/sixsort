/*
File: c:/bsd/rigel/sort/Licence.txt
Date: Sat Jun 09 22:22:31 2012

Copyright (c) 2012, Dennis de Champeaux.  All rights reserved.

The copyright holders hereby grant to any person obtaining a copy of
this software (the "Software") and/or its associated documentation
files (the Documentation), the irrevocable (except in the case of
breach of this license) no-cost, royalty free, rights to use the
Software for non-commercial evaluation/test purposes only, while
subject to the following conditions:

. Redistributions of the Software in source code must retain the above
copyright notice, this list of conditions and the following
disclaimers.

. Redistributions of the Software in binary form must reproduce the
above copyright notice, this list of conditions and the following
disclaimers in the documentation and/or other materials provided with
the distribution.

. Redistributions of the Documentation must retain the above copyright
notice, this list of conditions and the following disclaimers.

The name of the copyright holder, may not be used to endorse or
promote products derived from this Software or the Documentation
without specific prior written permission of the copyright holder.
 
THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS", WITHOUT WARRANTY
OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION OR THE USE OF OR
OTHER DEALINGS WITH THE SOFTWARE OR DOCUMENTATION.
*/


// File: c:/bsd/rigel/sort/UseSixSort.c
// Date: Thu Jan 07 15:38:08 2010

/*
   This file is a test bench for excercising sixsort and testing 
   it against other algorithms.
   >>Specific objects and a specific comparison function are used throughout<<.
   Feel free to change them as you see fit.
  
   The comparison function given here does NOT work for qsort, because qsort 
   thinks that entities to be sorted reside in the array, while sixsort knows
   that the array contains pointers to objects.  It >is< possible, of course,
   to construct an alternative comparison function for qsort.

   To compile:  $ gcc UseSixSort.c SixSort.o
   It produces the file: a.exe (or the like)


   The main function has many alternative functions.
   Un-comment one to activate.
   Inside such a function are often other choices that can be selected.
 */



#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

// #include "C2fsort.c"
// #include "C4.c"

// To avoid compiler warnings:::
void callQuicksort0(void **AA, int size, int (*compar )());
void callCut2(void **AA, int size, int (*compar )());
void callQsort(void **A, int size, int (*compar )());
void callHeapsort(void **A, int size, int (*compar )());
void callBentley();
void callLQ();
void callChensort();
void compareBentleyAgainstSixSort();
void compareLQAgainstSixSort();
void compareChenSortAgainstSixSort();
void compareCut2AgainstSixsort();
void compareQsortAgainstQuicksort0();
void compareQsortAgainstCut2();
void compareQuicksort0AgainstSixsort();
void compareSixsortAgainstXYZ();
void quicksort0(void **A, int N, int M, int (*compar )());
void cut2(void **A, int N, int M, int (*compar )());
void cut2f(void **A, int N, int M, int (*compar )());
void cut4(void **A, int N, int M, int (*compar )());
void sixsort(void **A, int size, 
	      int (*compar ) (const void *, const void * ));
void heapSort(void **a, int count, int (*compar )());
void testBentley();
void testQsort();
void testQuicksort0();
void testSixSort();
void timeTest();
void validateXYZ();
void validateHeapSort();
void validateSixSort();
void validateBentley();
void validateQsort();
int clock();
void validateSixSortBT();
void compareBentleyAgainstSixsort(); // on Bentley test bench


// Here global entities used throughout
// int (*compareXY)();
// void **A;

// Example of objects that can be used to populate an array to be sorted:
  // To obtain the int field from X: ((struct intval *) X)->val
  // To obtain the float field from X: ((struct intval *) X)->valf
  struct intval {
    int val;
    float valf;
  };

// Here an exmple comparison function for these objects:
// **** NOTE, again **** 
//      All the examples below use ::::
//          the intval objects, and 
//          the compareIntVal comparison function
int compareIntVal (const void *a, const void *b)
{
  struct intval *pa = (struct intval *) a;
  struct intval *pb = (struct intval *) b;
  return (pa->val - pb->val);
}

// This comparison formula is used for qsort in callQsort and
// for bentley in callBentley 
int compareIntVal2 (const void **a, const void **b)
{
  struct intval *pa = (struct intval *) *a;
  struct intval *pb = (struct intval *) *b;
  return (pa->val - pb->val);
}

int main (int argc, char *argv[]) {
  printf("Running UseSixSort ...\n");
  // Un-comment one of the following to use a specific functionality.
  // Modify the body of these functions as desired

  // To ask for the license expiration date and the host
     // sixsort(0, 0, 0);
  // To check that a sorted array is produced
     // testQuicksort0();
     // testSixSort(); 
     // testQsort();
     // testBentley();
  // validateHeapSort();
  // validateSixSort();
  // validateQsort();
  // validateBentley();
  // Measure the sorting time of an algorithm
  // timeTest();
  // Compare the outputs of two sorting algorithms
     // validateXYZ(); // must provide an other algorithm XYZ
     // ... and uncomment validateXYZ ...
  // Compare the speed fraction of two algorithms
     // compareSixsortAgainstXYZ();
     // ... and uncomment also compareSixsortAgainstXYZ ...
  // Whatever here:::
     // compareQuicksort0AgainstSixsort();
     // compareCut2AgainstSixsort();
     // compareQsortAgainstQuicksort0(); 
     // compareQsortAgainstCut2(); 
     compareBentleyAgainstSixSort();
     // compareLQAgainstSixSort();
     // compareChenSortAgainstSixSort();
     // validateSixSortBT();
     // compareBentleyAgainstSixsort(); // on Bentley test bench
     return 0;
} // end of main

void *myMalloc(char* location, int size) {
  void *p = malloc(size);
  if ( 0 == p ) {
    fprintf(stderr, "malloc fails for: %s\n", location);
    exit(1);
  }
  return p;
} // end myMalloc

// fillarray assigns random values to the int-field of our objects
void fillarray(void **A, int lng, int startv) {
  const int range = 1024*1024*32;
  int i;
  srand(startv);
  struct intval *pi;
  for ( i = 0; i < lng; i++) {
    pi = (struct intval *)A[i];
    pi->val = rand()%range; 
  }
} // end of fillarray

// Check that adjacent objects in an array are ordered.
// If not, it reports an error 
void check(void **A, int N, int M) {
  int i; int cnt = 0;
  void *x, *y;
  struct intval *pi;
  for (i = N + 1; i <= M; i++) {
    x = A[i-1]; y = A[i];
    if ( compareIntVal(y, x) < 0 ) {
      pi = (struct intval *) y;
      printf("%s %d %s %d %s", "Error at: ", i, 
	     " A[i]: ", pi->val, "\n");
      cnt++;
    }
  }
  printf("check # errors: %d\n", cnt);
} // end check


// testAlgorithm0 is a generalization of testSixsort;
// another algorithm can be plugged in
void testAlgorithm00(char* label, int siz, void (*alg1)(),
		     int (*compare1)()) {
  printf("%s on size: %d\n", label, siz);
  // create array
  struct intval *pi;
  void **A = myMalloc("testAlgorithm0 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("testAlgorithm0 2", sizeof (struct intval));
    A[i] = pi;
  };
  // fill its content
  fillarray(A, siz, 100);
  // sort it
  (*alg1)(A, siz, compare1);
  // and check it
  check(A, 0, siz-1);
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]);
    };
    free(A);
} // end testAlgorithm0

void testAlgorithm0(char* label, int siz, void (*alg1)() ) {
  testAlgorithm00(label, siz, alg1, compareIntVal);
} // end testAlgorithm0

void testAlgorithm2(char* label, int siz, void (*alg1)() ) {
  testAlgorithm00(label, siz, alg1, compareIntVal2);
} // end testAlgorithm2

// like testAlgorithm0 but the size of array is preset inside testAlgorithm
void testAlgorithm(char* label, void (*alg1)() ) {
  testAlgorithm0(label, 1024*1024, alg1);
  // testAlgorithm0(label, 1024, alg1);
} // end testAlgorithm0

// /* Example: use of testAlgorithm
void testSixSort() {
  void sixsort();
  testAlgorithm("Running sixsort ...", sixsort);
}
//  */
void testQuicksort0() {
  void callQuicksort0();
  testAlgorithm0("Check quicksort0()", 1024*1024, callQuicksort0);
} // end testQuicksort0()

void testQsort() {
  void callQsort();
  testAlgorithm2("Check callQsort()", 1024*1024, callQsort);
}

void testBentley() {
  void callBentley();
  testAlgorithm2("Check callBentley()", 1024*1024, callBentley);
}

// validateAlgorithm0 is used to check algorithm alg2 against a
// trusted algorithm alg1.
// The check consists of making sure that starting from identical
// inputs they produce identical outputs
void validateAlgorithm0(char* label, int siz, void (*alg1)(), void (*alg2)() ) {
  printf("%s on size: %d\n", label, siz);
  // create the input for alg1 ...
  struct intval *pi;
  void **A = myMalloc("validateAlgorithm0 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("validateAlgorithm0 2", sizeof (struct intval));
    A[i] = pi;
  };
  fillarray(A, siz, 100);
  // ... sort it
  (*alg1)(A, siz, compareIntVal);

  // create the input for alg2 ...
  void **B = myMalloc("validateAlgorithm0 3", sizeof(pi) * siz);
  // struct intval *pi;
  for (i = 0; i < siz; i++) {
    pi =  myMalloc("validateAlgorithm0 4", sizeof (struct intval));
    B[i] = pi;
  };
  fillarray(B, siz, 100);
  // ... sort it
  (*alg2)(B, siz, compareIntVal);
 
  // check that the two outputs are the same
  int foundError = 0;
  for (i = 0; i < siz; i++)
    // if ( A[i] != B[i] ) {
    if ( compareIntVal(A[i], B[i]) != 0 ) {
      printf("validate error i: %d\n", i);
      foundError = 1;
    }
  if ( !foundError ) 
    printf("NO error found ...\n");
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]); 
    };
    free(A);
    for (i = 0; i < siz; i++) {
      free(B[i]); 
    };
    free(B);
} // end validateAlgorithm0

// Like validateAlgorithm0 but with fixed array size
void validateAlgorithm(char* label, void (*alg1)(), void (*alg2)() ) {
  validateAlgorithm0(label, 1024 * 1024 * 16, alg1, alg2);
} // end validateAlgorithm

/* Example:: replace XYZ by what you want to validate
void validateXYZ() {
  void sixsort(), XYZ();
  validateAlgorithm("Running validate XYZ ...",
		    sixsort, XYZ);
}
 */

void callHeapsort(void **A, int size, 
	 int (*compar ) (const void *, const void * ) ) {
  heapSort(A, size, compar);
} // end callHeapsort

void validateHeapSort() {
  void callQuicksort0(), callHeapsort();
  validateAlgorithm("Running validate HeapSort ...",
		    callQuicksort0, callHeapsort);
} // end validateHeapSort
void validateSixSort() {
  void callQuicksort0(), sixsort();
  validateAlgorithm("Running validate SixSort ...",
		    callQuicksort0, sixsort);
} // end validateSixSort

// alg1 and alg2 use different comparison functions
void validateAlgorithm1(char* label, int siz, void (*alg1)(), void (*alg2)() ) {
  printf("%s on size: %d\n", label, siz);
  // create the input for alg1 ...
  struct intval *pi;
  void **A = myMalloc("validateAlgorithm0 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("validateAlgorithm0 2", sizeof (struct intval));
    A[i] = pi;
  };
  fillarray(A, siz, 100);
  // ... sort it
  (*alg1)(A, siz, compareIntVal);

  // create the input for alg2 ...
  void **B = myMalloc("validateAlgorithm0 3", sizeof(pi) * siz);
  // struct intval *pi;
  for (i = 0; i < siz; i++) {
    pi =  myMalloc("validateAlgorithm0 4", sizeof (struct intval));
    B[i] = pi;
  };
  fillarray(B, siz, 100);
  // ... sort it
  (*alg2)(B, siz, compareIntVal2);
 
  // check that the two outputs are the same
  int foundError = 0;
  for (i = 0; i < siz; i++)
    // if ( A[i] != B[i] ) {
    if ( compareIntVal(A[i], B[i]) != 0 ) {
      printf("validate error i: %d\n", i);
      foundError = 1;
    }
  if ( !foundError ) 
    printf("NO error found ...\n");
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]); 
    };
    free(A);
    for (i = 0; i < siz; i++) {
      free(B[i]); 
    };
    free(B);
} // end validateAlgorithm1

void validateQsort() {
  void callQuicksort0(), callQsort();
  validateAlgorithm1("Running validate qsort ...", 1024 * 1024,
		    callQuicksort0, callQsort);
} // end validateQsort

void validateBentley() {
  void callQuicksort0(), callBentley();
  validateAlgorithm1("Running validate bentley ...",  1024 * 1024,
		    callQuicksort0, callBentley);
} // end validateBentley

// Run an algorithm and report the time used
void timeTest() {
  printf("timeTest() of sixsort \n");
  int algTime, T;
  int seed;
  // int seedLimit = 10;
  int seedLimit = 3;
  int z;
  int siz = 1024 * 1024 * 16;
  // construct array
  struct intval *pi;
  void **A = myMalloc("timeTest 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("timeTest 2", sizeof (struct intval));
    A[i] = pi;
  };

  // warm up the process
  fillarray(A, siz, 666); 
  int sumTimes = 0;
  int reps = 3;
  for (z = 0; z < reps; z++) { // repeat to check stability
    algTime = 0;
    // measure the array fill time
    int TFill = clock();
    for (seed = 0; seed < seedLimit; seed++) 
      fillarray(A, siz, seed);
      // here alternative ways to fill the array
      // int k;
      // for ( k = 0; k < siz; k++ ) A[k] = 0;
      // for ( k = 0; k < siz; k++ ) A[k] = k%5;
      // for ( k = 0; k < siz; k++ ) A[k] = siz-k;
    TFill = clock() - TFill;
    // now we know how much time it takes to fill the array
    // measure the time to fill & sort the array
    T = clock();
    for (seed = 0; seed < seedLimit; seed++) { 
      fillarray(A, siz, seed);
      // for ( k = 0; k < siz; k++ ) A[k] = 0;
      // for ( k = 0; k < siz; k++ ) A[k] = k%5;
      // for ( k = 0; k < siz; k++ ) A[k] = siz-k;

      // callCut2(A, siz, compareIntVal);  
      sixsort(A, siz, compareIntVal);  
    }
    // ... and subtract the fill time to obtain the sort time
    algTime = (clock() - T - TFill)/ seedLimit;
    printf("algTime: %d \n", algTime);
    sumTimes = sumTimes + algTime;
  }
  printf("%s %d %s", "sumTimes: ", (sumTimes/reps), "\n");
  // free array
  for (i = 0; i < siz; i++) {
    free(A[i]); 
  };
  free(A);

} // end timeTest

// callCut2 is not used by sixsort; it is employed by UseSixSort.c
// for cut2 testing. 
void callCut2(void **A, int size, 
	int (*compar ) (const void *, const void * ) ) {
  cut2f(A, 0, size-1, compar);
} // end callCut2

// Report the speed fraction of two algorithms on a range of array sizes
void compareAlgorithms00(char *label, int siz, int seedLimit, 
		   void (*alg1)(), void (*alg2)(),
		   int (*compare1)(), int (*compare2)() ) {
  printf("%s on size: %d seedLimit: %d\n", label, siz, seedLimit);
  int alg1Time, alg2Time, T;
  int seed;
  int z;
  int limit = 1024 * 1024 * 16 + 1;
  while (siz <= limit) {
    printf("%s %d %s %d %s", "siz: ", siz, " seedLimit: ", seedLimit, "\n");
    struct intval *pi;
    void **A = myMalloc("compareAlgorithms0 1", sizeof(pi) * siz);
    // construct array
    int i;
    for (i = 0; i < siz; i++) {
      pi = myMalloc("compareAlgorithms0 2", sizeof (struct intval));
      A[i] = pi;
    };
    // warm up the process
    for (seed = 0; seed < seedLimit; seed++) 
      fillarray(A, siz, seed);
    // for (z = 0; z < 6; z++) { // repeat to check stability
    int repeats = 10; // even 
    float fracs[repeats-1];
    for (z = 0; z < repeats; z++) { // repeat to check stability
      alg1Time = 0; alg2Time = 0;
      int TFill = clock();
      for (seed = 0; seed < seedLimit; seed++) 
	fillarray(A, siz, seed);
      TFill = clock() - TFill;
      T = clock();
      for (seed = 0; seed < seedLimit; seed++) { 
	fillarray(A, siz, seed);
	(*alg1)(A, siz, compare1); 
      }
      alg1Time = clock() - T - TFill;
      T = clock();
      for (seed = 0; seed < seedLimit; seed++) { 
	fillarray(A, siz, seed);
	(*alg2)(A, siz, compare2);
      }
      alg2Time = clock() - T - TFill;
      /*
      printf("%s %d %s", "siz: ", siz, " ");
      printf("%s %d %s", "alg1Time: ", alg1Time, " ");
      printf("%s %d %s", "alg2Time: ", alg2Time, " ");
      */
      float frac = 0;
      if ( alg1Time != 0 ) frac = alg2Time / ( 1.0 * alg1Time );
      // printf("%s %f %s", "frac: ", frac, "\n");
      if (0==z) continue;
      int y = z-1;
      fracs[y] = frac;
      while (0 < y) {
	if ( fracs[y-1] <= fracs[y] ) break;
	frac = fracs[y-1]; fracs[y-1] = fracs[y]; fracs[y] = frac;
	y--;
      }
    }
    printf("siz:  %i median of fracs: %f\n", siz, fracs[repeats/2]);
    // free array
    for (i = 0; i < siz; i++) {
      free(A[i]);
    };
    free(A);
    siz = siz * 2;
    seedLimit = seedLimit / 2;
    // siz = siz * 4;
    // seedLimit = seedLimit / 4;
  }
} // end compareAlgorithms00

void compareAlgorithms0(char *label, int siz, int seedLimit, 
		   void (*alg1)(), void (*alg2)() ) {
  compareAlgorithms00(label, siz, seedLimit, alg1, alg2, 
		      compareIntVal, compareIntVal);
 } // end compareAlgorithms0

void compareAlgorithms2(char *label, int siz, int seedLimit, 
		   void (*alg1)(), void (*alg2)() ) {
  compareAlgorithms00(label, siz, seedLimit, alg1, alg2, 
		      compareIntVal2, compareIntVal);
 } // end compareAlgorithms2

void compareAlgorithms(char *label, void (*alg1)(), void (*alg2)() ) {
  compareAlgorithms0(label, 1024, 32 * 1024, alg1, alg2);
  // compareAlgorithms0(label, 16 * 1024 * 1024, 4, alg1, alg2);
  // compareAlgorithms0(label, 1024 * 1024, 32, alg1, alg2);
} // end compareAlgorithms


/* Example, replace XYZ by what you want to compare against
void compareSixsortAgainstXYZ() {
  int sixsort(), XYZ();
  compareAlgorithms("Compare sixsort vs XYZ", sixsort, XYZ);
}
*/
void compareQuicksort0AgainstSixsort() {
  void sixsort(), callQuicksort0();
  compareAlgorithms("Compare quicksort0 vs sixsort", callQuicksort0, sixsort);
} // end compareQuicksort0AgainstSixsort

void compareCut2AgainstSixsort() {
  void sixsort(), callCut2();
  compareAlgorithms("Compare cut2 vs sixsort", callCut2, sixsort);
} // end compareCut2AgainstSixsort 

void callQuicksort0(void **A, int size, 
	int (*compar ) (const void *, const void * ) ) {
  quicksort0(A, 0, size-1, compar);
} // end callQuicksort0

void compareQsortAgainstQuicksort0() {
   void callQsort(), callQuicksort0();
   compareAlgorithms2("Compare qsort vs quicksort0", 1024 * 1024, 32,
		      callQsort, callQuicksort0);
 } // end compareQsortAgainstQuicksort0

void compareQsortAgainstCut2() {
   void callQsort(), callCut2();
   compareAlgorithms2("Compare qsort vs cut2", 1024 * 1024, 32,
		      callQsort, callCut2);
 } // end compareQsortAgainstCut2

// Wrapper function to invoke qsort
void callQsort(void **A, int size,
      int (*compar ) (const void *, const void * ) ) {
  struct intval *pi;
  qsort(A, size, sizeof(pi), 
	(int(*)(const void *, const void *)) compar);
} // end callQsort

void compareBentleyAgainstSixSort(){
   void callBentley(), sixsort();
   compareAlgorithms2("Compare bentley vs SixSort", 1024, 32 * 1024,
   // compareAlgorithms2("Compare bentley vs SixSort", 1024 * 1024, 32,
		      callBentley, sixsort);
} // end compareBentleyAgainstSixSort

void compareLQAgainstSixSort(){
   void callLQ(), sixsort();
   compareAlgorithms2("Compare LQ vs SixSort", 1024, 32 * 1024,
   // compareAlgorithms2("Compare LQ vs SixSort", 1024 * 1024, 32,
		      callLQ, sixsort);
} // end compareLQAgainstSixSort

void compareChenSortAgainstSixSort(){
   void callChensort(), sixsort();
   compareAlgorithms2("Compare ChenSort vs SixSort", 1024, 32 * 1024,
   // compareAlgorithms2("Compare ChenSort vs SixSort", 1024 * 1024, 32,
		      callChensort, sixsort);
} // end compareChenSortAgainstSixSort

void bentley();
// Wrapper function to invoke bentley
void callBentley(void **A, int size,
      int (*compar ) (const void *, const void * ) ) {
  struct intval *pi;
  bentley(A, size, sizeof(pi), 
	(int(*)(const void *, const void *)) compar);
} // end callBentley

// #include <_ansi.h>
#include <stdlib.h>

#ifndef __GNUC__
#define inline
#endif

static inline char	*med33 _PARAMS((char *, char *, char *, int (*)()));
static inline void	 swapfunc _PARAMS((char *, char *, int, int));

#define min(a, b)	(a) < (b) ? a : b

/*
 * Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".
 */
#define swapcode(TYPE, parmi, parmj, n) { 		\
	long i = (n) / sizeof (TYPE); 			\
	register TYPE *pi = (TYPE *) (parmi); 		\
	register TYPE *pj = (TYPE *) (parmj); 		\
	do { 						\
		register TYPE	t = *pi;		\
		*pi++ = *pj;				\
		*pj++ = t;				\
        } while (--i > 0);				\
}

#define SWAPINIT(a, es) swaptype = ((char *)a - (char *)0) % sizeof(long) || \
	es % sizeof(long) ? 2 : es == sizeof(long)? 0 : 1;

static inline void swapfunc(a, b, n, swaptype)
     char *a;
     char *b;
     int n;
     int swaptype;
{
	if (swaptype <= 1) 
		swapcode(long, a, b, n)
	else
		swapcode(char, a, b, n)
}

#define swap(a, b)					\
	if (swaptype == 0) {				\
		long t = *(long *)(a);			\
		*(long *)(a) = *(long *)(b);		\
		*(long *)(b) = t;			\
	} else						\
		swapfunc(a, b, es, swaptype)

#define vecswap(a, b, n) 	if ((n) > 0) swapfunc(a, b, n, swaptype)

static inline char * med33(a, b, c, cmp)
     char *a;
     char *b;
     char *c;
     int (*cmp)(); 
{
	return cmp(a, b) < 0 ?
	       (cmp(b, c) < 0 ? b : (cmp(a, c) < 0 ? c : a ))
              :(cmp(b, c) > 0 ? b : (cmp(a, c) < 0 ? a : c ));
}

void bentley(a, n, es, cmp)
     void *a;
     size_t n;
     size_t es;
     int (*cmp)();
{
  char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
  int d, r, swaptype; 
  // The use of swap_cnt and the 2nd invocation of insertionsort has been removed
  // int swap_cnt; 

loop:	SWAPINIT(a, es);
  // swap_cnt = 0;
	if (n < 7) {
		for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
			for (pl = pm; pl > (char *) a && cmp(pl - es, pl) > 0;
			     pl -= es)
				swap(pl, pl - es);
		return;
	}
	pm = (char *) a + (n / 2) * es;
	if (n > 7) {
		pl = a;
		pn = (char *) a + (n - 1) * es;
		if (n > 40) {
			d = (n / 8) * es;
			pl = med33(pl, pl + d, pl + 2 * d, cmp);
			pm = med33(pm - d, pm, pm + d, cmp);
			pn = med33(pn - 2 * d, pn - d, pn, cmp);
		}
		pm = med33(pl, pm, pn, cmp);
	}
	swap(a, pm);
	pa = pb = (char *) a + es;

	pc = pd = (char *) a + (n - 1) * es;
	for (;;) {
		while (pb <= pc && (r = cmp(pb, a)) <= 0) {
			if (r == 0) {
			        // swap_cnt = 1;
				swap(pa, pb);
				pa += es;
			}
			pb += es;
		}
		while (pb <= pc && (r = cmp(pc, a)) >= 0) {
			if (r == 0) {
			        // swap_cnt = 1;
				swap(pc, pd);
				pd -= es;
			}
			pc -= es;
		}
		if (pb > pc)
			break;
		swap(pb, pc);
		// swap_cnt = 1;
		pb += es;
		pc -= es;
	}
	/*
	if (swap_cnt == 0) {  // Switch to insertion sort 
		for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
			for (pl = pm; pl > (char *) a && cmp(pl - es, pl) > 0; 
			     pl -= es)
				swap(pl, pl - es);
		return;
	}
	*/

	pn = (char *) a + n * es;
	r = min(pa - (char *)a, pb - pa);
	vecswap(a, pb - r, r);
	r = min(pd - pc, pn - pd - es);
	vecswap(pb, pn - r, r);
	/* Ordering on the recursive calls has been added to obtain at most 
	   log2(N) stack space 
	if ((r = pb - pa) > es)
		bentley(a, r / es, es, cmp);
	if ((r = pd - pc) > es) { 
	        // Iterate rather than recurse to save stack space 
		a = pn - r;
		n = r / es;
		goto loop;
	}
	*/
	
    int left =  pb - pa;
    int right = pd - pc;
    if ( left <= right ) {
       if ( left > es ) bentley(a, left / es, es, cmp);
       if ( right > es ) {
	   // Iterate rather than recurse to save stack space 
	   a = pn - right;
	   n = right / es;
	   goto loop;
       }
    } else {
       if ( right > es ) bentley(pn-right, right / es, es, cmp);
       if ( left > es ) {
	   // Iterate rather than recurse to save stack space 
	   // a = pn - left;
	   n = left / es;
	   goto loop;
       }
    }
} // end of bentley

/* Copyright (C) 1991,1992,1996,1997,1999,2004 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Written by Douglas C. Schmidt (schmidt@ics.uci.edu).

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* If you consider tuning this algorithm, you should consult first:
   Engineering a sort function; Jon Bentley and M. Douglas McIlroy;
   Software - Practice and Experience; Vol. 23 (11), 1249-1265, 1993.  */

#include <alloca.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Byte-wise swap two items of size SIZE. */
#define SWAP(a, b, size)						      \
  do									      \
    {									      \
      register size_t __size = (size);					      \
      register char *__a = (a), *__b = (b);				      \
      do								      \
	{								      \
	  char __tmp = *__a;						      \
	  *__a++ = *__b;						      \
	  *__b++ = __tmp;						      \
	} while (--__size > 0);						      \
    } while (0)

/* Discontinue quicksort algorithm when partition gets below this size.
   This particular magic number was chosen to work best on a Sun 4/260. */
#define MAX_THRESH 4

/* Stack node declarations used to store unfulfilled partition obligations. */
typedef struct
  {
    char *lo;
    char *hi;
  } stack_node;

/* The next 4 #defines implement a very fast in-line stack abstraction. */
/* The stack needs log (total_elements) entries (we could even subtract
   log(MAX_THRESH)).  Since total_elements has type size_t, we get as
   upper bound for log (total_elements):
   bits per byte (CHAR_BIT) * sizeof(size_t).  */
#define STACK_SIZE	(CHAR_BIT * sizeof(size_t))
#define PUSH(low, high)	((void) ((top->lo = (low)), (top->hi = (high)), ++top))
#define	POP(low, high)	((void) (--top, (low = top->lo), (high = top->hi)))
#define	STACK_NOT_EMPTY	(stack < top)


/* Order size using quicksort.  This implementation incorporates
   four optimizations discussed in Sedgewick:

   1. Non-recursive, using an explicit stack of pointer that store the
      next array partition to sort.  To save time, this maximum amount
      of space required to store an array of SIZE_MAX is allocated on the
      stack.  Assuming a 32-bit (64 bit) integer for size_t, this needs
      only 32 * sizeof(stack_node) == 256 bytes (for 64 bit: 1024 bytes).
      Pretty cheap, actually.

   2. Chose the pivot element using a median-of-three decision tree.
      This reduces the probability of selecting a bad pivot value and
      eliminates certain extraneous comparisons.

   3. Only quicksorts TOTAL_ELEMS / MAX_THRESH partitions, leaving
      insertion sort to order the MAX_THRESH items within each partition.
      This is a big win, since insertion sort is faster for small, mostly
      sorted array segments.

   4. The larger of the two sub-partitions is always pushed onto the
      stack first, with the algorithm then concentrating on the
      smaller partition.  This *guarantees* no more than log (total_elems)
      stack size is needed (actually O(1) in this case)!  */

// This is qsort in the Linux C-library
void
_quicksort (void *const pbase, size_t total_elems, size_t size,
	    //	    __compar_d_fn_t cmp, void *arg)
	    int (*cmp)(), void *arg)
{
  register char *base_ptr = (char *) pbase;

  const size_t max_thresh = MAX_THRESH * size;

  if (total_elems == 0)
    /* Avoid lossage with unsigned arithmetic below.  */
    return;

  if (total_elems > MAX_THRESH)
    {
      char *lo = base_ptr;
      char *hi = &lo[size * (total_elems - 1)];
      stack_node stack[STACK_SIZE];
      stack_node *top = stack;

      PUSH (NULL, NULL);

      while (STACK_NOT_EMPTY)
        {
          char *left_ptr;
          char *right_ptr;

	  /* Select median value from among LO, MID, and HI. Rearrange
	     LO and HI so the three values are sorted. This lowers the
	     probability of picking a pathological pivot value and
	     skips a comparison for both the LEFT_PTR and RIGHT_PTR in
	     the while loops. */

	  char *mid = lo + size * ((hi - lo) / size >> 1);

	  if ((*cmp) ((void *) mid, (void *) lo, arg) < 0)
	    SWAP (mid, lo, size);
	  if ((*cmp) ((void *) hi, (void *) mid, arg) < 0)
	    SWAP (mid, hi, size);
	  else
	    goto jump_over;
	  if ((*cmp) ((void *) mid, (void *) lo, arg) < 0)
	    SWAP (mid, lo, size);
	jump_over:;

	  left_ptr  = lo + size;
	  right_ptr = hi - size;

	  /* Here's the famous ``collapse the walls'' section of quicksort.
	     Gotta like those tight inner loops!  They are the main reason
	     that this algorithm runs much faster than others. */
	  do
	    {
	      while ((*cmp) ((void *) left_ptr, (void *) mid, arg) < 0)
		left_ptr += size;

	      while ((*cmp) ((void *) mid, (void *) right_ptr, arg) < 0)
		right_ptr -= size;

	      if (left_ptr < right_ptr)
		{
		  SWAP (left_ptr, right_ptr, size);
		  if (mid == left_ptr)
		    mid = right_ptr;
		  else if (mid == right_ptr)
		    mid = left_ptr;
		  left_ptr += size;
		  right_ptr -= size;
		}
	      else if (left_ptr == right_ptr)
		{
		  left_ptr += size;
		  right_ptr -= size;
		  break;
		}
	    }
	  while (left_ptr <= right_ptr);

          /* Set up pointers for next iteration.  First determine whether
             left and right partitions are below the threshold size.  If so,
             ignore one or both.  Otherwise, push the larger partition's
             bounds on the stack and continue sorting the smaller one. */

          if ((size_t) (right_ptr - lo) <= max_thresh)
            {
              if ((size_t) (hi - left_ptr) <= max_thresh)
		/* Ignore both small partitions. */
                POP (lo, hi);
              else
		/* Ignore small left partition. */
                lo = left_ptr;
            }
          else if ((size_t) (hi - left_ptr) <= max_thresh)
	    /* Ignore small right partition. */
            hi = right_ptr;
          else if ((right_ptr - lo) > (hi - left_ptr))
            {
	      /* Push larger left partition indices. */
              PUSH (lo, right_ptr);
              lo = left_ptr;
            }
          else
            {
	      /* Push larger right partition indices. */
              PUSH (left_ptr, hi);
              hi = right_ptr;
            }
        }
    }

  /* Once the BASE_PTR array is partially sorted by quicksort the rest
     is completely sorted using insertion sort, since this is efficient
     for partitions below MAX_THRESH size. BASE_PTR points to the beginning
     of the array to sort, and END_PTR points at the very last element in
     the array (*not* one beyond it!). */

  // #define min(x, y) ((x) < (y) ? (x) : (y))

  {
    char *const end_ptr = &base_ptr[size * (total_elems - 1)];
    char *tmp_ptr = base_ptr;
    char *thresh = min(end_ptr, base_ptr + max_thresh);
    register char *run_ptr;

    /* Find smallest element in first threshold and place it at the
       array's beginning.  This is the smallest array element,
       and the operation speeds up insertion sort's inner loop. */

    for (run_ptr = tmp_ptr + size; run_ptr <= thresh; run_ptr += size)
      if ((*cmp) ((void *) run_ptr, (void *) tmp_ptr, arg) < 0)
        tmp_ptr = run_ptr;

    if (tmp_ptr != base_ptr)
      SWAP (tmp_ptr, base_ptr, size);

    /* Insertion sort, running from left-hand-side up to right-hand-side.  */

    run_ptr = base_ptr + size;
    while ((run_ptr += size) <= end_ptr)
      {
	tmp_ptr = run_ptr - size;
	while ((*cmp) ((void *) run_ptr, (void *) tmp_ptr, arg) < 0)
	  tmp_ptr -= size;

	tmp_ptr += size;
        if (tmp_ptr != run_ptr)
          {
            char *trav;

	    trav = run_ptr + size;
	    while (--trav >= run_ptr)
              {
                char c = *trav;
                char *hi, *lo;

                for (hi = lo = trav; (lo -= size) >= tmp_ptr; hi = lo)
                  *hi = *lo;
                *hi = c;
              }
          }
      }
  }
} // end _quicksort 

// LQ is qsort in the Linux C-library
void callLQ(void **A, int size, int (*compar ) () ) { 
  struct intval *pi;
  _quicksort(A, size, sizeof(pi), compar, NULL);
} // end callLQ

/*   This is the C code for Adaptive Symmetry Partition Sort       */     
/*   which sorts the specified sequence into ascending order.      */
/*   This algorithm is Rem-adaptive and offers better performance  */ 
/*   than Psort                                                    */
/*   Date : 2007/01/24, version 1.0                                */
/*   Copyright (C) 2007 Jingchao Chen                              */
/*   Any comment is welcome. For any question, email to            */
/*   chen-jc@dhu.edu.cn or chenjingchao@yahoo.com                  */
// From: http://www.gtoal.com/languages/bcpl/BCPL/bcplprogs/sort/AdoSymmetryPSort.cpp

// #include <conio.h>
#include <stdlib.h>
#include <stdio.h>
// #include <dos.h>
#define SIZE_n 45
#define ItemType int

int  n=SIZE_n;
ItemType key[SIZE_n];


void Adp_SymPSort(void *a, int n, int es, int (*cmp)(const void *, const void *));
void callChensort(void **A, int size, int (*compar ) () ) { 
  struct intval *pi;
  Adp_SymPSort(A, size, sizeof(pi), compar);
}

// void check(const void * a);
/*
main(void)
{   int  i;

    printf("\n Adaptive Symmetry Partition Sort \n",n);
    srand(2007);
    for(i=0;i<n;i++) key[i]=rand()%n; // generate a random integer in [0,32767],
   	Adp_SymPSort((char *)key,n,sizeof(ItemType),cmp);
    check(key); //verify output;
}
*/
 /*
void check(const void * a)
{    int i;
     for (i=0; i< n-1; i++){
          if (*((ItemType *)a+i)>*((ItemType *)a+i+1))
          {
                printf( "\nThe sequence is not ordered");
                return;
          }
     }
     printf( "\nThe sequence is correctly sorted");
}
 */
// the code for the sorting algorithm begins  
  /*
#define swapvector(TYPE,pi,pj,n)            \
	do {                                    \
       TYPE t= *(TYPE *) (pi);              \
       *(TYPE *) (pi) = *(TYPE *) (pj);     \
       *(TYPE *) (pj) = t;                  \
       pi+=sizeof(TYPE); pj+=sizeof(TYPE);  \
       n-=sizeof(TYPE);                     \
  	} while (n > 0);
 
void swapfunc(char *a, char *b, int n, int swaptype)
{    if (swaptype <=1 ) swapvector(long,a,b,n)
     else swapvector(char,a,b,n)
}

#define swap(a,b)                          \
    if (swaptype == 0) {                   \
       long t = * (long *) (a);            \
       * (long *) (a) = * (long *) (b);    \
       * (long *) (b) = t;                 \
	}                                      \
    else swapfunc(a,b,es,swaptype)

#define SWAPINIT(a,es) swaptype =                           \
 (a - (char *) 0) % sizeof(long) || es % sizeof(long) ? 2 : \
 es == sizeof(long) ? 0 : 1
  */


#define p 16
#define beta1 256
#define beta2 512
// Symmetry Partition Sort
void SymPartitionSort(char *a, int s, int n, int es, int (*cmp)(const void *,const void *))
{   char *pm,*pb,*pc,*pi,*pj;
    int i,v,vL,m,left,right,swaptype,sp,eq,ineq,rc;
    left=right=0;
    SWAPINIT(a,es);
    while(1){
        if(n < 8){ //Insertion sort on small arrays
             for (s=1; s < n; s++)
                for (pb = a+s*es; cmp(pb-es,pb) > 0; ) {
                        swap(pb,pb-es); pb-=es; 
                        if(pb <= a) break;
                }
             return;
        }
        m= s<0 ? -s:s;
        if(m <= 2){//First,middle,last items are ordered and placed 1st,2nd and last
            v = beta2 > n ? n : 63;
            pc=a+(v-1)*es;
            pm=a+es; 
            swap(pm,a+(v/2)*es);
            if(cmp(a, pm) > 0) {swap(a,pm);}
                if((cmp(pm, pc) > 0)) {
                      swap(pm,pc);
                      if((cmp(a, pm) > 0)) {swap(a,pm);}
                }
                left=right=1; pc-=es;
            }
            else{
               v=m > n/beta1 ? n : p*m-1;
               if(s < 0) {  //Move sorted items to left end
                      if(v<n) {left=m; s=-s;}
                      else    {left=(m+1)/2; right=m/2;} 
                      swapfunc(a, a+(n-m)*es, left*es, swaptype);
                      left--;
               }
               if(s>0){
                      pb=a+m*es; pc=a+v*es;  
                      if(v < n){ //Extract sampling items 
                          sp=(n/v)*es; pj=pb; pi=pb;  
                          for(; pi < pc; pi+=es, pj+=sp) swap(pi,pj);
                      }
                      i=right=m/2; //Right move sorted items
                      do{ pb-=es; pc-=es; swap(pb,pc); i--;} while (i);
                      left=(m-1)/2; 
               }
               pm=a+left*es; pc=pm+(v-m)*es;
            }
//Fat partition begins
        pb=pi=pm+es;  
        do {
            while ( (rc=cmp(pb,pm)) < 0 ) pb+=es;
            if(pb >= pc) break;
            if(rc==0){
				if(pi!=pb) {swap(pb,pi);}
                 pi+=es; pb+=es;
                 continue;
            }
            while ((rc=cmp(pc,pm)) > 0 ) pc-=es;
            if(pb >= pc) break;
            swap(pb,pc);
            if(rc==0){
				if(pi!=pb) { swap(pb,pi);}
                pi+=es; 
            }
            pb+=es; pc-=es;
        } while (pb <= pc);
//Move equal-key items
        eq=pi-pm, ineq=pb-pi;
        if( ineq < eq) pi=pm+ineq;
        pc=pb;
        while (pm < pi ) { pc-=es; swap(pc,pm); pm+=es;} 
//Fat partition ends
            vL=(pb-a)/es; 
            if(right < v-vL) SymPartitionSort(pb, -right, v-vL, es, cmp);
            vL=vL-eq/es; 
            if(v < n){
                if(left < vL) SymPartitionSort(a, left,vL,es,cmp);
                s=v;  //Remove tail recursion
            }
            else{
                if(left >= vL) return;
                s=left; n=vL; //Remove tail recursion
            }
    }
} // end SymPartitionSort

// Adaptive Symmetry Partition Sort
void Adp_SymPSort(void *a, int n, int es, int (*cmp)(const void *,const void *))
{   char *pb,*pc,*pi,*pj;
    int swaptype,i,j,ne,rc,D_inv,left,m,Rev=0;
  
    SWAPINIT((char *)a,es);
//Find 1st run
    ne = n * es;
    for (i=es; i < ne; i+=es){
         if((rc=cmp((char *)a+i-es, (char *)a+i)) != 0 ){
             if(Rev==0) Rev= rc < 0 ? 1 : -1;//Rev=1: increasing, -1: decreasing
             else if(rc*Rev > 0) break;
         }
    }
    D_inv= Rev*(i/es);   //D_inv: difference of inversions & orders
    for(j=i+es; j < ne; j+=(97*es)){
         if((rc=cmp((char *)a+j-es, (char *)a+j)) < 0) D_inv++;
         if(rc>0) D_inv--;
    }
    pb=(char *)a+i-es;
    if(abs(D_inv) > n/512 ) {     
         if(Rev*D_inv < 0) {pb=(char *)a; Rev=-Rev;}  //If 1st run is reverse, re-find it
            pc=(char *)a+n*es; pj=pb;
            while(1){
                pj=pj+10*es; pi=pj-es;
                if(pj >= pc) break;
                while (pj < pc && Rev*cmp(pj-es, pj) <=0) pj+=es; //Find next run foreward
                while (pi > pb && Rev*cmp(pi-es, pi) <=0) pi-=es; //Find next run backward
                if(pj-pi < 4*es) continue;
                if(pb!=a) { //Find knots in 1st and 2nd run 
                      j=((pj-pi)/es)/2;
                      m=((pb-(char *)a)/es)/4;
                      if (j > m ) j=m;
                      for(i=0; i<j; i++) if(Rev*cmp(pb-i*es,pi+i*es) <= 0) break;
                      if(i>=j) continue;
                      pb=pb+(1-i)*es; pi=pi+i*es;
                }
                // Merge two runs by moving 2nd knot to 1st knot 
                if(pi!=pb) while(pi < pj ) { swap(pb,pi); pb+=es; pi+=es;}
                else pb=pj;
                pb-=es;
            }
    }   
    left=(pb-(char *)a)/es+1;
    if(Rev==-1){ //if the longest run reverse, reverse it
        pc=(char *)a;
        while(pc < pb ) {swap(pc,pb); pc+=es; pb-=es; }
    }
    if(left < n) SymPartitionSort((char *)a, left, n, es, cmp);
} // end Adp_SymPSort


// Bentley test-bench content generators
/*
void reverse();
void reverseFront();
void reverseBack();
void tweakSort();
void dither();
*/
void reverse2();
void reverse(void **A, int n) {
  reverse2(A, 0, n-1);
} // end reverse
void reverse2(void **A, int start, int end) {
  void *x;
  while ( start < end ) {
    x = A[start]; A[start++] = A[end]; A[end--] = x;
  }    
} // end reverse2
void reverseFront(void **A, int n) {
  reverse2(A, 0, n/2);
} // end reverseFront
void reverseBack(void **A, int n) {
  reverse2(A, n/2, n-1);
} // end reverseBack

void tweakSort(void **AA, int n) {
  /*
  A = AA;
  compareXY = compareIntVal;
  cut4(0, n-1);
  */
  callCut2(AA, n, compareIntVal);
} // end tweakSort
void dither(void **A, int n) {
  int k;
  struct intval *pi;
  for (k = 0; k < n; k++) {
    // A[k] = A[k] + (k % 5);
    pi = (struct intval *)A[k];
    pi->val = pi->val + (k % 5);
  }
} // end dither

void sawtooth(void **A, int n, int m, int tweak) {
  // int *A = malloc (sizeof(int) * n);
  struct intval *pi;
  int k;
  for (k = 0; k < n; k++) {
    pi = (struct intval *)A[k];
    pi->val = k % m; 
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end sawtooth

void rand2(void **A, int n, int m, int tweak, int seed) {
  srand(seed);
  int k;
  struct intval *pi;
  for (k = 0; k < n; k++) {
    // A[k] = rand() % m;
    pi = (struct intval *)A[k];
    pi->val = rand() % m;
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end rand2

void stagger(void **A, int n, int m, int tweak) {
  int k;
  struct intval *pi;
  for (k = 0; k < n; k++) {
    // A[k] = (k*m+k) % n;
    pi = (struct intval *)A[k];
    pi->val = (k*m+k) % n;
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end stagger

void plateau(void **A, int n, int m, int tweak) {
  int k;
  struct intval *pi;
  for (k = 0; k < n; k++) {
    // A[k] = ( k <= m ? k : m );
    pi = (struct intval *)A[k];
    pi->val = ( k <= m ? k : m );
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end plateau

void shuffle(void **A, int n, int m, int tweak, int seed) {
  srand(seed);
  int k, i, j;
  i = j = 0;
  struct intval *pi;
  for (k = 0; k < n; k++) {
    pi = (struct intval *)A[k];
    if ( 0 == ( rand() %m ) ) { 
      // j = j+2; A[k] = j;
      j = j+2;  pi->val = j;
    } else {
      // i = i+2; A[k] = i;
      i = i+2;  pi->val = i;
    }
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end shuffle

void slopes(void **A, int n, int m, int tweak) {
  int k, i, b, ak;
  i = k = b = 0; ak = 1;
  struct intval *pi;
  while ( k < n ) {
    if (1000000 < ak) ak = k; else
    if (ak < -1000000) ak = -k;
    // A[k] = -(ak + b); ak = A[k];
    pi = (struct intval *)A[k];
    ak = -(ak + b);
    pi->val = ak;
    k++; i++; b++;
    if ( 11 == b ) { b = 0; }
    if ( m == i ) { ak = ak*2; i = 0; }
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end slopes

// Using the Bentley test-bench distributions to validate SixSort;
// heapsort is the trusted algorithm.
void validateSixSortBT() {
  printf("Entering validateSixSortBT Sawtooth ........\n");
  // printf("Entering validateSixSortBT Rand2 ........\n");
  // printf("Entering validateSixSortBT Plateau ........\n");
  // printf("Entering validateSixSortBT Shuffle ........\n");
  // printf("Entering validateSixSortBT Stagger ........\n");
  int sortcBTime, cut2Time, T;
  int seed =  666;
  int z;
  int siz = 1024*1024;
  // int limit = 1024 * 1024 * 16 + 1;
  // int seedLimit = 32 * 1024;
  int limit = siz + 1;
  // int seedLimit = 32;
  int seedLimit = 1;
  float frac;
  while (siz <= limit) {
    printf("%s %d %s %d %s", "siz: ", siz, " seedLimit: ", seedLimit, "\n");
    // int A[siz];
    struct intval *pi;
    void **A = myMalloc("compareAlgorithms0 1", sizeof(pi) * siz);
    void **B = myMalloc("compareAlgorithms0 3", sizeof(pi) * siz);
    // construct array
    int i;
    for (i = 0; i < siz; i++) {
      pi = myMalloc("compareAlgorithms0 2", sizeof (struct intval));
      A[i] = pi;
      pi = myMalloc("compareAlgorithms0 4", sizeof (struct intval));
      B[i] = pi;
    };
    // warm up the process
    fillarray(A, siz, seed);
    int TFill, m, tweak;
    int sortcBCnt, cut2Cnt; // , sortcBCntx, cut2Cntx;
    int sumQsortB, sumCut2; // , sumQsortBx, sumCut2x;
    // for (z = 0; z < 3; z++) { // repeat to check stability
    for (z = 0; z < 1; z++) { // repeat to check stability
      sortcBCnt = cut2Cnt = sumQsortB = sumCut2 = 0;
      // sortcBCntx = cut2Cntx = sumQsortBx = sumCut2x = 0;
      for (m = 1; m < 2 * siz; m = m * 2) {
      // m = 1024 * 1024; {
      // m = 1; {
      	for (tweak = 0; tweak <= 5; tweak++ ) {
	// tweak = 5; {
	  sortcBTime = 0; cut2Time = 0;
	  TFill = clock();
	  for (seed = 0; seed < seedLimit; seed++) 
	    sawtooth(A, siz, m, tweak);
	    // rand2(A, siz, m, tweak, seed);
	    // plateau(A, siz, m, tweak);
	    // shuffle(A, siz, m, tweak, seed);
	    // stagger(A, siz, m, tweak);
	  TFill = clock() - TFill;
	  T = clock();
	  for (seed = 0; seed < seedLimit; seed++) { 
	    sawtooth(A, siz, m, tweak);
	    // rand2(A, siz, m, tweak, seed);
	    // plateau(A, siz, m, tweak);
	    // shuffle(A, siz, m, tweak, seed);
	    // stagger(A, siz, m, tweak);
	    callHeapsort(A, siz, compareIntVal);
	  }
	  sortcBTime = sortcBTime + clock() - T - TFill;
	  sumQsortB += sortcBTime;
	  // if ( 4 != tweak ) sumQsortBx += sortcBTime;
	  T = clock();
	  for (seed = 0; seed < seedLimit; seed++) { 
	    sawtooth(B, siz, m, tweak);
	    // rand2(B, siz, m, tweak, seed);
	    // plateau(B, siz, m, tweak);
	    // shuffle(B, siz, m, tweak, seed);
	    // stagger(B, siz, m, tweak);
	    sixsort(B, siz, compareIntVal);  
	  }
	  cut2Time = cut2Time + clock() - T - TFill;
	  sumCut2 += cut2Time;
	  // if ( 4 != tweak ) sumCut2x += cut2Time;
	  printf("Size: %d m: %d tweak: %d ", siz, m, tweak);
	  printf("sortcBTime: %d ", sortcBTime);
	  printf("Cut2Time: %d ", cut2Time);
	  frac = 0;
	  if ( sortcBTime != 0 ) frac = cut2Time / ( 1.0 * sortcBTime );
	  printf("frac: %f \n", frac);
	  if ( sortcBTime < cut2Time ) sortcBCnt++;
	  else cut2Cnt++;
	  for (i = 0; i < siz; i++) {
	    if ( compareIntVal(A[i], B[i]) != 0 ) {
	      printf("***** validateSixSortBT m: %i tweak: %i at i: %i\n", 
		     m, tweak, i);
	      exit(0);
	    }
	  }
	}
	printf("sumQsortB:   %i sumCut2:  %i frac: %f", 
	       sumQsortB, sumCut2, (sumCut2/(1.0 * sumQsortB)));
	printf(" sortcBCnt:  %i cut2Cnt:  %i\n", sortcBCnt, cut2Cnt);
      }
      frac = 0;
      if ( sumQsortB != 0 ) frac = sumCut2 / ( 1.0 * sumQsortB );
      printf("Measurements:\n");
      printf("sumQsortB:   %i sumCut2:  %i frac: %f", 
	     sumQsortB, sumCut2, (sumCut2/(1.0 * sumQsortB)));
      printf(" sortcBCnt:  %i cut2Cnt:  %i\n", sortcBCnt, cut2Cnt);
      // printf("sumQsortBx:  %i sumCut2x: %i", sumQsortBx, sumCut2x);
      // printf(" sortcBCntx: %i cut2Cntx: %i\n", sortcBCntx, cut2Cntx);

    }
    // free array
    for (i = 0; i < siz; i++) {
      free(A[i]); free(B[i]);
    };
    free(A); free(B);
    siz = siz * 2;
    seedLimit = seedLimit / 2;
  }
} // end validateSixSortBT

void compareBentleyAgainstSixsort() {
  printf("Entering compareBentleyAgainstCut3 Sawtooth ........\n");
  // printf("Entering compareBentleyAgainstCut3 Rand2 ........\n");
  // printf("Entering compareBentleyAgainstCut3 Plateau ........\n");
  // printf("Entering compareBentleyAgainstCut3 Shuffle ........\n");
  // printf("Entering compareBentleygAainstCut3 Stagger ........\n");
  int bentleyTime, cut3Time, T;
  int seed = 666;
  int z;
  // int siz = 1024;
  int siz = 1024*1024;
  // int limit = 1024 * 1024 * 16 + 1;
  // int seedLimit = 32 * 1024;
  int limit = siz + 1;
  // int seedLimit = 32;
  int seedLimit = 1;
  float frac;
  while (siz <= limit) {
    printf("%s %d %s %d %s", "siz: ", siz, " seedLimit: ", seedLimit, "\n");
    // int A[siz];
    // void **A = malloc (sizeof(int) * siz);
    // create array
    struct intval *pi;
    void **A = myMalloc("compareBentleyAgainstFivesort 1", sizeof(pi) * siz);
    int i;
    for (i = 0; i < siz; i++) {
      pi = myMalloc("compareBentleyAgainstFivesort 2", sizeof (struct intval));
      A[i] = pi;
    };

    // warm up the process
    fillarray(A, siz, seed);
    int TFill, m, tweak;
    int bentleyCnt, cut3Cnt; // , bentleyCntx, cut3Cntx;
    int sumQsortB, sumCut3; // , sumQsortBx, sumCut3x;
    // for (z = 0; z < 3; z++) { // repeat to check stability
    for (z = 0; z < 1; z++) { // repeat to check stability
      bentleyCnt = cut3Cnt = sumQsortB = sumCut3 = 0;
      // bentleyCntx = cut3Cntx = sumQsortBx = sumCut3x = 0;
      for (m = 1; m < 2 * siz; m = m * 2) {
      // m = 1024 * 1024; {
      	for (tweak = 0; tweak <= 5; tweak++ ) {
	  if (4 == tweak) continue; // due to bias 
	  bentleyTime = 0; cut3Time = 0;
	  TFill = clock();
	  for (seed = 0; seed < seedLimit; seed++) 
	    // sawtooth(A, siz, m, tweak);
	    // rand2(A, siz, m, tweak, seed);
	    // plateau(A, siz, m, tweak); // not used
	    // shuffle(A, siz, m, tweak, seed); // not used
	    // stagger(A, siz, m, tweak);
	    slopes(A, siz, m, tweak);
	  TFill = clock() - TFill;
	  T = clock();
	  for (seed = 0; seed < seedLimit; seed++) { 
	    // sawtooth(A, siz, m, tweak);
	    // rand2(A, siz, m, tweak, seed);
	    // plateau(A, siz, m, tweak); // not used
	    // shuffle(A, siz, m, tweak, seed);  // not used
	    // stagger(A, siz, m, tweak);
	    slopes(A, siz, m, tweak);
	    // callBentley(A, 0, siz-1); 
	    callCut2(A, siz, compareIntVal); 
	    // callQuicksort0(B, siz, compareIntVal); 
	  }
	  bentleyTime = bentleyTime + clock() - T - TFill;
	  sumQsortB += bentleyTime;
	  // if ( 4 != tweak ) sumQsortBx += bentleyTime;
	  T = clock();
	  for (seed = 0; seed < seedLimit; seed++) { 
	    // sawtooth(A, siz, m, tweak);
	    // rand2(A, siz, m, tweak, seed);
	    // plateau(A, siz, m, tweak); // not used
	    // shuffle(A, siz, m, tweak, seed); // not used
	    // stagger(A, siz, m, tweak);
	    slopes(A, siz, m, tweak);
	    sixsort(A, siz, compareIntVal);
	  }
	  cut3Time = cut3Time + clock() - T - TFill;
	  sumCut3 += cut3Time;
	  // if ( 4 != tweak ) sumCut3x += cut3Time;
	  printf("size: %d m: %d tweak: %d ", siz, m, tweak);
	  printf("bentleyTime: %d ", bentleyTime);
	  printf("Cut3Time: %d ", cut3Time);
	  frac = 0;
	  if ( bentleyTime != 0 ) frac = cut3Time / ( 1.0 * bentleyTime );
	  printf("frac: %f \n", frac);
	  if ( bentleyTime < cut3Time ) bentleyCnt++;
	  else cut3Cnt++;
	}
	printf("sumQsortB:   %i sumCut3:  %i frac: %f", 
	       sumQsortB, sumCut3, (sumCut3/(1.0 * sumQsortB)));
	printf(" bentleyCnt:  %i cut3Cnt:  %i\n", bentleyCnt, cut3Cnt);
      }
      frac = 0;
      if ( sumQsortB != 0 ) frac = sumCut3 / ( 1.0 * sumQsortB );
      printf("Measurements:\n");
      printf("sumQsortB:   %i sumCut3:  %i frac: %f", 
	     sumQsortB, sumCut3, (sumCut3/(1.0 * sumQsortB)));
      printf(" bentleyCnt:  %i cut3Cnt:  %i\n", bentleyCnt, cut3Cnt);
      // printf("sumQsortBx:  %i sumCut3x: %i", sumQsortBx, sumCut3x);
      // printf(" bentleyCntx: %i cut3Cntx: %i\n", bentleyCntx, cut3Cntx);
    }
    free(A);
    // siz = siz * 2;
    // seedLimit = seedLimit / 2;
    siz = siz * 4;
    seedLimit = seedLimit / 4;
  }

} // end compareBentleyAgainstSixsort
