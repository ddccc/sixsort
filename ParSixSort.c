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
// Date: Thu Jan 07 15:08:48 2010
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

const int cut2Limit = 127;
const int cut3Limit = 250;
const int cut4Limit = 375; 
const int probeParamCut4 = 100000;

// Here more global entities used throughout
int (*compareXY)();
void **A;
int sleepingThreads = 0;
int NUMTHREADS;


char* expiration = "*** License for sixsort has expired ...\n";

void iswap(int p, int q, void **A) {
  void *t = A[p];
  A[p] = A[q];
  A[q] = t;
} // end of iswap

// calculate the median of 3
int med(void **A, int a, int b, int c,
	int (*compar ) (const void *, const void * ) ) {
  return
    compar( A[a], A[b] ) < 0 ?
    ( compar( A[b], A[c] ) < 0 ? b : compar( A[a], A[c] ) < 0 ? c : a)
    : compar( A[b], A[c] ) > 0 ? b : compar( A[a], A[c] ) > 0 ? c : a;
} // end med

// insertionsort for small segments where the loop has been unrolled
void insertionsort(int N, int M) {
  if ( M <= N ) return;
  // M <= N + 10 
  register int i, s;
  // put minimum element at index N
  int minimumPosition;
  void *minimum, *next, *ai;
  minimumPosition = N;
  minimum = A[N];
  for ( i = N+1; i <= M; i++ ) {
    ai = A[i];
    // if ( ai < minimum ) {
    if ( compareXY(ai, minimum) < 0 ) {
      minimum = ai;
      minimumPosition = i;
    }
  }
  if ( N != minimumPosition ) {
    A[minimumPosition] = A[N];
    A[N] = minimum;
  }
  // now push elements back to their proper location
  s = N+1;
  // if ( M == s ) return;
  while ( s < M ) {
    s=s+1;
    next=A[s];
    ai = A[s-1];
    // if ( ai <= next ) continue;
    if ( compareXY(ai, next) <= 0 ) continue;
    A[s] = ai;
    ai = A[s-2];
    if ( compareXY(ai, next) <= 0 ) { A[s-1] = next; continue; }
    A[s-1] = ai;
    ai = A[s-3];
    if ( compareXY(ai, next) <= 0 ) { A[s-2] = next; continue; }
    A[s-2] = ai;
    ai = A[s-4];
    if ( compareXY(ai, next) <= 0 ) { A[s-3] = next; continue; }
    A[s-3] = ai;
    ai = A[s-5];
    if ( compareXY(ai, next) <= 0 ) { A[s-4] = next; continue; }
    A[s-4] = ai;
    ai = A[s-6];
    if ( compareXY(ai, next) <= 0 ) { A[s-5] = next; continue; }
    A[s-5] = ai;
    ai = A[s-7];
    if ( compareXY(ai, next) <= 0 ) { A[s-6] = next; continue; }
    A[s-6] = ai;
    ai = A[s-8];
    if ( compareXY(ai, next) <= 0 ) { A[s-7] = next; continue; }
    A[s-7] = ai;
    ai = A[s-9];
    if ( compareXY(ai, next) <= 0 ) { A[s-8] = next; continue; }
    A[s-8] = ai;
    ai = A[s-10];
    if ( compareXY(ai, next) <= 0 ) { A[s-9] = next; continue; }
    A[s-9] = ai;
    ai = A[s-11];
    if ( compareXY(ai, next) <= 0 ) { A[s-10] = next; continue; }
    A[s-10] = ai;
    ai = A[s-12];
    if ( compareXY(ai, next) <= 0 ) { A[s-11] = next; continue; }
    fprintf(stderr, "ParSixSort/ insertionsort() ");
    fprintf(stderr, "N: %d M %d s: s\n", N, M, s);
    exit(1);
  }
  return;
}

void quicksort0c();
// Quicksort function for invoking quicksort0c.
void quicksort0(int N, int M) {
  int L = M - N;
  if ( L <= 0 ) return;
  // printf("quicksort0 %d %d \n", N, M);
  if ( L <= 10 ) { 
    insertionsort(N, M);
    return;
  }
  int depthLimit = 2 * floor(log(L));
  quicksort0c(N, M, depthLimit);
} // end quicksort0

void heapc();
// Quicksort equipped with a defense against quadratic explosion;
// calling heapsort if depthlimit exhausted
void quicksort0c(int N, int M, int depthLimit) {
  // printf("Enter quicksort0c N: %d M: %d %d\n", N, M, depthLimit);
  while ( N < M ) {
    // printf("Enter quicksort0c N: %d M: %d %d\n", N, M, depthLimit);
    int L = M - N;
    if ( L <= 10 ) {
      insertionsort(N, M);
      return;
    }
    if ( depthLimit <= 0 ) {
      heapc(A, N, M);
      return;
    }
    depthLimit--;
    // 10 < L
    // grab median of 3 or 9 to get a good pivot
    int pn = N;
    int pm = M;
    int p0 = (pn+pm)/2;
    if ( 40 < L ) { // do median of 9
      int d = L/8;
      pn = med(A, pn, pn + d, pn + 2 * d, compareXY);
      p0 = med(A, p0 - d, p0, p0 + d, compareXY);
      pm = med(A, pm - 2 * d, pm - d, pm, compareXY);
      /* Activation of the check for duplicates gives a slow down of 
	 1/4% on uniform input.  If you suspect that duplicates
	 causing quadratic deterioration are not caught higher-up by cut3
	 you may want to experiment with this check::::
      if ( L < 100 ) { // check for duplicates
	int duplicate = -1;
	if ( compareXY(A[pn], A[pm]) == 0 ) { duplicate = pn; } else
	if ( compareXY(A[pn], A[p0]) == 0 ) { duplicate = p0; } else
	if ( compareXY(A[pm], A[p0]) == 0 ) { duplicate = p0; };
	if ( 0 <= duplicate ) {
	  void quicksort0();
	  dflgm(N, M, duplicate, quicksort0, depthLimit);
	  return;
	}
      }
      */
    }
    p0 = med(A, pn, p0, pm, compareXY); // p0 is index to 'best' pivot ...
    iswap(N, p0, A); // ... and is put in first position as required by quicksort0c

    register void *p = A[N]; // pivot
    register int i, j;
    i = N;
    j = M;
    register void *ai; void *aj;

    /* Split array A[N,M], N<M in two segments using pivot p; 
       construct a partition with A[N,i), A(i,M] and N <= i <= M, and
       N <= k <= i -> A[k] <= p  and  i < k <= M -> p < A[k];
       Allow the worse cases: N=i or i=M.
       Recurse on A[N,i) and A(i,M) (or in the reverse order).
       This code does NOT do swapping; instead it disposes 
       ai/aj in a hole created by setting aj/ai first.  
    */
    /* Start state:
	  |-------------------------------|
          N=i                           j=M
	  N = i < j = M
          N <= k < i -> A[k] <= p    
          N < j < k <= M -> p < A[k]
	  A[N] = p
          N < i -> p < A[i]
    */
    while ( i < j ) {
      /*
	  |-------o---------------[--------|
          N       i               j        M
	  N <= i < j <= M
          N <= k < i -> A[k] <= p    
          N < j < k <= M -> p < A[k]
	  A[N] <= p
          N < i -> p < A[i]
          p + A[N,i) + A(i,M] is a permutation of the input array
      */
      aj = A[j];
      while ( compareXY(p, aj) < 0 ) { 
        /*
	  |-------o---------------[--------|
          N       i               j        M
	  N = i < j <= M or N < i <= j <= M
          N <= k < i -> A[k] <= p    
          N < j <= k <= M -> p < A[k]
	  A[N] <= p
	  N < i -> p < A[i}
	  p + A[N,i) + A(i,M] is a permutation of the input array
          p < aj = A[j]
	*/
	j--; 
	aj = A[j]; 
      }
      /*
	  |-------o---------------[--------|
          N       i               j        M
	  N = i = j < M or N < i & = i-1 <= j <= M
          N <= k < i -> A[k] <= p    
          j < k <= M -> p < A[k]
	  A[N] <= p
	  N < i -> p < A[i}
	  p + A[N,i) + A(i,M] is a permutation of the input array
          aj = A[j] <= p
	*/
      if ( j <= i ) {
	/*
	  |-------o-----------------------|
          N       i                       M
	  N = i = j < M or N < i & = i-1 = j < M
          N <= k < i -> A[k] <= p    
          i < k <= M -> p < A[k]
	  A[N] <= p
	  p + A[N,i) + A(i,M] is a permutation of the input array
      */
	break;
      }
      // i < j 
      A[i] = aj; // fill hole !
      /*
	  |-------]---------------o--------|
          N       i               j        M
	  N <= i < j <= M
          N <= k <= i -> A[k] <= p    
          j < k <= M -> p < A[k]
	  A[N] <= p
	  p + A[N,j) + A(j,M] is a permutation of the input array
	  aj = A[j] <= p
      */
      i++; ai = A[i];
      while ( i < j && compareXY(ai, p) <= 0 ) {
      /*
	  |-------]---------------o--------|
          N       i               j        M
	  N < i < j <= M
          N <= k <= i -> A[k] <= p    
          j < k <= M -> p < A[k]
	  A[N] <= p
	  p + A[N,j) + A(j,M] is a permutation of the input array
	  aj = A[j] <= p
      */
	i++; ai = A[i]; 
      }
      if ( j <= i ) 
      /*
	  |----------------------o--------|
          N                     i=j       M
	  N < i = j <= M
          N <= k < i -> A[k] <= p    
          j < k <= M -> p < A[k]
	  A[N] <= p
	  p + A[N,j) + A(j,M] is a permutation of the input array
      */
	break;
      // i < j  & p < ai = A[i] 
      A[j] = ai;
      j--;
      /*
	  |--------o--------------[--------|
          N        i              j        M
	  N < i <= j <= M
          N <= k < i -> A[k] <= p    
          j < k <= M -> p < A[k]
	  A[N] <= p
          N < i -> p < ai = A[i]
	  p + A[N,i) + A(i,M] is a permutation of the input array
      */
    }
    A[i] = p;
    /*
	  |--------]----------------------|
          N        i                      M
	  N <= i <= M
          N <= k <= i -> A[k] <= p    
          i < k <= M -> p < A[k]
	  A[N] <= p
	  A[N,i] + A(i,M] is a permutation of the input array
    */
    // Recurse on the smallest one and iterate on the other one
    int ia = i-1; int ib = i+1; 
    if ( i-N < M-i ) { 
      if ( N < ia ) quicksort0c(N, ia, depthLimit);  
      N = ib; 
    } else { 
      if ( ib < M ) quicksort0c(ib, M, depthLimit);  
      M = ia; 
    }
  }
} // end of quicksort0c


// Dutch flag type function that initializes the middle segment in the middle
void dflgm(int N, int M, int pivotx, void (*cut)(), int depthLimit) {
  // printf("dflgm N %i M %i pivotx %i dl %i\n", N,M,pivotx,depthLimit);  
  /*
    Simple version of partitioning with: L/M/R
    L < pivot, M = pivot, R > pivot
   */
  register int i, j, lw, up; // indices
  register void* p3; // pivot
  register void* ai, *aj, *am;
  register int r; // comparison output 
  // int z; // for tracing

  /*
  if ( depthLimit <= 0 ) { // avoid quadradic explosion
    heapc(A, N, M);
    return;
  }
  int L = M - N;
  if ( L < 100 ) {
    // delegated small segments
    quicksort0c(N, M, depthLimit);
    return;
  }
  */

  i = N; j = M; lw = pivotx-1; up = pivotx+1;
    /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      
      N <= i < lw <= up < j <= M
      N <= x < i ==> A[x] < p3
      lw < x < up  ==> A[x] = p3
      j < x <= M & ==> p3 < A[x] 
    */
  p3 = A[pivotx]; // There IS a middle value somewhere:

 L0:
   /*
     |---)-----(----)-------(----|
     N   i     lw  up       j    M
   */

  // while ( A[i] < p3 ) i++;
  while ( (r = compareXY(A[i], p3)) < 0 ) i++;
  if ( lw < i ) {
    i--;
    if ( N < i ) goto leftClosed;
    i = N;
    goto leftEmpty;
  }
  ai = A[i]; // p3 <= ai
  // if ( p3 < ai ) goto LaiR; 
  if ( 0 < r ) goto LaiR; 
  goto LaiM;

 LaiR:
   /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      ai -> R
   */
  // while ( p3 < A[j] ) j--;
  while ( (r = compareXY(p3, A[j])) < 0 ) j--;
  aj = A[j]; // aj <= p3
  // if ( aj < p3 ) { // aj -> L
  if ( 0 < r ) {
    A[i] = aj; A[j] = ai; 
    goto L1; }
  // aj -> M
  if ( j <= up ) {
    if ( j == M ) goto emptyRightaiR;
    j++; goto rightClosedaiR;
  }
  // up < j
  am = A[up];
  // if ( am < p3 ) { // am -> L
  if ( (r = compareXY(am, p3)) < 0 ) {
    A[i] = am; A[up++] = aj; A[j] = ai;
    goto L1; 
  }
  // if ( p3 < am ) { // am -> R
  if ( 0 < r ) {
    A[up++] = aj; A[j--] = am; 
    if ( j < up ) { j++; goto rightClosedaiR; }
    // up <= j
    goto LaiR; // info loss ...
  }
  // am -> M
  up++;
  goto LaiR;

 LaiM:
   /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      ai -> M
   */
  // while ( N < lw && A[lw] == p3 ) lw--;
  while ( N < lw && (r = compareXY(A[lw], p3)) == 0 ) lw--;
  if ( N == lw ) { goto leftEmpty; }
  // N < lw
  if ( i == lw ) { i--; goto leftClosed; }
  // i < lw
  am = A[lw];
  // if ( am < p3 ) { // am -> L
  if ( r < 0 ) {
    A[i++] = am; A[lw--] = ai;
    if ( lw < i ) { i--; goto leftClosed; }
    goto L0; // info loss
  }
  // am -> R
  // while ( p3 < A[j] ) j--;
  while ( (r = compareXY(p3, A[j])) < 0 ) j--;
  aj = A[j]; // aj <= p3
  // if ( aj < p3 ) { // aj -> L
  if ( 0 < r ) {
    A[i] = aj; A[lw--] = ai; A[j] = am;
    goto L1;
  }
  // aj -> M
  if ( j < up ) {
    A[lw--] = aj; A[j] = am;
    goto rightClosedaiM;
  }
  // up <= j
  A[lw--] = aj; A[j--] = am;
  if ( j < up ) {
    j++; // right side closed
    if ( i == lw ) { // left side closed
      i--; goto done;
    }
    goto rightClosedaiM;
  }
  // up <= j
  if ( i == lw ) {
    i--; goto leftClosed;
  }
  goto LaiM; // info loss
  
 leftEmpty:
   /*
      |-------------)-------(----|
    N=i             up      j    M
   */
  // while ( up <= M && A[up] == p3 ) up++;
  while ( up <= M && (r = compareXY(A[up], p3)) == 0 ) up++;
  if ( M < up ) return; // !!
  if ( j < up ) { j++; goto done; }
  // up <= j
  am = A[up];
  // if ( am < p3) { // am -> L
  if ( r < 0 ) {
    A[up++] = A[N]; A[N] = am; 
    if ( j < up ) { j++; goto done; }
    // up <= j
    goto leftClosed;
  }
  // am -> R
  // while ( p3 < A[j] ) j--;
  while ( (r = compareXY(p3, A[j])) < 0 ) j--;
  if ( j < up ) { j++; goto done; }
  // up < j
  aj = A[j]; // aj <= p3
  // if ( aj < p3 ) { // aj -> L
  if ( 0 < r ) {
    A[j--] = am; A[up++] = A[N]; A[N] = aj;
    if ( j < up ) { j++; goto done; }
    goto leftClosed;
  }
  // aj -> M
  A[j--] = am; A[up++] = aj;
  if ( j < up ) { j++; goto done; }
  goto leftEmpty;
  
 emptyRightaiR:
   /*
      |---)-----(---------------|
      N   i     lw              j=M
      ai -> R
   */
  // while ( A[lw] == p3 ) lw--;
  while ( (r = compareXY(A[lw], p3)) == 0 ) lw--;
  am = A[lw];
  // if ( p3 < am ) { // am -> R
  if ( 0 < r ) {
    if ( i == lw ) {
      A[i] = A[M]; A[M] = ai; i--; goto done;
    }
    // i < lw
    A[lw--] = A[M]; A[M] = am; goto rightClosedaiR;
  }
  // am -> L
  A[i++] = am; A[lw--] = A[M]; A[M] = ai;
  if ( lw < i ) { i--; goto done; }
  goto rightClosed;

  L1: 
  /*
    |---]-----(----)-------[----|
    N   i     lw  up       j    M
  */
  // while ( A[++i] < p3 );
  while ( (r = compareXY(A[++i], p3)) < 0 );
  if ( lw < i ) { i--; j--; goto leftClosed; }
  // i <= lw
  ai = A[i];
  // if ( p3 < ai ) { // ai -> R
  if ( 0 < r ) {
  L1Repeat:
    // while ( p3 < A[--j] );
    while ( (r = compareXY(p3, A[--j])) < 0 );
    if ( j < up ) { j++; goto rightClosedaiR; }
    // up <= j
    aj = A[j];
    // if ( aj < p3 ) { // aj -> L
    if ( 0 < r ) {
      A[i] = aj; A[j] = ai; 
      goto L1;
    }
    // aj -> M
    // while ( A[up] == p3 ) up++;
    while ( (r = compareXY(A[up], p3)) == 0 ) up++;
    if ( j < up ) { j++; goto rightClosedaiR; }
    // up <= j
    am = A[up];
    // if ( am < p3 ) { // am -> L
    if ( r < 0 ) {
      A[i] = am; A[up++] = aj; A[j] = ai;
      if ( j < up ) { i++; goto rightClosed; }
      goto L1;
    }
    // am -> R
    A[up++] = aj; A[j] = am;
    if ( j < up ) { goto rightClosedaiR; }
    // up <= j
    goto L1Repeat;
  }
  // ai -> M
 L1Repeat2:
  // while ( A[lw] == p3 ) lw--;
  while ( (r = compareXY(A[lw], p3)) == 0 ) lw--;
  if ( lw < i ) { i--; j--; goto leftClosed; }
  // i < lw
  am = A[lw]; 
  // if ( am < p3 ) { // am -> L
  if ( r < 0 ) {
    A[i] = am; A[lw--] = ai;
    if ( lw < i ) { i--; j--; goto leftClosed; } 
    goto L1;
  }
  // am -> R
  // while ( p3 < A[--j] );
  while ( (r = compareXY(p3, A[--j])) < 0 );
  if ( j < up ) {
    A[lw--] = A[j]; A[j] = am; goto rightClosedaiM;
  }
  // up <= j
  aj = A[j];
  // if ( aj < p3 ) { // aj -> L
  if  ( 0 < r ) {
    A[i] = aj; A[lw--] = ai; A[j] = am;
    if ( lw < i ) { j--; goto leftClosed; }
    goto L1;
  }
  // aj -> M
  A[lw--] = aj; A[j] = am; 
  goto L1Repeat2;

 leftClosed: 
   /* 
      |---]----------)-------(----|
      N   i          up      j    M
   */
  // while ( p3 < A[j] ) j--;
  while ( (r = compareXY(p3, A[j])) < 0 ) j--;
  if ( j < up ) { j++; goto done; }
  // up <= j
  aj = A[j]; // aj <= p3
  // if ( aj < p3 ) { // aj -> L
  if ( 0 < r ) {
    repeatM:
    // while ( A[up] == p3 ) up++;
    while ( (r = compareXY(A[up], p3)) == 0 ) up++;
    am = A[up];
    // if ( p3 < am ) { // am -> R
    if ( 0 < r ) {
      A[j--] = am; A[up++] = A[++i]; A[i] = aj;
      if ( j < up ) { j++; goto done; }
      goto leftClosed;
    }
    // am -> L
    if ( up == j ) { A[j++] = A[++i]; A[i] = am; goto done; }  
    // up < j
    A[up++] = A[++i]; A[i] = am;
    goto repeatM;
  }
  // aj -> M
  repeatM2:
  // while ( up <= j && A[up] == p3 ) up++;
  while ( up <= j && (r = compareXY(A[up], p3)) == 0 ) up++;
  if ( j <= up ) { j++; goto done; }
  // up < j
  am = A[up];
  // if ( p3 < am ) { // am -> R
  if ( 0 < r ) {
    A[j--] = am; A[up++] = aj;
    if ( j < up ) { j++; goto done; }
    goto leftClosed;
  }
  // am -> L
  A[up++] = A[++i]; A[i] = am; 
  goto repeatM2;

 rightClosed:
   /*
      |---)-----(-----------[----|
      N   i    lw           j    M
   */
  // while ( A[i] < p3 ) i++;
  while ( (r = compareXY(A[i], p3)) < 0 ) i++;
  if ( lw < i ) { i--; goto done; }
  // i <= lw
  ai = A[i]; // p3 <= ai
  // if ( p3 < ai ) { // ai -> R
  if ( 0 < r ) {
    goto rightClosedaiR;
  }
  // ai -> M
  // goto rightClosedaiM;

 rightClosedaiM:
  // while ( i <= lw && A[lw] == p3 ) lw--;
  while ( i <= lw && (r = compareXY(A[lw], p3)) == 0 ) lw--;
  if ( lw < i ) { i--; goto done; }
  // i <= lw
  am = A[lw];
  // if ( p3 < am ) { // am -> R
  if ( 0 < r ) {
    A[lw--] = A[--j]; A[j] = am; 
    if ( lw < i ) { i--; goto done; }
    goto rightClosedaiM;
  }
  // am -> L
  A[i++] = am; A[lw--] = ai;
  if ( lw < i ) { i--; goto done; }
  goto rightClosed;

 rightClosedaiR: 
   /*
      |---)-----(-----------[----|
      N   i    lw           j    M
      ai -> R
   */
  // while ( A[lw] == p3 ) lw--;
  while ( (r = compareXY(A[lw], p3)) == 0 ) lw--;
  if ( i == lw ) {
    A[i--] = A[--j]; A[j] = ai; goto done;
  }
  // i < lw
  am = A[lw];
  // if ( am < p3 ) { // am -> L
  if ( r < 0 ) {
    A[i++] = am; A[lw--] = A[--j]; A[j] = ai;
    if ( lw < i ) { i--; goto done; }
    // i <= lw
    goto rightClosed;
  }
  // am -> R
  A[lw--] = A[--j]; A[j] = am; goto rightClosedaiR;
  
 done: 
    /*
      |---]---------[---------|
      N   i         j         M
    */
  // printf("done dflgm N %i i %i j %i M %i\n", N,i,j,M);

    /*
      for ( z = N; z <= i; z++ )
	// if ( p3 <= A[z] ) {
        if ( compareXY(p3, A[z]) <= 0 ) {
	  printf("doneL z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      for ( z = i+1; z < j; z++ )
	// if ( p3 != A[z] ) {
	if ( compareXY(p3, A[z]) != 0 ) {
	  printf("doneM z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      for ( z = j; z <= M ; z++ )
	// if ( A[z] <= p3 ) {
	if ( compareXY(A[z], p3) <= 0 ) {
	  printf("doneR z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      */
    if ( i - N  < M - j ) {
      (*cut)(N, i, depthLimit);
      (*cut)(j, M, depthLimit);
      return;
    }
    (*cut)(j, M, depthLimit);
    (*cut)(N, i, depthLimit);
} // end dflgm

void cut2c();
// cut2 is a support function to call up the workhorse cut2c
void cut2(int N, int M) { 
  // printf("cut2 %d %d \n", N, M);
  int L = M - N;
  if ( L < cut2Limit ) { 
    quicksort0(N, M);
    return;
  }
  int depthLimit = 2 * floor(log(L));
  cut2c(N, M, depthLimit);
} // end cut2

// Cut2 does 2-partitioning with one pivot; cut2 is NOT used
// by SixSort and neither by FiveSort.  It is included for comparisons. 
// Cut2 invokes cut3duplicates when duplicates are found.
// cut2 is used as a best in class quicksort implementation 
// {with a defense against quadratic behavior due to duplicates}
void cut2c(int N, int M, int depthLimit) {
  int L;
  Start:
  if ( depthLimit <= 0 ) {
    heapc(A, N, M);
    return;
  }
  L = M - N;
  if ( L < cut2Limit ) { 
    quicksort0c(N, M, depthLimit);
    return;
  }
  depthLimit--;
  
  // Check for duplicates
        int sixth = (L + 1) / 6;
        int e1 = N  + sixth;
        int e5 = M - sixth;
        int e3 = (N+M) / 2; // The midpoint
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
	if ( compareXY(T, A[N]) <= 0 || compareXY(A[M], T) < 0) {
	   // give up because cannot find a good pivot
	   // dflgm is a dutch flag type of algorithm
	   void cut2c();
	   dflgm(N, M, e3, cut2c, depthLimit);
	   return;
	 }

	register  int I, J; // indices
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
	while ( compareXY(A[++I],  T) < 0 ); AI = A[I];
  Right:
	// J = J - 1;
	// AJ = A[J];
	// if ( T <= AJ ) goto Right;
	// if ( compareXY(T, AJ) <= 0 ) goto Right;
	while ( compareXY(T, A[--J]) <= 0 ); AJ = A[J];
	if ( I < J ) {
	  A[I] = AJ; A[J] = AI;
	  goto Left;
	}
	// Tail iteration
	if ( (I - N) < (M - J) ) { // smallest one first
	  cut2c(N, J, depthLimit);
	  N = I; 
	  goto Start;
	}
	cut2c(I, M, depthLimit);
	M = J;
	goto Start;
} // (*  OF cut2; *) ... the brackets reminds that this was Pascal code


struct stack *ll;
struct task *newTask();
void addTaskSynchronized();

void cut4Pc();
// cut4P is doing 4-partitioning using 3 pivots
void cut4P(int N, int M) {
  // printf("cut4P %d %d \n", N, M);
  int L = M - N; 
  // cut4Pc(N, M, 0); return; // for testing heapsort
  if ( L < cut4Limit ) {
    cut2(N, M); 
    return; 
  }
  int depthLimit = 2 * floor(log(L));
  cut4Pc(N, M, depthLimit);
} // end cut4P

void cut4Pc(int N, int M, int depthLimit) 
{
  // printf("cut4Pc %d %d  %d\n", N, M, depthLimit);
  if ( depthLimit <= 0 ) {
    heapc(A, N, M);
    return;
  }
  int L = M - N; 
  if ( L < cut4Limit ) {
    cut2c(N, M, depthLimit); 
    return; 
  }
  depthLimit--;

  register void *maxl, *middle, *minr; // pivots for left/ middle / right regions

  int probeLng = L/ probeParamCut4;
  if ( probeLng < 28 ) probeLng = 28; // quite short indeed
  int halfSegmentLng = probeLng/2;
  int N1 = N + L/2 - halfSegmentLng;
  int M1 = N1 + probeLng - 1;
  int quartSegmentLng = probeLng/4;
  int maxlx = N1 + quartSegmentLng;
  int middlex = N1 + halfSegmentLng;
  int minrx = M1 - quartSegmentLng;
  int offset = L/probeLng;  

  // assemble the mini array [N1, M1]  
  int k;
  for (k = 0; k < probeLng; k++) iswap(N1 + k, N + k * offset, A);
  // sort this mini array to obtain good pivots
  cut2(N1, M1);

  // fix the end points
  if ( compareXY(A[N1], A[N]) < 0 ) iswap(N1, N, A);
  if ( compareXY(A[M], A[M1]) < 0 ) iswap(M1, M, A);

  void *first = A[N1];
  maxl = A[maxlx];
  middle = A[middlex];
  minr = A[minrx];
  void *last = A[M1];

  /*
  // test here for duplicates and go to cut3duplicates when dups found
  int dupx = -1; // candidate index to dups
  if ( compareXY(first, last) == 0 ) dupx = N1; else
    // if ( middle == first || middle == last ||
    //     middle == maxl || middle == minr ) dupx = middlex; else
    // if ( first == maxl ) dupx = maxlx; else
    // if ( last == minr ) dupx = minrx;
  if ( compareXY(middle, first) == 0 ||
       compareXY(middle, last) == 0 ||
       compareXY(middle, maxl) == 0 ||
       compareXY(middle, minr) == 0 ) dupx = middlex; else
  if ( compareXY(maxl, first) == 0 ) dupx = maxlx; else
  if ( compareXY(minr, last) == 0 ) dupx = minrx;
  void cut4Pc();
  if ( 0 <= dupx ) { // there are duplicates 
    dflgm(N, M, dupx, cut4Pc, depthLimit);
    return;
  }
  */

 // check to play safe
  // if ( maxl < A[N] || A[M] < minr || middle <= maxl || minr <= middle ) 
  if ( compareXY(maxl, A[N]) < 0 ||
       compareXY(A[M], minr) < 0 ||
       compareXY(middle, maxl) <= 0 ||
       compareXY(minr, middle) <= 0 ) {
    dflgm(N, M, middlex, cut4Pc, depthLimit);
    return;
  }

  register int i, j, lw, up, z; // indices

  // Here the general layout:
   /*   L             ML         MR             R
    |-----]------+[---------]---------]+-----[-----|
    N     i      lw         z         up     j     M
   */

  
  /* There are invariants to be maintained (which are >essential< 
     for machine assisted correctness proofs):
     maxl < middle < minr
     If there are two gaps:
       N <= x <= i --> A[x] <= maxl
       lw < x <= z  --> maxl < A[x] <= middle
       z < x < up  --> middle < A[x] < minr
       j <= x <= M --> minr <= A[x]
     If the left gap has closed:
       N <= x < i --> A[x] <= maxl
       i <= x <= z --> maxl < A[x] <= middle
       z <  x < up  --> middle < A[x] < minr
       j <= x <= M --> minr <= A[x]
     If the right gap has closed:
       N <= x <= i --> A[x] <= maxl
       lw < x <= z  --> maxl < A[x] <= middle
       z < x <= j  --> middle < A[x] < minr
       j < x <= M --> minr <= A[x]
  */
  void *x, *y; // values
  int hole;

  /* We employ again whack-a-mole. We know in which partition element x 
     should be.  Find a close, undecided position where x should go.  
     Store its content in y.  Drop x (simplified).  Set x to y and repeat. 
   */
  // Ready to roll ...

  i = N; j = M; z = middlex; lw = z-1; up = z+1; hole = N;

   /*   L             ML         MR             R
    |-----]------+[---------]---------]+-----[-----|
    N     i      lw         z         up     j     M
    MR is the only segment that is empty
   */
  i = N; j = M; z = middlex; lw = z-1; up = z+1; hole = N;

   /*   L             ML         MR             R
    |-----]------+[---------]---------]+-----[-----|
    N     i      lw         z         up     j     M
    MR is the only segment that is empty
   */

  x = A[N]; A[N] = maxl;
  // if ( x <= middle ) {
  if ( compareXY(x, middle) <= 0 ) {
    // if ( x <= maxl ) goto TLgMLgRL;
    if ( compareXY(x, maxl) <= 0 ) goto TLgMLgRL;
    goto TLgMLgRML;
  }
  // middle < x
  // if ( x < minr ) goto TLgMLgRMR;
  if ( compareXY(x, minr) < 0 ) goto TLgMLgRMR;
    goto TLgMLgRR;

	// MR empty
 TLgMLgRL:
         /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> L             z
	 */
	i++;
	y = A[i];
	// if ( y <= middle ) { 
	if ( compareXY(y, middle) <= 0 ) {
	  // if ( y <= maxl ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    goto TLgMLgRL;
	  }
	  // y -> ML 
	  if ( i <= lw ) {
	    A[i] = x;
	    x = y;
	    goto TLgMLgRML;
	  }
         /*   L                            R
	   |o----][----------]+--------[-----|
	   N      i           up       j     M
           x -> L            z
	 */
	  goto TLMLgRL;
	} 
	// middle < y
	A[i] = x;
	x = y;
	if ( compareXY(minr, y) <= 0 ) { 
	  goto TLgMLgRR;
	}
	goto TLgMLgRMR;

	// MR empty
 TLgMLgRML:
         /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> ML            z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = x;
	      x = y;
	      lw--;
	      goto TLgMLgRL;
	    }
	    // left gap closed
	    i++;
         /*   L                           R
	   |o----][----------]+--------[-----|
	   N      i           up       j     M
           x -> ML           z
	 */
	    goto TLMLgRML;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLgRML;
	}
	// middle < y
	A[lw] = x;
	x = y;
	lw--;
	if ( compareXY(minr, y) <= 0 ) {
	  goto TLgMLgRR;
	}
	// y -> MR
	goto TLgMLgRMR;

	// MR is empty
 TLgMLgRMR:
         /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> MR            z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  A[up] = x;
	  x = y;
	  up++;
	  if ( compareXY(y, maxl) <= 0 ) {
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	   N     i         lw   up        j     M
           x -> L             z
	 */
	    goto TLgMLMRgRL;
	  }
	  // y -> ML
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	   N     i         lw   up        j     M
           x -> ML            z
	 */
	  goto TLgMLMRgRML;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) { 
	  up++;
	  goto TLgMLMRgRMR;
	}
	// y -> R
	if ( j <= up ) { 
	  j--; z = j;
	  // right gap closed, while MR is empty ...
         /*   L                       R
	   |o----]---------+[-][-------------|
	   N     i         lw j              M
           x -> MR            z
	 */
	  goto TLgMLRMR;
	}
	// up < j
	A[up] = x;
	x = y;
	up++;
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	   N     i         lw   up        j     M
           x -> R             z
	 */
	goto TLgMLMRgRR;

	// MR empty  z+1=up
 TLgMLgRR: 
         /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> R             z
	 */
	j--;
	y = A[j];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    A[j] = x;
	    x = y;
	    goto TLgMLgRL;
	  }
	  // y -> ML
	  if ( up <= j ) {
	    A[j] = x;
	    x = y;
	    goto TLgMLgRML;
	  }
	  // right gap closed, empty MR	  
	 /*   L                           R
	   |o----]---------+[-][--------------|
	   N     i        lw  j               M
           x -> R             z
	 */
	  goto TLgMLRR;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  goto TLgMLgRR;
	}
	// y -> MR
	A[j] = x;
	x = y;
         /*   L                           R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> MR            z
	 */
	goto TLgMLgRMR;

	// empty MR
 TLMLgRML: 
         /*   L                           R
	   |o----][----------]+--------[-----|
	   N      i           up       j     M
           x -> ML           z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    A[up] = A[i];
	    A[i] = y;
	    i++;
	    z = up; up++; 
	    goto TLMLgRML;
	  }
	  // y -> ML
	  z = up; up++; 
	  goto TLMLgRML;	  
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  A[up] = x;
	  x = y;
	  z = up; up++; 
         /*   L                           R
	   |o----][----------]+--------[-----|
	   N      i           up       j     M
           x -> MR           z
	 */
	  goto TLMLgRMR;
	}
	// y -> R
	if ( up < j ) {
	  A[up] = x;
	  x = y;
	  z = up; up++; 
	  goto TLMLgRR;
	}
	// right gap closed with only 3 segments !
         /*   L                           R
	   |o----][----------][-------------|
	   N      i           j             M
           x -> ML           z
	 */
	j--;
	i--;
	A[hole] = A[i];
	A[i] = x;
        /*   L                           R
	   |-----][----------][-------------|
	   N      i          j              M
           x -> ML           z
	 */
	goto Finish3;

 TLgMLMRgRL:
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	   N     i        lw     up       j     M
           x -> L             z
	 */
	i++;
	y = A[i];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) goto TLgMLMRgRL;
	  // y -> ML
	  if ( lw < i ) {
	 /*   L                              R
	   |o----][-----------|-]+--------[-----|
	   N      i           z  up       j     M
           x -> L             
	 */
	    goto TLMLMRgRL;
	  }
	  A[i] = x;
	  x = y;
	  goto TLgMLMRgRML;
	}
	// middle < y
	A[i] = x;
	x = y;
	if ( compareXY(minr, y) <= 0 ) { 
	  goto TLgMLMRgRR;
	}
	goto TLgMLMRgRMR;

 TLgMLMRgRML:
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	   N     i        lw     up       j     M
           x -> ML            z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = x;
	      x = y;
	      lw--;
	      goto TLgMLMRgRL;
	    }
	    // left gap closed
	    i++;
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
	   N      i              up       j     M
           x -> ML            z
	 */
	    goto TLMLMRgRML;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLMRgRML;
	}
	// middle < y
	A[lw] = x;
	x = y;
	lw--;
	if ( compareXY(minr, y) <= 0 ) { 
	  goto TLgMLMRgRR;
	}
	goto TLgMLMRgRMR;

 TLgMLMRgRR:
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	   N     i        lw     up       j     M
           x -> R             z
	 */
	j--;
	y = A[j];
	if ( compareXY(y, middle) <= 0 ) {
	  A[j] = x;
	  x = y;
	  if ( compareXY(y, maxl) <= 0 ) { 
	    goto TLgMLMRgRL;
	  }
	  goto TLgMLMRgRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  goto TLgMLMRgRR;
	}
	// y -> MR
	if ( up <= j ) {
	  A[j] = x;
	  x = y;
	  goto TLgMLMRgRMR;
	}
	// right gap closed
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	   N     i        lw          j      M
           x -> R             z
	 */
	goto TLgMLMRRR;

 TLgMLMRgRMR:
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	   N     i        lw     up       j     M
           x -> MR            z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  A[up] = x;
	  x = y;
	  up++;
	  if ( compareXY(y, maxl) <= 0 ) { 
	    goto TLgMLMRgRL;
	  }	  
	  goto TLgMLMRgRML;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLgMLMRgRMR;
	}
	// y -> R
	if ( up < j ) {
	  A[up] = x;
	  x = y;
	  up++;
	  goto TLgMLMRgRR;
	}
	// right gap closed
	j--;
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	   N     i        lw          j      M
           x -> MR            z
	 */
	goto TLgMLMRRMR;

	// right gap closed, empty MR
 TLgMLRMR: 
         /*   L                           R
	   |o----]---------+[-][-------------|
	   N     i        lw  j              M
           x -> MR            z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(maxl, y) < 0 ) {
	    lw--;
	    goto TLgMLRMR;
	  }
	  // y -> L
	  if ( i < lw ) {
	    A[lw] = A[j];
	    A[j] = x;
	    z--;
	    x = y;
	    lw--;
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	   N     i         lw         j      M
           x -> L             z
	 */
	    goto TLgMLMRRL;
	  }
	  // left gap closed
         /*   L       ML            R
	   |o----][----------][-------------|
	   N     i           j              M
           x -> MR           z
	 */
	  // create MR with 1 element ...
	  A[hole] = A[i];
	  A[i] = A[j];
	  A[j] = x;
	  z--;
	  i--;
	  j++;
	  goto Finish4;
	}
	// middle < y / postpone x
	// Shift ML to the left and add y to R or create MR
	A[lw] = A[j];
	A[j] = y;
	lw--;
	if ( compareXY(minr, y) <= 0 ) { // y -> R
	  j--;
	  z = j;
	  goto TLgMLRMR;
	}
	// y -> MR
	z--;
	goto TLgMLMRRMR;

	  // right gap closed, empty MR
 TLgMLRR:
	 /*   L                       R
	   |o----]---------+[-][--------------|
	   N     i        lw  j               M
           x -> R             z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = A[j];
	      A[j] = x;
	      j--;
	      z = j;
	      x = y;
	      lw--;
	 /*   L                       R
	   |o----]---------+[-][--------------|
	   N     i         lw j               M
           x -> L             z
	 */
	      goto TLgMLRL;
	    }
	    // left gap closed also & i=lw & y = A[i]
	    A[hole] = y; // A[hole] = A[i];
	    A[i] = A[j];
	    A[j] = x;
	    j--;
	    goto Finish3;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLRR;
	}
	// middle < y / postpone x
	// Shift ML to the left and add y to R or create MR
	A[lw] = A[j];
	A[j] = y;
	lw--;
	if ( compareXY(minr, y) <= 0 ) {  // y -> R
	   j--;
	   z = j;
	   goto TLgMLRR;
	}
	// y -> MR
	z--;
	goto TLgMLMRRR;

	// right gap closed, empty MR
 TLgMLRL:
	 /*   L                        R
	   |o----]---------+[-][--------------|
	   N     i        lw  j               M
           x -> L             z
	 */
	i++;
	y = A[i];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) goto TLgMLRL;
	  if ( i <= lw ) {
	    A[i] = x;
	    x = y;
	 /*   L                        R
	   |o----]---------+[-][--------------|
	   N     i        lw  j               M
           x -> ML            z
	 */
	    goto TLgMLRML;
	  }
	  // left gap closed
	  A[hole] = x;
	  goto Finish3;
	}
	// middle < y
	A[i] = x;
	x = y;
	if ( compareXY(minr, y) <= 0 ) goto TLgMLRR;
	goto TLgMLRMR;

	// right gap closed, empty MR
 TLgMLRML: 
	 /*   L                        R
	   |o----]---------+[-][--------------|
	   N     i        lw  j               M
           x -> ML            z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = x;
	      x = y;
	      lw--;
	      goto TLgMLRL;
	    }
	    // left gap also closed
	    A[hole] = A[i];
	    A[i] = x;
	    goto Finish3;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {  // shift ML to the left
	  A[lw] = A[j];
	  A[j] = y;
	  j--;
	  z = j;
	  lw--;
	  goto TLgMLRML;
	}
	// y -> MR !
	A[lw] = A[j];
	A[j] = y;
	z--;
	lw--;
        /*   L                            R
	   |o----]---------+[-|-------][-----|
	   N     i        lw          j      M
           x -> ML            z
	 */
	goto TLgMLMRRML;

	// right gap closed
 TLgMLMRRR:
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	   N     i        lw          j      M
           x -> R             z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {  // shift ML&MR to the left
	      A[lw] = A[z]; A[z] = A[j]; A[j] = x;
	      x = y;
	      lw--; z--; j--; 
	      goto TLgMLMRRL;
	    }
	    // left gap also closed; shift ML&MR to the left
	    A[hole] = A[i]; A[i] = A[z]; A[z] = A[j]; A[j] = x;
	    i--; z--; 
	    goto Finish4;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLMRRR;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) { // shift ML&MR to the left
	  A[lw] = A[z]; A[z] = A[j]; A[j] = y;
	  lw--; z--; j--; 
	  goto TLgMLMRRR;
	}
	// y -> MR; shift ML to the left
	A[lw] = A[z]; A[z] = y;
	lw--; z--; 
	goto TLgMLMRRR;

 TLgMLMRRMR:
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	   N     i        lw          j      M
           x -> MR            z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = A[z]; A[z] = x;
	      x = y;
	      z--; lw--;
	      goto TLgMLMRRL;
	    }
	    // left gap also closed
	    A[hole] = y; A[lw] = A[z]; A[z] = x;
	    i--; z--; j++;
	    goto Finish4;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLMRRMR;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  A[lw] = A[z]; A[z] = A[j]; A[j] = y;
	  z--; j--; lw--;
	  goto TLgMLMRRMR;
	}
	// y -> MR
	A[lw] = A[z]; A[z] = y;
	z--; lw--;
	goto TLgMLMRRMR;

 TLgMLMRRML:
        /*   L                         R
	   |o----]------+[--|------][-----|
	   N     i     lw          j      M
           x -> ML          z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = x;
	      x = y;
	      lw--;
	      goto TLgMLMRRL;
	    }
	    // left gap also closed
	    A[hole] = y; // y = A[i] = A[lw]
	    A[lw] = x;
	    i--; j++;
	    goto Finish4;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLMRRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  A[lw] = A[z]; A[z] = A[j]; A[j] = y;
	  z--; j--; lw--;
	  goto TLgMLMRRML;
	}
	// y -> MR
	A[lw] = A[z]; A[z] = y;
	z--; lw--;
	goto TLgMLMRRML;

 TLgMLMRRL:
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	   N     i        lw          j      M
           x -> L             z
	 */
	i++;
	y = A[i];
	if ( compareXY(y, middle) <= 0) {
	  if ( compareXY(y, maxl) <= 0 ) goto TLgMLMRRL;
	  if ( i <= lw ) {
	    A[i] = x;
	    x = y;
	    goto TLgMLMRRML;
	  }
	  // left gap closed
	  A[hole] = x;
	  i--; j++;
	  goto Finish4;
	}
	// middle < y
	A[i] = x;
	x = y;
	if ( compareXY(minr, y) <= 0 ) goto TLgMLMRRR;
	// y -> MR
	goto TLgMLMRRMR;

	// left gap closed
 TLMLMRgRL:
        /*   L                               R
	   |o----][-----------|-]+--------[-----|
	   N      i           z  up       j     M
           x -> L             
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) { // y -> L
	    z++;
	    A[up] = A[z]; A[z] = A[i]; A[i] = y;
	    i++; up++;
	    goto TLMLMRgRL;
	  }
	  // y -> ML
	  z++;
	  A[up] = A[z]; A[z] = y;
	  up++;
	  goto TLMLMRgRL;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLMLMRgRL;
	}
	// y -> R
	if ( up < j ) {
	  z++;
	  A[up] = A[z]; A[z] = A[i]; A[i] = x;
	  x = y;
	  i++; up++;
	  goto TLMLMRgRR;
	}
	// right gap closed
	A[hole] = x; 
	i--;
	goto Finish4;

 TLMLMRgRML:
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
	   N      i              up       j     M
           x -> ML            z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    z++;
	    A[up] = A[z]; A[z] = A[i]; A[i] = y;
	    i++; up++;
	    goto TLMLMRgRML;
	  }
	  // y -> ML
	  z++;
	  A[up] = A[z]; A[z] = y;
	  up++;
	  goto TLMLMRgRML;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLMLMRgRML;
	}
	// y --> R
	if ( up < j ) {
	    z++;
	    A[up] = A[z]; A[z] = x;
	    x = y;
	    up++;
	    goto TLMLMRgRR;
	}
	// right gap closed
	i--;
	A[hole] = A[i]; A[i] = x;
	i--;
	goto Finish4;
	
 TLMLMRgRR:
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
	   N      i              up       j     M
           x -> R             z
	 */
	j--;
	y = A[j];
	if ( compareXY(y, middle) <= 0 ) {
	  A[j] = x;
	  x = y;
	  if ( compareXY(y, maxl) <= 0 ) {
	    goto TLMLMRgRL;
	  }
	  // y -> ML
	  goto TLMLMRgRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) goto TLMLMRgRR;
	// y -> MR
	if ( up <= j ) {
	  A[j] = x;
	  x = y;
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
	   N      i              up       j     M
           x -> MR            z
	 */
	  goto TLMLMRgRMR;
	}
	// right gap closed
	i--;
	A[hole] = A[i]; A[i] = A[z]; A[z] = A[j]; A[j] = x;
	z--; i--;
	goto Finish4;

 TLMLMRgRMR:
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
	   N      i              up       j     M
           x -> MR            z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    z++;
	    A[up] = A[z]; A[z] = A[i]; A[i] = y;
	    i++; up++;
	    goto TLMLMRgRMR;
	  }
	  // y -> ML
	  z++;
	  A[up] = A[z]; A[z] = y;
	  up++;
	  goto TLMLMRgRMR;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLMLMRgRMR;
	}
	// y --> R
	if ( up < j ) {
	  A[up] = x;
	  x = y;
	  up++;
	  goto TLMLMRgRR;
	}
	// right gap closed
	i--;
	A[hole] = A[i]; A[i] = A[z]; A[z] = x;
	i--; z--;
	goto Finish4;

	// left gap closed, empty MR
 TLMLgRL:
	 /*    L                          R
	   |o----][----------]+--------[-----|
	   N      i           up       j     M
           x -> L            z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    A[up] = A[i];
	    A[i] = y;
	    i++; z = up; up++; 
	    goto TLMLgRL;
	  }
	  // y -> ML
	  z = up; up++; 
	  goto TLMLgRL;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) { // y -> MR ! create MR
	  up++;
	  goto TLMLMRgRL;
	}
	// y --> R
	if ( up < j ) {
	  A[up] = A[i]; A[i] = x;
	  x = y;
	  i++; z = up; up++; 
	  goto TLMLgRR;
	}
	// right gap closed
	A[hole] = x;
	j--;
	goto Finish3;

	// left gap closed, empty MR
 TLMLgRMR:
         /*   L                           R
	   |o----][----------]+--------[-----|
	   N      i           up       j     M
           x -> MR           z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    A[up] = A[i]; A[i] = y;
	    i++; z = up; up++; 
	    goto TLMLgRMR;
	  }
	  // y -> ML
	  z = up; up++; 
	  goto TLMLgRMR;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) { // create MR !
	  up++;
	  goto TLMLMRgRMR;
	}
	// y --> R
	if ( up < j ) { // create MR !
	  A[up] = x;
	  x = y;
	  up++;
	  goto TLMLMRgRR;
	}
	// right gap closed // create MR, single element
	i--;
	A[hole] = A[i]; A[i] = A[z]; A[z] = x;
	i--; z--;
	goto Finish4;

	// left gap closed, empty MR
 TLMLgRR:
         /*   L                           R
	   |o----][----------]+--------[-----|
	   N      i           up       j     M
           x -> R            z
	 */
	j--;
	y = A[j];
	if ( compareXY(y, middle) <= 0 ) {
	  A[j] = x;
	  x = y;
	  if ( compareXY(y, maxl) <= 0 ) goto TLMLgRL;
	  // y -> ML
	  goto TLMLgRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) goto TLMLgRR;
	// y --> MR !
	if ( up < j ) {
	  A[j] = x;
	  x = y;
	  goto TLMLgRMR;
	}
	// both gaps closed, create MR, single element
	i--;
	A[hole] = A[i]; A[i] = A[z]; A[z] = y; A[j] = x;
	i--; z--;
	goto Finish4;

 Finish0:
	// printf("cut4P exit Finish0 N: %d M: %d\n", N, M);
	return;

	// +++++++++++++++both gaps closed+++++++++++++++++++
	int lleft, lmiddle, lright;
	// int k; // for testing
 Finish3:
	// printf("cut4P exit Finish3 N: %d M: %d\n", N, M);

         /*   L            ML/MR          R
	   |-----][-------------------][-----|
	   N      i                   j      M
	 */
	/*
	for  (k = N; k < i; k++) 
	  if ( maxl < A[k] ) {
	    printf("%s %d %s %d %s", "L3 error N: ", N, " M: ", M, "\n");
	    printf("%s %d %s %d %s", "L3 error N: ", N, " i: ", i, "\n");
	    printf("%s %d %s %d %s", "L3 error k: ", k, " k: ", k, "\n");
	    printf("%s %d %s %d %s", "L3 error maxl: ", maxl, " minr: ", minr, "\n");
	  }
	for  (k = i; k <= j; k++) 
	  if ( A[k] <= maxl || minr <= A[k] ) {
	    printf("%s %d %s %d %s", "M3 error N: ", N, " M: ", M, "\n");
	    printf("%s %d %s %d %s", "M3 error i: ", i, " j: ", j, "\n");
	    printf("%s %d %s %d %s", "M3 error k: ", k, " k: ", k, "\n");
	    printf("%s %d %s %d %s", "M3 error maxl: ", maxl, " minr: ", minr, "\n");
	  }
	for  (k = j+1; k <= M; k++) 
	  if ( A[k] < minr ) {
	    printf("%s %d %s %d %s", "R3 error N: ", N, " M: ", M, "\n");
	    printf("%s %d %s %d %s", "R3 error j: ", j, " M: ", M, "\n");
	    printf("%s %d %s %d %s", "R3 error k: ", k, " k: ", k, "\n");
	    printf("%s %d %s %d %s", "R3 error maxl: ", maxl, " minr: ", minr, "\n");
	  }
	*/

	lleft = i - N;
	lmiddle = j - i;
	lright = M - j;
	if ( lleft < lmiddle ) {      // L < M
	  if ( lleft < lright ) {     // L < R
	    // cut4P(N, i-1);
	    if ( lmiddle < lright ) { // L < M < R
	      addTaskSynchronized(ll, newTask(j+1, M, depthLimit));
	      addTaskSynchronized(ll, newTask(i, j, depthLimit));
	      // cut4P(i, j);
	      // cut4P(j+1, M);
	    } else {                  // L < R < M  
	      addTaskSynchronized(ll, newTask(i, j, depthLimit));   
	      addTaskSynchronized(ll, newTask(j+1, M, depthLimit));
	      // cut4P(j+1, M);
	      // cut4P(i, j);
	    }
	    cut4Pc(N, i-1, depthLimit);
	    return;
	  } else {                    // R < L < M 
	    // cut4P(j+1, M);
	    // cut4P(N, i-1);
	    // cut4P(i, j);
	    addTaskSynchronized(ll, newTask(i, j, depthLimit));
	    addTaskSynchronized(ll, newTask(N, i-1, depthLimit));
	    cut4Pc(j+1, M, depthLimit);
	    return;
	  }
	}                             // M < L
	if ( lmiddle < lright ) {     // M < R
	  // cut4P(i, j);
	  if ( lleft < lright) {
	    // cut4P(N, i-1);
	    // cut4P(j+1, M);
	    addTaskSynchronized(ll, newTask(j+1, M, depthLimit));
	    addTaskSynchronized(ll, newTask(N, i-1, depthLimit));
	  } else {
	    // cut4P(j+1, M);
	    // cut4P(N, i-1);
	    addTaskSynchronized(ll, newTask(N, i-1, depthLimit));
	    addTaskSynchronized(ll, newTask(j+1, M, depthLimit));
	  }
	  cut4Pc(i, j, depthLimit);
	  return;
	}                             // R < M < L
	// cut4P(j+1, M);
	// cut4P(i, j);
	// cut4P(N, i-1);
	addTaskSynchronized(ll, newTask(N, i-1, depthLimit));
	addTaskSynchronized(ll, newTask(i, j, depthLimit));
	cut4Pc(j+1, M, depthLimit);
	return;

 Finish4:
	// printf("cut4P exit Finish4 N: %d M: %d\n", N, M);
         /*   L        ML         MR         R
	   |-----][----------|-----------][-----|
	   N     i           z            j     M
	 */
	if ( z-N < M-z ) {
	  /*
	  cut4P(N, i);
	  cut4P(i+1, z);
	  if ( j-z < M-j ) {
	    cut4P(z+1, j-1);
	    cut4P(j, M);
	    return;
	  }
	  cut4P(j, M);
	  cut4P(z+1, j-1);
	  */
	  if ( j-z < M-j ) {
	    // cut4P(z+1, j-1);
	    // cut4P(j, M);
	    addTaskSynchronized(ll, newTask(j, M, depthLimit));
	    addTaskSynchronized(ll, newTask(z+1, j-1, depthLimit));
	  } else {
	    // cut4P(j, M);
	    // cut4P(z+1, j-1);
	    addTaskSynchronized(ll, newTask(z+1, j-1, depthLimit));
	    addTaskSynchronized(ll, newTask(j, M, depthLimit));
	  }
	  addTaskSynchronized(ll, newTask(i+1, z, depthLimit));
	  cut4Pc(N, i, depthLimit);
	  return;
	}
	// M-z <= z-N
	//cut4P(z+1, j-1);
	// cut4P(j, M);
	if ( i-N < z-i ) {
	  // cut4P(N, i);
	  // cut4P(i+1, z);
	  addTaskSynchronized(ll, newTask(N, i, depthLimit));
	  addTaskSynchronized(ll, newTask(i+1, z, depthLimit));
	} else {
	  // cut4P(i+1, z);
	  // cut4P(N, i);
	  addTaskSynchronized(ll, newTask(i+1, z, depthLimit));
	  addTaskSynchronized(ll, newTask(N, i, depthLimit));
	}
	addTaskSynchronized(ll, newTask(j, M, depthLimit));
	cut4Pc(z+1, j-1, depthLimit);
} // end cut4Pc

void heapSort();
void heapc(void **A, int N, int M) {
  if ( M <= N ) return;
  // printf("Entering heapc %d %d\n", N, M);
  heapSort(&A[N], M-N+1);
} // end heapc

void heapify();
void siftDown(); 
void heapSort(void **a, int count) {
  // input:  an unordered array a of length count
  // first place a in max-heap order
  heapify(a, count);
  // in languages with zero-based arrays the children are 2*i+1 and 2*i+2
  int end = count-1; 
  while ( end > 0 ) {
    // (swap the root(maximum value) of the heap with 
    // the last element of the heap)
    // swap(a[end], a[0]);
    iswap(end, 0, a);
    // (decrease the size of the heap by one so that the 
    // previous max value will stay in its proper placement) 
    end = end - 1;
    // (put the heap back in max-heap order)
    siftDown(a, 0, end);
  }
} // end heapSort
        
void heapify(void **a, int count) {
  // (start is assigned the index in a of the last parent node)
  int start = (count - 2) / 2;
  while ( 0 <= start ) {
    // (sift down the node at index start to the proper place such 
    // that all nodes below the start index are in heap order)
    siftDown(a, start, count-1);
    start = start - 1;
  } // (after sifting down the root all nodes/elements are in heap order)
} // end heapify
 
void siftDown(void **a, int start, int end) {
  // input:  end represents the limit of how far down the heap to sift.
  int root = start;
  int child, swapi;
  // (While the root has at least one child)
  while ( root * 2 + 1 <= end ) {
    child = root * 2 + 1; // (root*2 + 1 points to the left child)
    swapi = root; // (keeps track of child to swap with)
    // (check if root is smaller than left child)
    // if ( a[swapi] < a[child] ) 
    if ( compareXY(a[swapi], a[child]) < 0 ) 
      swapi = child;
    // (check if right child exists, and if it's bigger 
    // than what we're currently swapping with)
    // if ( child+1 <= end && a[swapi] < a[child+1] )
    if ( child+1 <= end && compareXY(a[swapi],a[child+1]) < 0 )
      swapi = child + 1;
    // (check if we need to swap at all)
    if ( swapi != root ) {
      // swap(a[root], a[swapi]);
      iswap(root, swapi, a);
      root = swapi; // (repeat to continue sifting down the child now)
    } else return;
  }
} // end siftDown

void *myMallocSS(char* location, int size) {
  void *p = malloc(size);
  if ( 0 == p ) {
    fprintf(stderr, "malloc fails for: %s\n", location);
    // printf("Cannot allocate memory in: %s\n", location);
    exit(1);
  }
  return p;
} // end of myMalloc


  // To obtain the int n field from X: ((struct task *) X)->n
  // To obtain the int m field from X: ((struct task *) X)->m
  // To obtain the task next field from X: ((struct task *) X)->next
struct task {
  int n;
  int m;
  int dl;
  struct task *next;
};
int getN(struct task *t) { return ((struct task *) t)->n; }
int getM(struct task *t) { return ((struct task *) t)->m; }
int getDL(struct task *t) { return ((struct task *) t)->dl; }
struct task *getNext(struct task *t) { return ((struct task *) t)->next; }

void setN(struct task *t, int n) { ((struct task *) t)->n = n; }
void setM(struct task *t, int m) { ((struct task *) t)->m = m; }
void setDL(struct task *t, int dl) { ((struct task *) t)->dl = dl; }
void setNext(struct task *t, struct task* tn) { 
  ((struct task *) t)->next = tn; }
struct task *newTask(int N, int M, int depthLimit) {
  struct task *t = (struct task *) 
    myMallocSS("ParSixSort/ newTask()", sizeof (struct task));
  setN(t, N); setM(t, M); setDL(t, depthLimit); setNext(t, NULL);
  return t;
} // end newTask

struct stack {
  struct task *first;
  int size;
};
struct task *getFirst(struct stack *ll) { 
  return ((struct stack *) ll)->first; }
int getSize(struct stack *ll) { 
  return ((struct stack *) ll)->size; }
void setFirst(struct stack *ll, struct task *t) { 
  ((struct stack *) ll)->first = t; }
void setSize(struct stack *ll, int s) { 
  ((struct stack *) ll)->size = s; }
void incrementSize(struct stack *ll) { 
  setSize(ll, getSize(ll) + 1); }
void decrementSize(struct stack *ll) { 
  setSize(ll, getSize(ll) - 1); }
struct stack *newStack() {
  struct stack *ll = (struct stack *)
    myMallocSS("ParSixSort/ newStack()", sizeof (struct stack));
  setFirst(ll, NULL); setSize(ll, 0);
  return ll;
} // end newStack
int isEmpty(struct stack *ll) { 
  return ( NULL == getFirst(ll) ); 
} // end isEmpty
struct task *pop(struct stack *ll) {
  struct task *t = getFirst(ll);
  if ( NULL == t ) return NULL;
  setFirst(ll, getNext(t));
  decrementSize(ll);
  return t;
} // end pop
void push(struct stack *ll, struct task *t) {
  if ( !isEmpty(ll) ) setNext(t, getFirst(ll)); 
  setFirst(ll, t);
  incrementSize(ll);
} // end push


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


void *sortThread(void *A) { // A-argument is NOT used
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
    int n = getN(t);
    int m = getM(t);
    int depthLimit = getDL(t);
    free(t);
    // taskCnt++;
    cut4Pc(n, m, depthLimit);
  }

  //  printf("Exit of Thread number: %ld taskCnt: %d\n", pthread_self(), taskCnt);
  return NULL;
} // end sortThread

int cut2SLimit = 2000;
void sixsort(void **AA, int size, 
	int (*compar ) (const void *, const void * ),
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
  A = AA;
  compareXY = compar;
  if ( size <= cut2SLimit || numberOfThreads <= 0) {
    cut2(0, size-1);
    return;
  }
  sleepingThreads = 0;
  NUMTHREADS = numberOfThreads;
  // printf("Entering sortArray\n");
  ll = newStack();
  int depthLimit = 2 * floor(log(size + 1));
  struct task *t = newTask(0, size-1, depthLimit);
  addTaskSynchronized(ll, t);

  pthread_t thread_id[NUMTHREADS];
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
