/*
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

// File: c:/bsd/rigel/sort/ParSixSort.c
// Date: Thu Jan 07 15:08:48 2010, 2015
/* This file has the source of the algorithms that make up ParSixSort
   headed by sixsort
*/
/* compile with: 
   gcc -c ParSixSort.c
   which produces:
   ParSixSort.o
   Compile & link with UseParSixSort:
   gcc UseParSixSort.c ParSixSort.o
*/
// To adjust restrictions: go to sixsort(...) at the end of this file 


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>


#define errexit(code,str)                          \
  fprintf(stderr,"%s: %s\n",(str),strerror(code)); \
  exit(1);

char* expiration = "*** License for sixsort has expired ...\n";

// Here more global entities used throughout
// int (*compareXY)();
// void **A;
int sleepingThreads = 0;
int NUMTHREADS;

struct stack *ll;
struct task *newTask();

void addTaskSynchronized();

// /*
#include "Hsort.c"
#include "Dsort.c"
#include "Isort.c"
// */
// #include "Qusort.c"

#include "D3sort.c" 
#include "C2sort.c" 

// #include "D4.c"
#include "Qstack.c"
#include "C4p.c"

void *myMallocSS(char* location, int size) {
  void *p = malloc(size);
  if ( 0 == p ) {
    fprintf(stderr, "malloc fails for: %s\n", location);
    // printf("Cannot allocate memory in: %s\n", location);
    exit(1);
  }
  return p;
} // end of myMalloc

pthread_mutex_t condition_mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond2  = PTHREAD_COND_INITIALIZER;

void addTaskSynchronized(struct stack *ll, struct task *t) {
  /*
  printf("Thread#: %ld ", pthread_self());
  printf("addTask N: %d M: %d sleepingThreads: %d\n", 
	 getN(t), getM(t), sleepingThreads);
  */
  pthread_mutex_lock( &condition_mutex2 );
  push(ll, t);
  pthread_cond_signal( &condition_cond2 );
  pthread_mutex_unlock( &condition_mutex2 );
} // end addTaskSynchronized


void *sortThread(void *AAA) { // AAA-argument is NOT used
  // int taskCnt = 0;
  //  printf("Thread number: %ld #sleepers %d\n", 
  //         pthread_self(), sleepingThreads);
  struct task *t = NULL;
  for (;;) {
    pthread_mutex_lock( &condition_mutex2 );
       sleepingThreads++;
       while ( NULL == ( t = pop(ll) ) && 
	       sleepingThreads < NUMTHREADS ) {
	 pthread_cond_wait( &condition_cond2, &condition_mutex2 );
       }
       if ( NULL != t ) sleepingThreads--;
    pthread_mutex_unlock( &condition_mutex2 );
    if ( NULL == t ) { 
      pthread_mutex_lock( &condition_mutex2 );
      pthread_cond_signal( &condition_cond2 );
      pthread_mutex_unlock( &condition_mutex2 );
      break;
    }
    void **A = getA(t);
    int n = getN(t);
    int m = getM(t);
    int depthLimit = getDL(t);
    int (*compare)() = getXY(t);
    free(t);
    // taskCnt++;
    cut4Pc(A, n, m, depthLimit, compare);
  }

  //  printf("Exit of Thread number: %ld taskCnt: %d\n", pthread_self(), taskCnt);
  return NULL;
} // end sortThread

int partitionLeft(void **A, int N, int M, int (*compareXY)()) { 
  /*
    |------------------------|
    N                        M 
    A[N] < T             A[M] = T 
   */
  int i = N; int j = M; void *T = A[M];
 again:
  while ( compareXY(A[++i], T) < 0 ); // T <= A[i]
  while ( compareXY(T, A[--j]) <= 0 ); // A[j] < T
  if ( i < j ) {
      iswap(i, j, A);
      goto again;
  }
  return j;
} // end partitionLeft

void *partitionThreadLeft(void *ptr) {
  struct task *tx = ( struct task * ) ptr;
  void **A = getA(tx);
  int n = getN(tx);
  int m = getM(tx);
  // int T = getDL(tx);
  int (*compare)() = getXY(tx);
  int ix = partitionLeft(A, n, m, compare);
  setN(tx, ix);
} // end partitionThreadLeft

int partitionRight(void **A, int N, int M, int (*compareXY)()) { 
  /*
    |------------------------|
    N                        M 
    A[N] = T             T <= A[M] 
    Do NOT change A[N]; do allow that no elements < T
   */
  int i = N; int j = M; void *T = A[N];
  while ( compareXY(A[++i], T) < 0 ); // T <= A[i]
  while ( i <= j && compareXY(T, A[--j]) <= 0 ); // j<i or A[j] < T
  if ( j < i ) { // j = N -> no elements < T
    return j;
  }
  // i < j
  iswap(i, j, A);
  // both sides not empty
 again:
  while ( compareXY(A[++i], T) < 0 ); // T <= A[i]
  while ( compareXY(T, A[--j]) <= 0 ); // A[j] < T
  if ( i < j ) {
      iswap(i, j, A);
      goto again;
  }
  return j;
} // end partitionRight

void *partitionThreadRight(void *ptr) {
  struct task *tx = ( struct task * ) ptr;
  void **A = getA(tx);
  int n = getN(tx);
  int m = getM(tx);
  // int T = getDL(tx);
  int (*compare)() = getXY(tx);
  int ix = partitionRight(A, n, m, compare);
  setN(tx, ix);
} // end partitionThreadRight

int cut2SLimit = 2000;
void sixsort(void **A, int size, 
	int (*compareXY ) (const void *, const void * ),
	int numberOfThreads) {
  /*
  // Set host & licence expiration date
  char* myHost = "ddc2";
  int year = 2010;
  int month = 11; // December = 11
  int day = 31;
  // show allowed host and licence expiration date
  if ( 0 == AA ) {
    printf("Licence expires: %d / %d / %d\n", year, month+1, day);
    printf("Host: %s\n", myHost);
    exit(0);
  }
  // Check whether licence expired
  time_t now = time(NULL);
  struct tm *laterx = localtime(&now);
  laterx->tm_year = year-1900; // = 2010
  laterx->tm_mon = month;
  laterx->tm_mday = day;
  time_t endTime = mktime(laterx);
  if ( endTime < now ) {
     printf(expiration);
     exit(1);
  }
  // Check whether this host is allowed
  int nameLng = 100;
  char hostName[nameLng];
  int out = gethostname(hostName, nameLng);
  if ( 0 != out ) {
    printf("*** Error: cannot get: Host name\n");
    exit(1);
  }
  out = strcmp(myHost, hostName);
  if ( 0 != out ) {
    printf("*** Error: not allowed host\n");
    exit(1);
  }
  */
  // Proceed !
  // A = AA;
  // compareXY = compar;

  if ( size <= cut2SLimit || numberOfThreads <= 1 ) {
    cut2(A, 0, size-1, compareXY);
    return;
  }
  // printf("sixsort size %d\n", size);
  sleepingThreads = 0;
  NUMTHREADS = numberOfThreads;
  pthread_t thread_id[NUMTHREADS];
  ll = newStack();
  int depthLimit = 2.5 * floor(log(size));
  // Try doing the first partition in parallel with two threads
  int N = 0; int M = size-1; 
  int L = M-N;

  // Check for duplicates
  int sixth = (M - N + 1) / 6;
  int e1 = N  + sixth;
  int e5 = M - sixth;
  int e3 = N + L/2; // The midpoint
  // int e3 = (N+M) / 2; // The midpoint
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
    struct task *t = newTask(A, 0, size-1, depthLimit, compareXY);
    addTaskSynchronized(ll, t);
  } else {
    /*
      |------------------------|------------------------|
      N                        e3                       M
      A[N] < T             A[e3] = T                 T<=A[M]
    */
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
    /*      LL            LR           RL           RR
       |--------------]----------][-----------]-----------------|
       N     <        i1   >=    e3    <      i2      >=        M
       i2 = e3 -> RL is empty
    */
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
    /*
      |------------------------][----------------------------|
      N           <           m3           >=                M
     */
    // printf("pss m3 %i\n", m3);
    t1 = newTask(A, N, m3, depthLimit, compareXY);
    addTaskSynchronized(ll, t1);
    t1 = newTask(A, m3+1, M, depthLimit, compareXY);
    addTaskSynchronized(ll, t1);
  }
  // printf("Entering sortArray\n");
  int i;
  int errcode;
  for ( i = 0; i < NUMTHREADS; i++ ) {
    if ( errcode=pthread_create(&thread_id[i], NULL, 
				sortThread, (void*) A) ) {
      errexit(errcode,"ParSort/ sortArray()/pthread_create");
    }
  }

  for ( i = 0; i < NUMTHREADS; i++ ) {
    if ( errcode=pthread_join(thread_id[i], NULL) ) {
      errexit(errcode,"ParSort/ sortArray()/pthread_join");
    }
  }
  free(ll);
  // printf("Exiting sortArray\n");

} // end sixsort
/* compile with: 
   gcc -c ParSixSort.c
   which produces:
   ParSixSort.o
   Compile & link with UseParSixSort:
   gcc UseParSixSort.c ParSixSort.o
*/
