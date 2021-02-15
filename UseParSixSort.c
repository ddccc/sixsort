/*
File: c:/bsd/rigel/sort/Licence.txt
Date: Sat Jun 09 22:22:31 2012, 2015

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
   This file is a test bench for excercising parallel sixsort and testing 
   it against other (parallel) algorithms.
   >>Specific objects and a specific comparison function are used throughout<<.
   Feel free to change them as you see fit.
  
   The comparison function given here does NOT work for qsort, because qsort 
   thinks that entities to be sorted reside in the array, while sixsort knows
   that the array contains pointers to objects.  It >is< possible, of course,
   to construct an alternative comparison function for qsort.

   To compile:  $ gcc UseParSixSort.c ParSixSort.o
   It produces the file: a.exe (or the like)


   The main function has many alternative functions.
   Un-comment one to activate.
   Inside such a function are often other choices that can be selected.
 */



#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
// #include <unistd.h>
#include <string.h>
#include <math.h>

/*
#include "Hsort.c"
#include "Dsort.c"
#include "Isort.c"
#include "Qusort.c"
*/

#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }
void heapc();
void insertionsort();
void dflgm();
#include "Qusort.c"

#define errexit(code,str)                          \
  fprintf(stderr,"%s: %s\n",(str),strerror(code)); \
  exit(1);


int NUMTHREADS = 2;

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
  return ((struct intval *)a)->val - ((struct intval *)b)->val;
}

// To avoid compiler warnings::
void callHeapsort(void **A, int size, int (*compar)() );
void heapc(void **A, int N, int M, int (*compar)());
void heapSort(void **A, int size, int (*compar)());
void quicksort0(void **A, int N, int M, int (*compar)());
void quicksort0c(void **A, int N, int M, int depthLimit, int (*compar)());
// void cut2c(void **A, int N, int M, int depthLimit, int (*compar)());
void dflgm(void **A, int N, int M, int pivotx, void (*cut)(), int depthLimit, int (*compar)());
void sixsort(void **AA, int size, 
	int (*compar ) (const void *, const void * ),
	int numberOfThreads);
void testCut2S();
void testSixsort();
void testSixSort2();
void testThreesort();
void timeTest();
void compareThreesortAgainstQuicksort0();
void compareSixsortAgainstQuicksort0();

void validateParSixSortBT();
void validateXYZ();

int main (int argc, char *argv[]) {
  printf("Running UseParSixSort ...\n");
  // Un-comment one of the following to use a specific functionality.
  // Modify the body of these functions as desired

  // To ask for the license expiration date and the host
     // sixsort(0, 0, 0);
  // To check that cut2S produces a sorted array
  // testCut2S();
  // To check that sixsort produces a sorted array
  // testSixsort();
  // Ditto but using the general function testAlgorithm
     // ... and uncomment also testSixSort2 ...
     // testSixSort2();
  // To check that threesort produces a sorted array
  // testThreesort();
  // Compare the outputs of two sorting algorithms
  // validateXYZ(); // must provide an other algorithm XYZ
     // ... and uncomment validateXYZ ...
  // Measure the sorting time of an algorithm
  timeTest();
  // Compare the speed fraction of two algorithms
     // compareSixsortAgainstXYZ();
     // ... and uncomment also compareSixsortAgainstXYZ ...
  // Whatever here:::
     // compareThreesortAgainstQuicksort0();
     // compareSixsortAgainstQuicksort0(); NONO
     // validateParSixSortBT();
     return 0;
} // end of main

void *myMalloc(char* location, int size) {
  void *p = malloc(size);
  if ( 0 == p ) {
    // printf("Cannot allocate memory in: %s\n", location);
    fprintf(stderr, "malloc fails for: %s\n", location);
    exit(1);
  }
  return p;
} // end myMalloc
 

// fillarray assigns random values to the int-field of our objects
void fillarray(void **A, int lng, int startv) {
  // printf("fillarray startv %i\n", startv);
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
  int i;
  void *x, *y;
  struct intval *pi;
  for (i = N + 1; i <= M; i++) {
    x = A[i-1]; y = A[i];
    if ( compareIntVal(y, x) < 0 ) {
      pi = (struct intval *) y;
      printf("%s %d %s %d %s", "Error at: ", i, 
	     " A[i]: ", pi->val, "\n");
    }
  }
} // end check

// initializing an array, sort it and check it
void testSixsort() {
  printf("Running testSixsort ...\n");
  int siz = 1024 * 1024 * 8;
  // int siz = 1024 * 1024;
  // int siz = 15;
  // create array
  struct intval *pi;
  void **A = myMalloc("testSixsort 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("testSixsort 2", sizeof (struct intval));
    A[i] = pi;
  };
  // fill its content
  fillarray(A, siz, 100);
  // sort it
  // int t0 = clock();
  
    struct timeval tim;
    gettimeofday(&tim, NULL);
    double t0=tim.tv_sec+(tim.tv_usec/1000000.0);
  
  // int compareIntVal();
  sixsort(A, siz, compareIntVal, NUMTHREADS);
  // int t1 = clock();
  gettimeofday(&tim, NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  // and check it
  check(A, 0, siz-1);
  printf("Sorting size: %i time: %.2f\n", siz, t1-t0);
  // printf("Sorting time: siz: %d duration: %i\n", siz, t1-t0);
} // end testSixsort()

void threesort();
void testThreesort() {
  printf("Running testThreesort ...\n");
  // int siz = 1024 * 1024 * 8;
  // int siz = 1024 * 1024;
  // int siz = 15;
  // create array
  int siz = 1024;
  while ( siz < 1024 * 1024 * 16 ) {
    siz = siz *2;
    struct intval *pi;
    void **A = myMalloc("testThreesort 1", sizeof(pi) * siz);
    int i;
    for (i = 0; i < siz; i++) {
      pi = myMalloc("testThreesort 2", sizeof (struct intval));
      A[i] = pi;
    };
    // fill its content
    fillarray(A, siz, 100);
    // sort it
    // t0 = clock();
    struct timeval tim;
    gettimeofday(&tim, NULL);
    double t0=tim.tv_sec+(tim.tv_usec/1000000.0);
    // int compareIntVal();
    threesort(A, siz, compareIntVal, NUMTHREADS);
    // int t1 = clock();
    gettimeofday(&tim, NULL);
    double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
    // and check it
    check(A, 0, siz-1);
    // printf("Sorting size: %d time: %d\n", siz, t1-t0);
    printf("Sorting time: siz: %d duration: %.2f\n", siz, t1-t0);
  }
} // end testThreesort()


// testAlgorithm0 is a generalization of testSixsort;
// another algorithm can be plugged in
// alg1 is a parallel algorithm
void testAlgorithm0(char* label, int siz, void (*alg1)() ) {
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
  // int compareIntVal();
  (*alg1)(A, siz, compareIntVal, NUMTHREADS);
  // and check it
  check(A, 0, siz-1);
} // end testAlgorithm0

// like testAlgorithm0 but the size of array is preset inside testAlgorithm
void testAlgorithm(char* label, void (*alg1)() ) {
  // testAlgorithm0(label, 1024 * 1024, alg1);
  testAlgorithm0(label, 1024, alg1);
  // testAlgorithm0(label, 15, alg1);
} // end testAlgorithm0

// Example: use of testAlgorithm
void testSixSort2() {
  void sixsort();
  testAlgorithm("Running sixsort ...", sixsort);
}

// alg1 is a sequential algorithm
void testAlgorithm0S(char* label, int siz, void (*alg1)() ) {
  printf("%s on size: %d\n", label, siz);
  // create array
  struct intval *pi;
  void **A = myMalloc("testAlgorithm0S 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("testAlgorithm0S 2", sizeof (struct intval));
    A[i] = pi;
  };
  // fill its content
  fillarray(A, siz, 100);
  // sort it
  // int compareIntVal();
  (*alg1)(A, siz, compareIntVal);
  // and check it
  check(A, 0, siz-1);
} // end testAlgorithm0S

// like testAlgorithm0 but the size of array is preset inside testAlgorithm
void testAlgorithmS(char* label, void (*alg1)() ) {
  testAlgorithm0(label, 1024 * 1024, alg1);
  // testAlgorithm0S(label, 100, alg1);
} // end testAlgorithm0

// Example: use of testAlgorithm
void testQuicksort0S() {
  void callQuicksort0();
  testAlgorithmS("Running quicksort0S ...", callQuicksort0);
}

// validateAlgorithm0 is used to check algorithm alg1 against a
// trusted algorithm alg2
// The check consists of making sure that starting from identical
// inputs they produce identical outputs
// alg1 is a parallel one, alg2 is sequential
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
  // int compareIntVal();
  (*alg1)(A, siz, compareIntVal, NUMTHREADS);

  // create the input for alg2 ...
  void **B = myMalloc("validateAlgorithm0 3", sizeof(pi) * siz);
  for (i = 0; i < siz; i++) {
    pi =  myMalloc("validateAlgorithm0 4", sizeof (struct intval));
    B[i] = pi;
  };
  fillarray(B, siz, 100);
  // ... sort it
  // int compareIntVal();
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
} // end validateAlgorithm0

// Like validateAlgorithm0 but with fixed array size
// alg1 is a parallel one, alg2 is sequential
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
void validateXYZ() {
  void sixsort(), callQuicksort0();
  validateAlgorithm("Running validate using callQuicksort0 ...",
		    sixsort, callQuicksort0);
}
/*
void validateXYZ() {
  void threesort(), callQuicksort0();
  validateAlgorithm("Running validate using callQuicksort0 ...",
		    threesort, callQuicksort0);
}
*/

// Run an algorithm and report the time used
void timeTest() {
  double algTime, T;
  int seed;
  int seedLimit = 3;
  int z;
  // int siz = 1024 * 1024 * 16;
  int siz = 1024 * 1024 * 4;
  // int siz = 1024 * 5;
  printf("timeTest() of sixsort on size %i\n", siz);
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
  float sumTimes = 0;
  for (z = 0; z < 3; z++) { // repeat to check stability
    // measure the array fill time
    // int TFill = clock();
    
    struct timeval tim;
    gettimeofday(&tim, NULL);
    double TFILL=tim.tv_sec+(tim.tv_usec/1000000.0);
    
    for (seed = 0; seed < seedLimit; seed++) 
      fillarray(A, siz, seed);
      // here alternative ways to fill the array
      // int k;
      // for ( k = 0; k < siz; k++ ) A[k] = 0;
      // for ( k = 0; k < siz; k++ ) A[k] = k%5;
      // for ( k = 0; k < siz; k++ ) A[k] = siz-k;
    // TFill = clock() - TFill;
    gettimeofday(&tim, NULL);
    TFILL=tim.tv_sec+(tim.tv_usec/1000000.0) - TFILL;
    // now we know how much time it takes to fill the array
    // measure the time to fill & sort the array
    // T = clock();
    gettimeofday(&tim, NULL);
    T=tim.tv_sec+(tim.tv_usec/1000000.0);
    for (seed = 0; seed < seedLimit; seed++) { 
      fillarray(A, siz, seed);
      // for ( k = 0; k < siz; k++ ) A[k] = 0;
      // for ( k = 0; k < siz; k++ ) A[k] = k%5;
      // for ( k = 0; k < siz; k++ ) A[k] = siz-k;
      sixsort(A, siz, compareIntVal, NUMTHREADS);
    }
    // ... and subtract the fill time to obtain the sort time
    // algTime = clock() - T - TFill;
    gettimeofday(&tim, NULL);
    algTime=tim.tv_sec+(tim.tv_usec/1000000.0) - T - TFILL;
    printf("algTime: %f \n", algTime);
    sumTimes = sumTimes + algTime;
  }
  printf("%s %f %s", "average: ", sumTimes/seedLimit, "\n");
} // end timeTest()

// Report the speed fraction of two algorithms on a range of array sizes
// alg1 is parallel, alg2 is sequential
void compareAlgorithms0(char *label, int siz, int seedLimit, void (*alg1)(), void (*alg2)() ) {
  printf("%s on size: %d seedLimit: %d #threads: %d\n", label, siz, seedLimit, NUMTHREADS);
  double alg1Time, alg2Time, T;
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
    for (z = 0; z < 5; z++) { // repeat to check stability
      // int TFill = clock();
	struct timeval tim;
	gettimeofday(&tim, NULL);
	double TFILL=tim.tv_sec+(tim.tv_usec/1000000.0);
      for (seed = 0; seed < seedLimit; seed++) 
	fillarray(A, siz, seed);
      // TFill = clock() - TFill;
      gettimeofday(&tim, NULL);
      TFILL=tim.tv_sec+(tim.tv_usec/1000000.0) - TFILL;
      // T = clock();
      gettimeofday(&tim, NULL);
      T=tim.tv_sec+(tim.tv_usec/1000000.0);
      for (seed = 0; seed < seedLimit; seed++) { 
	fillarray(A, siz, seed);
	(*alg1)(A, siz, compareIntVal, NUMTHREADS); 
      }
      // alg1Time = clock() - T - TFill;
      gettimeofday(&tim, NULL);
      alg1Time=tim.tv_sec+(tim.tv_usec/1000000.0) - T - TFILL;

      // T = clock();
      gettimeofday(&tim, NULL);
      T=tim.tv_sec+(tim.tv_usec/1000000.0);
      for (seed = 0; seed < seedLimit; seed++) { 
	fillarray(A, siz, seed);
	(*alg2)(A, siz, compareIntVal);
      }
      // alg2Time = clock() - T - TFill;
      gettimeofday(&tim, NULL);
      alg2Time=tim.tv_sec+(tim.tv_usec/1000000.0) - T - TFILL;

      printf("%s %d %s", "siz: ", siz, " ");
      printf("%s %f %s", "alg1Time: ", alg1Time, " ");
      printf("%s %f %s", "alg2Time: ", alg2Time, " ");
      float frac = 0; float frac2 = 0;
      if ( alg1Time != 0 &&  alg2Time != 0) { 
	frac = alg2Time / ( 1.0 * alg1Time ); frac2 = 1.0/frac;
      }
      printf("%s %f %s %f %s", "frac: ", frac, "frac2: ", frac2, "\n");
    }
    // free array
    for (i = 0; i < siz; i++) {
      free(A[i]);
    };
    free(A);
    siz = siz * 2;
    seedLimit = seedLimit / 2;
  }
} // end compareAlgorithms0

// alg1 is parallel, alg2 is sequential
void compareAlgorithms(char *label, void (*alg1)(), void (*alg2)() ) {
  // compareAlgorithms0(label, 1024, 32 * 1024, alg1, alg2);
  compareAlgorithms0(label, 1024 * 1024, 32, alg1, alg2);
  // compareAlgorithms0(label, 1024 * 1024 * 16, 2, alg1, alg2);
} // end compareAlgorithms

/* Example, replace XYZ by what you want to compare against
void compareSixsortAgainstXYZ() {
  void sixsort(), XYZ();
  compareAlgorithms("Compare sixsort vs XYZ", sixsort, XYZ);
}
*/

void compareThreesortAgainstQuicksort0() {
  void threesort(), callQuicksort0();
  compareAlgorithms("Compare threesort vs quicksort0", threesort, callQuicksort0);
} // end compareThreesortAgainstQuicksort0

void compareSixsortAgainstQuicksort0() {
  void  sixsort(), callQuicksort0();
  compareAlgorithms("Compare sixsort vs quicksort0", sixsort, callQuicksort0);
} // end compareSixsortAgainstQuicksort0


// Here are global entities used throughout

// void **A;
// int (*compareXY)();
void quicksort0();
void callQuicksort0(void **A, int size, int (*compar ) () ) {
  quicksort0(A, 0, size-1, compar);
} // end callQuicksort0

int sleepingThreads0;
struct stack *llx;

// infrastructure for parallel threesort
struct stack *newStack();
struct task *newTask();
void push(struct stack *ll, struct task *t);
struct task *pop();
void *getA(struct task *t);
int getN(struct task *t);
int getM(struct task *t);
int getDL(struct task *t);
void *getXY(struct task *t);

pthread_mutex_t condition_mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond3  = PTHREAD_COND_INITIALIZER;

void addTaskSynchronized0(struct stack *ll, struct task *t)
 {
  // printf("Thread#: %ld ", pthread_self());
  // printf("addTask N: %d M: %d sleepingThreads0: %d\n", 
  //	 getN0(t), getM0(t), sleepingThreads0);
  pthread_mutex_lock( &condition_mutex3 );
  push(ll, t);
  pthread_cond_signal( &condition_cond3 );
  pthread_mutex_unlock( &condition_mutex3 );
 } // end addTaskSynchronized0

/* threesort is a 4-level sorter:
   parallel quicksort0
   sequential quicksort0
   sequential quicksort0
   sequential insertionsort
 */

void quicksort0pc();
void *sortThread3(void *AAA) { // AAA-argument is NOT used
  // int taskCnt = 0;
  //  printf("Thread number: %ld #sleepers %d\n", 
  //         pthread_self(), sleepingThreads);
  struct task *t = NULL;
  for (;;) {
    pthread_mutex_lock( &condition_mutex3 );
       sleepingThreads0++;
       while ( NULL == ( t = pop(llx) ) && 
	       sleepingThreads0 < NUMTHREADS ) {
	 pthread_cond_wait( &condition_cond3, &condition_mutex3 );
       }
       if ( NULL != t ) sleepingThreads0--;
    pthread_mutex_unlock( &condition_mutex3 );
    if ( NULL == t ) { 
      pthread_mutex_lock( &condition_mutex3 );
      pthread_cond_signal( &condition_cond3 );
      pthread_mutex_unlock( &condition_mutex3 );
      break;
    }
    void **A = getA(t);
    int n = getN(t);
    int m = getM(t);
    int depthLimit = getDL(t);
    int (*compar)() = getXY(t);
    free(t);
    // taskCnt++;
    quicksort0pc(A, n, m, depthLimit, compar);
  }
  // printf("Exit of Thread number: %ld taskCnt: %d\n", pthread_self(), taskCnt);
    return NULL;
 }  // end sortThread3

// void iswap();
#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }

void *partitionThreadLeft();
void *partitionThreadRight();
const int quicksort0SLimit; // 2000
void threesort(void **A, int size, 
	  int (*compareXY ) (const void *, const void * ),
	  int num) {
  // printf("threesort size %i\n", size);
  if ( size <= quicksort0SLimit || num <= 1) {
    callQuicksort0(A, size, compareXY);
    return;
  }
  sleepingThreads0 = 0;
  NUMTHREADS = num;
  // A = AA;
  // compareXY = compar;
  pthread_t thread_id[NUMTHREADS];
  llx = newStack();
  int depthLimit = 2.9 * floor(log(size));

  //   struct task *t = newTask(0, size-1, depthLimit);
  //  addTaskSynchronized0(llx, t);

  // /*
  // Try doing the first partition in parallel with two threads
  int N = 0; int M = size-1; 
  int L = M-N;

  // Check for duplicates
  int sixth = (L + 1) / 6;
  int e1 = N  + sixth;
  int e5 = M - sixth;
  int e3 = N + (L>>1); // N + L/2; // The midpoint
  int e4 = e3 + sixth;
  int e2 = e3 - sixth;
  
  // Sort these elements using a 5-element sorting network
  void *ae1 = A[e1], *ae2 = A[e2], *ae3 = A[e3], *ae4 = A[e4], *ae5 = A[e5];
  void *t;
  // if (ae1 > ae2) { t = ae1; ae1 = ae2; ae2 = t; }
  if ( 0 < compareXY(ae1, ae2) ) { t = ae1; ae1 = ae2; ae2 = t; } // 1-2
  if ( 0 < compareXY(ae4, ae5) ) { t = ae4; ae4 = ae5; ae5 = t; } // 4-5
  if ( 0 < compareXY(ae1, ae3) ) { t = ae1; ae1 = ae3; ae3 = t; } // 1-3
  if ( 0 < compareXY(ae2, ae3) ) { t = ae2; ae2 = ae3; ae3 = t; } // 2-3
  if ( 0 < compareXY(ae1, ae4) ) { t = ae1; ae1 = ae4; ae4 = t; } // 1-4
  if ( 0 < compareXY(ae3, ae4) ) { t = ae3; ae3 = ae4; ae4 = t; } // 3-4
  if ( 0 < compareXY(ae2, ae5) ) { t = ae2; ae2 = ae5; ae5 = t; } // 2-5
  if ( 0 < compareXY(ae2, ae3) ) { t = ae2; ae2 = ae3; ae3 = t; } // 2-3
  if ( 0 < compareXY(ae4, ae5) ) { t = ae4; ae4 = ae5; ae5 = t; } // 4-5
  // ... and reassign
  A[e1] = ae1; A[e2] = ae2; A[e3] = ae3; A[e4] = ae4; A[e5] = ae5;

  // Fix end points
  if ( compareXY(ae1, A[N]) < 0 ) iswap(N, e1, A);
  if ( compareXY(A[M], ae5) < 0 ) iswap(M, e5, A);
  
  void *T = ae3; // pivot

  // check Left label invariant
  // if ( T <= A[N] || A[M] < T ) {
  if ( compareXY(T, A[N]) <= 0 || compareXY(A[M], T) < 0 ) {
    // cannot do first parallel partition
    struct task *t = newTask(A, 0, M, depthLimit, compareXY);
    addTaskSynchronized0(llx, t);
  } else {
   
    //   |------------------------|------------------------|
    //   N                        e3                       M
    //  A[N] < T             A[e3] = T                 T<=A[M]

    struct task *t1 = newTask(A, N, e3, 0, compareXY);
    struct task *t2 = newTask(A, e3, M, 0, compareXY);
    int errcode;
    if ( (errcode=pthread_create(&thread_id[1], NULL, 
				 partitionThreadLeft, (void*) t1) )) {
      errexit(errcode,"ParSixSort/sixsort()/pthread_create");
    }
    if ( (errcode=pthread_create(&thread_id[2], NULL, 
				 partitionThreadRight, (void*) t2)) ) {
      errexit(errcode,"ParSixSort/sixsort()/pthread_create");
    }
    if ( (errcode=pthread_join(thread_id[1], NULL) )) {
      errexit(errcode,"ParSixSort/sixsort()/pthread_join");
    }
    int i1 = getN(t1); free(t1);
    if ( (errcode=pthread_join(thread_id[2], NULL) )) {
      errexit(errcode,"ParSixSort/sixsort()/pthread_join");
    }
    int i2 = getN(t2); free(t2);
    //      LL            LR           RL           RR
    //   |--------------]----------][-----------]-----------------|
    //   N     <        i1   >=    e3    <      i2      >=        M
    //   i2 = e3 -> RL is empty
    int k; int m3;
    if ( e3 == i2 ) { m3 = i1; } else {
      int middle2 = e3+1;
      // int b = middle - i1; int c = i2 - middle2;
      int b = e3 - i1; int c = i2 - middle2;
      // swap the two middle segments
      if ( b <= c ) {
	// printf("b <= c\n");
	for ( k = 0; k < b; k++ ) // iswap(e3-k, i2-k, A);
	  { int xx = e3-k, yy = i2-k; iswap(xx, yy, A); } 
	m3 = i2 - b;
      }
      else {
	// printf("c < b\n");
	for ( k = 0; k < c+1; k++ ) // iswap(middle2+k, i1+1+k, A);
	  { int xx = middle2+k, yy = i1+1+k; iswap(xx, yy, A); } 
	m3 = i1 + c+1;
      }
    }

    //  |------------------------][----------------------------|
    //  N           <           m3           >=                M
    t1 = newTask(A, N, m3, depthLimit, compareXY);
    addTaskSynchronized0(llx, t1);
    t1 = newTask(A, m3+1, M, depthLimit, compareXY);
    addTaskSynchronized0(llx, t1);
  }
  // */
  
  // printf("Entering threesort sortArray\n");
  int i; int errcode; 

  for ( i = 0; i < NUMTHREADS; i++ ) {
    if ( (errcode=pthread_create(&thread_id[i], NULL, 
				 sortThread3, (void*) A)) ) {
      errexit(errcode,"ParSort/ sortArray()/pthread_create");
    }
  }
  for ( i = 0; i < NUMTHREADS; i++ ) {
    if ( (errcode=pthread_join(thread_id[i], NULL)) ) {
      errexit(errcode,"ParSort/ sortArray()/pthread_join");
    }
  }
  free(llx);
  // printf("Exiting sortArray\n");
} // end threesort


// int quicksort0SLimit = 2000;
int quicksort0SLimit2 = 1000;

void quicksort0pc();
void quicksort0p(void **A, uint N, int M, int (*compar)() ) {
  int L = M - N;
  if ( L < quicksort0SLimit ) { 
    quicksort0(A, N, M, compar);
    return;
  }
  int depthLimit = 2.9 * floor(log(L));
  quicksort0pc(A, N, M, depthLimit, compar);
} // end quicksort0p

// void heapc();
// void quicksort0c(void **A, int N, int M, int depthLimit, int (*compar)());
// quicksort0pc is pretty close to parallel FourSort
void quicksort0pc(void **A, int N, int M, int depthLimit, int (*compareXY)()) {
        int L;
 Loop:
	// printf("quicksort0pc N %i M %i\n", N, M);
	if ( depthLimit <= 0 ) {
	  heapc(A, N, M, compareXY);
	  return;
	}
	L = M - N;
	if ( L <= quicksort0SLimit ) { 
	  quicksort0c(A, N, M, depthLimit, compareXY);
	  return;
	}
	depthLimit--;

	// Check for duplicates
        int sixth = (L + 1) / 6;
        int e1 = N  + sixth;
        int e5 = M - sixth;
	int e3 = N+ (L>>1); // N + L/2; // The midpoint
        int e4 = e3 + sixth;
        int e2 = e3 - sixth;

        // Sort these elements using a 5-element sorting network
        void *ae1 = A[e1], *ae2 = A[e2], *ae3 = A[e3], *ae4 = A[e4], *ae5 = A[e5];
	void *t;
        // if (ae1 > ae2) { t = ae1; ae1 = ae2; ae2 = t; }
	if ( 0 < compareXY(ae1, ae2) ) { t = ae1; ae1 = ae2; ae2 = t; } // 1-2
	if ( 0 < compareXY(ae4, ae5) ) { t = ae4; ae4 = ae5; ae5 = t; } // 4-5
	if ( 0 < compareXY(ae1, ae3) ) { t = ae1; ae1 = ae3; ae3 = t; } // 1-3
	if ( 0 < compareXY(ae2, ae3) ) { t = ae2; ae2 = ae3; ae3 = t; } // 2-3
	if ( 0 < compareXY(ae1, ae4) ) { t = ae1; ae1 = ae4; ae4 = t; } // 1-4
	if ( 0 < compareXY(ae3, ae4) ) { t = ae3; ae3 = ae4; ae4 = t; } // 3-4
	if ( 0 < compareXY(ae2, ae5) ) { t = ae2; ae2 = ae5; ae5 = t; } // 2-5
	if ( 0 < compareXY(ae2, ae3) ) { t = ae2; ae2 = ae3; ae3 = t; } // 2-3
	if ( 0 < compareXY(ae4, ae5) ) { t = ae4; ae4 = ae5; ae5 = t; } // 4-5
	// ... and reassign
	A[e1] = ae1; A[e2] = ae2; A[e3] = ae3; A[e4] = ae4; A[e5] = ae5;

	// Fix end points
	if ( compareXY(ae1, A[N]) < 0 ) iswap(N, e1, A);
	if ( compareXY(A[M], ae5) < 0 ) iswap(M, e5, A);

	register void *T = ae3; // pivot

	// check Left label invariant
	// if ( T <= A[N] || A[M] < T ) {
	if ( compareXY(T, A[N]) <= 0 || compareXY(A[M], T) < 0 ) {
	   // give up because cannot find a good pivot
	   // dflgm is a dutch flag type of algorithm
	   // void quicksort0c();
	   dflgm(A, N, M, e3, quicksort0pc, depthLimit, compareXY);
	   return;
	 }

        register int I, J; // indices
	register void *AI, *AJ; // array values


	// initialize running indices
	I= N;
	J= M;

	// The left segment has elements < T
	// The right segment has elements >= T
  Left:
	// I = I + 1;
	// AI = A[I];
	// if (AI < T) goto Left;
	// if ( compareXY(AI,  T) < 0 ) goto Left;
	while ( compareXY(A[++I],  T) < 0 ); 
	AI = A[I];
// Right:
	// J = J - 1;
	// AJ = A[J];
	// if ( T <= AJ ) goto Right;
	// if ( compareXY(T, AJ) <= 0 ) goto Right;
	while ( compareXY(T, A[--J]) <= 0 ); 
	AJ = A[J];
	if ( I < J ) {
	  A[I] = AJ; A[J] = AI;
	  goto Left;
	}
	// Tail iteration
  	if ( (I - N) < (M - J) ) { // smallest one first
	  // quicksort0Pc(N, J, depthLimit);
	  // N = I; 
	  addTaskSynchronized0(llx, newTask(A, I, M, depthLimit, compareXY));
	  M = J;
	  goto Loop;
	}
	// quicksort0Pc(I, M, depthLimit);
	// M = J;
	addTaskSynchronized0(llx, newTask(A, N, J, depthLimit, compareXY));
	N = I;
	goto Loop;
} // (*  OF quicksort0p; *) ... the brackets remind that this was Pascal code


// Bentley file content generators
void reverse();
void reverseFront();
void reverseBack();
void tweakSort();
void dither();
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
} // end reverseBAck

void **A;
int (*compareXY)();
void tweakSort(void **AA, int n) {
  /*
  A = AA;
  compareXY = compareIntVal;
  quicksort0(0, n-1);
  */
  callQuicksort0(AA, n, compareIntVal);
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


void callHeapsort(void **A, int size, 
	 int (*compar ) (const void *, const void * ) ) {
  heapSort(A, size, compar);
} // end callHeapsort

void validateParSixSortBT() {
  printf("Entering validateParSixSortBT Sawtooth ........\n");
  // printf("Entering validateParSixSortBT Rand2 ........\n");
  // printf("Entering validateParSixSortBT Plateau ........\n");
  // printf("Entering validateParSixSortBT Shuffle ........\n");
  // printf("Entering validateParSixSortBT Stagger ........\n");
  int sortcBTime, quicksort0Time, T;
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
    int sortcBCnt, quicksort0Cnt; // , sortcBCntx, quicksort0Cntx;
    int sumQsortB, sumQuicksort0; // , sumQsortBx, sumQuicksort0x;
    // for (z = 0; z < 3; z++) { // repeat to check stability
    for (z = 0; z < 1; z++) { // repeat to check stability
      sortcBCnt = quicksort0Cnt = sumQsortB = sumQuicksort0 = 0;
      // sortcBCntx = quicksort0Cntx = sumQsortBx = sumQuicksort0x = 0;
      for (m = 1; m < 2 * siz; m = m * 2) {
      // m = 1024 * 1024; {
      // m = 1; {
      	for (tweak = 0; tweak <= 5; tweak++ ) {
	// tweak = 5; {
	  sortcBTime = 0; quicksort0Time = 0;
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
	    sixsort(B, siz, compareIntVal, NUMTHREADS);  
	  }
	  quicksort0Time = quicksort0Time + clock() - T - TFill;
	  sumQuicksort0 += quicksort0Time;
	  // if ( 4 != tweak ) sumQuicksort0x += quicksort0Time;
	  printf("Size: %d m: %d tweak: %d ", siz, m, tweak);
	  printf("sortcBTime: %d ", sortcBTime);
	  printf("Quicksort0Time: %d ", quicksort0Time);
	  frac = 0;
	  if ( sortcBTime != 0 ) frac = quicksort0Time / ( 1.0 * sortcBTime );
	  printf("frac: %f \n", frac);
	  if ( sortcBTime < quicksort0Time ) sortcBCnt++;
	  else quicksort0Cnt++;
	  for (i = 0; i < siz; i++) {
	    if ( compareIntVal(A[i], B[i]) != 0 ) {
	      printf("***** validateParSixSortBT m: %i tweak: %i at i: %i\n", 
		     m, tweak, i);
	      exit(0);
	    }
	  }
	}
	printf("sumQsortB:   %i sumQuicksort0:  %i frac: %f", 
	       sumQsortB, sumQuicksort0, (sumQuicksort0/(1.0 * sumQsortB)));
	printf(" sortcBCnt:  %i quicksort0Cnt:  %i\n", sortcBCnt, quicksort0Cnt);
      }
      frac = 0;
      if ( sumQsortB != 0 ) frac = sumQuicksort0 / ( 1.0 * sumQsortB );
      printf("Measurements:\n");
      printf("sumQsortB:   %i sumQuicksort0:  %i frac: %f", 
	     sumQsortB, sumQuicksort0, (sumQuicksort0/(1.0 * sumQsortB)));
      printf(" sortcBCnt:  %i quicksort0Cnt:  %i\n", sortcBCnt, quicksort0Cnt);
      // printf("sumQsortBx:  %i sumQuicksort0x: %i", sumQsortBx, sumQuicksort0x);
      // printf(" sortcBCntx: %i quicksort0Cntx: %i\n", sortcBCntx, quicksort0Cntx);

    }
    // free array
    for (i = 0; i < siz; i++) {
      free(A[i]); free(B[i]);
    };
    free(A); free(B);
    siz = siz * 2;
    seedLimit = seedLimit / 2;
  }
} // end validateParSixSortBT
