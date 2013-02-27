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

// File: c:/bsd/rigel/sort/SixSort.c
// Date: Thu Jan 07 15:08:48 2010
/* This file has the source of the algorithms that make up SixSort
   headed by sixsort
*/
/* compile with: 
   gcc -c SixSort.c
   which produces:
   SixSort.o
   Compile & link with UseSixSort:
   gcc UseSixSort.c SixSort.o
*/
// To adjust restrictions: go to sixsort(...) at the end of this file 


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

const int cut2Limit = 250;
const int cut3Limit = 250; 
const int cut4Limit = 500; 
const int probeParamCut4 = 50000;

// Here more global entities used throughout
int (*compareXY)();
void **A;


char* expiration = "*** License for sixsort has expired ...\n";

void iswap(int p, int q, void **A) {
  void *t = A[p];
  A[p] = A[q];
  A[q] = t;
} // end of iswap

// calculate the median of 3
int med(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
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
  while ( s < M ) { // if ( M == s ) return;
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
    fprintf(stderr, "SixSort/ insertionsort() ");
    fprintf(stderr, "N: %d M %d s: s\n", N, M, s);
    exit(1);
  }
  return;
} // end insertionsort

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
	  cut3duplicates(N, M, duplicate, quicksort0, depthLimit);
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

void cut3duplicates();
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
        int sixth = (M - N + 1) / 6;
        int e1 = N  + sixth;
        int e5 = M - sixth;
        int e3 = (N+M) / 2; // The midpoint
        int e4 = e3 + sixth;
        int e2 = e3 - sixth;

        // Sort these elements using a 5-element sorting network
        void *ae1 = A[e1], *ae2 = A[e2], *ae3 = A[e3], *ae4 = A[e4], *ae5 = A[e5];
	void *t;
        // if (ae1 > ae2) { t = ae1; ae1 = ae2; ae2 = t; }
	int r; void *duplicate = NULL;
	r = compareXY(ae1, ae2);
	if ( 0 == r ) duplicate = ae2; else                  // 1-2
	  if ( 0 < r ) { t = ae1; ae1 = ae2; ae2 = t; }

	r = compareXY(ae4, ae5);
	if ( 0 == r ) duplicate = ae4; else                  // 4-5
	  if ( 0 < r ) { t = ae4; ae4 = ae5; ae5 = t; }

	r = compareXY(ae1, ae3);
	if ( 0 == r ) duplicate = ae3; else                  // 1-3
	  if ( 0 < r ) { t = ae1; ae1 = ae3; ae3 = t; }

	r = compareXY(ae2, ae3);
	if ( 0 == r ) duplicate = ae3; else                  // 2-3
	  if ( 0 < r ) { t = ae2; ae2 = ae3; ae3 = t; }

	r = compareXY(ae1, ae4);
	if ( 0 == r ) duplicate = ae4; else
	  if ( 0 < r ) { t = ae1; ae1 = ae4; ae4 = t; }      // 1-4

	r = compareXY(ae3, ae4);
	if ( 0 == r ) duplicate = ae3; else
	  if ( 0 < r ) { t = ae3; ae3 = ae4; ae4 = t; }      // 3-4

	r = compareXY(ae2, ae5);
	if ( 0 == r ) duplicate = ae2; else
	  if ( 0 < r ) { t = ae2; ae2 = ae5; ae5 = t; }      // 2-5

	r = compareXY(ae2, ae3);
	if ( 0 == r ) duplicate = ae3; else                  // 2-3
	  if ( 0 < r ) { t = ae2; ae2 = ae3; ae3 = t; }

	r = compareXY(ae4, ae5);                             // 4-5
	if ( 0 == r ) duplicate = ae4; else
	  if ( 0 < r ) { t = ae4; ae4 = ae5; ae5 = t; }
	// ... and reassign
	A[e1] = ae1; A[e2] = ae2; A[e3] = ae3; A[e4] = ae4; A[e5] = ae5;

	if ( NULL != duplicate ) { // found a duplicate thus delegate
	  int duplicatex = 0;
	  if ( duplicate == ae4 ) { duplicatex = e4; } else
	  if ( duplicate == ae3 ) { duplicatex = e3; } else
	  if ( duplicate == ae2 ) { duplicatex = e2; } else
	  if ( duplicate == ae1 ) { duplicatex = e1; } else
	  if ( duplicate == ae5 ) { duplicatex = e5; } else
	    { goto skip; } // defensive
	  void cut2c();
	  cut3duplicates(N, M, duplicatex, cut2c, depthLimit);
	  return;
	}
 skip:

	// Fix end points
	if ( compareXY(ae1, A[N]) < 0 ) iswap(N, e1, A);
	if ( compareXY(A[M], ae5) < 0 ) iswap(M, e5, A);

	register void *T = ae3; // pivot

	register  int I, J; // indices
	register void *AI, *AJ; // array values


	// initialize running indices
	I= N;
	J= M;

	// The left segment has elements < T
	// The right segment has elements >= T
  Left:
	 I = I + 1;
	 AI = A[I];
	 // if (AI < T) goto Left;
	 if ( compareXY( AI,  T) < 0 ) goto Left;
  Right:
	 J = J - 1;
	 AJ = A[J];
	 // if ( T <= AJ ) goto Right;
	 if ( compareXY( T, AJ) <= 0 ) goto Right;
	 if ( I < J ) {
	   A[I] = AJ; A[J] = AI;
	   goto Left;
	 }
	 // Tail recursion
  	 if ( (I - N) < (M - J) ) { // smallest one first
	   cut2c(N, J, depthLimit);
	   N = I; 
	   goto Start;
	 }
	 cut2c(I, M, depthLimit);
	 M = J;
	 goto Start;
} // (*  OF cut2; *) ... the brackets reminds that this was Pascal code

void cut3c();
// cut3 is for direct invocation of cut3c - for testing purposes
// cut4c invokes cut3c directly
void cut3(int N, int M) {
  int L = M - N;
  if ( L < cut3Limit ) { 
    quicksort0(N, M);
    return;
  }
  int depthLimit = 2 * floor(log(L));
  cut3c(A, N, M, depthLimit);
} // end cut3

// Three partition sorting with 2 pivots
void cut3c(int N, int M, int depthLimit) 
{
  int L; // M-N
  register int k; // for-index
  register int i, j; // indices
  register void *ai, *aj; // array values
  register void *pl, *pr; // pivots

  // A 3d recursive call is avoided by jumping back to Start. 
  Start: 
  if ( depthLimit <= 0 ) { // avoid quadradic explosion
    heapc(A, N, M);
    return;
  }

  L = M - N;
  if ( L < cut3Limit ) { // delegated small segments
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
	int r; void *duplicate = NULL;
	r = compareXY(ae1, ae2);
	if ( 0 == r ) duplicate = ae2; else                  // 1-2
	  if ( 0 < r ) { t = ae1; ae1 = ae2; ae2 = t; }

	r = compareXY(ae4, ae5);
	if ( 0 == r ) duplicate = ae4; else                  // 4-5
	  if ( 0 < r ) { t = ae4; ae4 = ae5; ae5 = t; }

	r = compareXY(ae1, ae3);
	if ( 0 == r ) duplicate = ae3; else                  // 1-3
	  if ( 0 < r ) { t = ae1; ae1 = ae3; ae3 = t; }

	r = compareXY(ae2, ae3);
	if ( 0 == r ) duplicate = ae3; else                  // 2-3
	  if ( 0 < r ) { t = ae2; ae2 = ae3; ae3 = t; }

	r = compareXY(ae1, ae4);
	if ( 0 == r ) duplicate = ae4; else
	  if ( 0 < r ) { t = ae1; ae1 = ae4; ae4 = t; }      // 1-4

	r = compareXY(ae3, ae4);
	if ( 0 == r ) duplicate = ae3; else
	  if ( 0 < r ) { t = ae3; ae3 = ae4; ae4 = t; }      // 3-4

	r = compareXY(ae2, ae5);
	if ( 0 == r ) duplicate = ae2; else
	  if ( 0 < r ) { t = ae2; ae2 = ae5; ae5 = t; }      // 2-5

	r = compareXY(ae2, ae3);
	if ( 0 == r ) duplicate = ae3; else                  // 2-3
	  if ( 0 < r ) { t = ae2; ae2 = ae3; ae3 = t; }

	r = compareXY(ae4, ae5);                             // 4-5
	if ( 0 == r ) duplicate = ae4; else
	  if ( 0 < r ) { t = ae4; ae4 = ae5; ae5 = t; }
	// ... and reassign
	A[e1] = ae1; A[e2] = ae2; A[e3] = ae3; A[e4] = ae4; A[e5] = ae5;

	if ( NULL != duplicate ) { // found a duplicate thus delegate
	  int duplicatex = 0;
	  if ( duplicate == ae4 ) { duplicatex = e4; } else
	  if ( duplicate == ae3 ) { duplicatex = e3; } else
	  if ( duplicate == ae2 ) { duplicatex = e2; } else
	  if ( duplicate == ae1 ) { duplicatex = e1; } else
	  if ( duplicate == ae5 ) { duplicatex = e5; } else
	    { goto skip; } // defensive
	  void cut2c();
	  cut3duplicates(N, M, duplicatex, cut2c, depthLimit);
	  return;
	}
 skip:

	// Fix end points
	// if ( ae1 < A[N] ) iswap(N, e1, A);
	if ( compareXY(ae1, A[N]) < 0 ) iswap(N, e1, A);
	if ( compareXY(A[M], ae5) < 0 ) iswap(M, e5, A);
	pl = A[e2]; pr = A[e4];


	// A[N] < pl < A[e3] < pr < A[M]
	// Thus all three sections are NOT empty ! 

	// initialize running indices
	k = i = N;
	j = M;

	/* 
	  |-]--------------------------[-|
	  N i                          j M

	   pl < pr
	   N <= x <= i  ==> A[x] < pl
	   j <= x <= M  ==> pr < A[x]
	*/

	while (1) {
	  k++;
	  // printf("Start k %i \n", k);
	  ai = A[k];
	  // if ( ai < pl ) { 
	  if ( compareXY(ai, pl) < 0 ) { 
	    i++;
	    A[k] = A[i]; A[i] = ai;
	    continue;
	  }
	  // if ( ai <= pr ) continue;
	  if ( compareXY(ai, pr) <= 0 ) continue;
	  break;
	}

 Right:

	/* 
	  |-]----]o--------------------[-|
	  N i     k                    j M

	   N <= x <= i  ==> A[x] < pl
	   j <= x <= M  ==> pr < A[x]
	   i < x < k    ==> pl <= A[x] <= pr
	   pr < ai = A[k]
	*/

	// printf("Right N: %i i: %i k: %i j: %i M: %i \n", N, i, k, j, M);

	/*
	for ( z = N; z <= i; z++ ) 
	  if ( pl <= A[z] ) printf("RightL z: %i\n", z);

	for ( z = i+1; z < k; z++ ) 
	  if ( A[z] < pl ) printf("RightM1 z: %i\n", z);
	for ( z = i+1; z < k; z++ ) 
	  if ( pr < A[z] ) printf("RightM2 z: %i\n", z);

	for ( z = j; z <= M; z++ ) 
	  if ( A[z] <= pr ) printf("RightR z: %i\n", z);
	*/

	if ( k == j ) goto Finish;
	while (1) {
	  j--;
	  aj = A[j];
	  // if ( aj < pl ) {
	  if ( compareXY(aj, pl) < 0 ) {
	    A[j] = ai; 
	    i++;
	    A[k] = A[i]; A[i] = aj;
	    break;
	  }
	  // if ( aj <= pr ) {
	  if ( compareXY(aj, pr) <= 0 ) {
	    A[k] = aj; A[j] = ai;
	    break;
	  }
	  // pl < aj
	  if ( k == j ) goto Finish;
	  // continue;	  
	}
 Left:
	/* 
	  |-]----]--------------------[-|
	  N i    k                    j M

	   N <= x <= i  ==> A[x] < pl
	   j <= x <= M  ==> pr < A[x]
	   i < x <= k   ==> pl <= A[x] <= pr
	*/

	// printf("Left N: %i i: %i k: %i j: %i M: %i \n", N, i, k, j, M);

	while (1) {
	  k++;
	  ai = A[k];
	  // if ( ai < pl ) {
	  if ( compareXY(ai, pl) < 0 ) {
	    i++;
	    A[k] = A[i]; A[i] = ai;
	    continue;
	  }
	  // if ( ai <= pr ) continue;
	  if ( compareXY(ai, pr) <= 0 ) continue;
	  if ( k == j ) break;
	  goto Right;
	}

 Finish:
	// printf("Finish  i: %i j: %i \n", i, j);
	  /*
	      |------]---------[----------------|
	      N      i         j                M

	      N < i < j < M
	      N <= x <= i  ==> A[x] < pl
	      i < x < j  ==> pl <= A[x] <= pr
	      j <= x <= M ==> pr < A[x] 
	  */

	// Recurse in order of segment size	
	if ( i - N < j - i ) {
	  cut3c(N, i, depthLimit);
	  if ( j - i < M - j ) {
	    cut3c(i+1, j-1, depthLimit);
	    N = j;
	    goto Start;
	  }
	  cut3c(j, M, depthLimit);
	  N = i+1; M = j-1;
	  goto Start;
	}
	if ( j - i < M - j ) {
	  cut3c(i+1, j-1, depthLimit);
	  if ( i - N < M - j) {
	    cut3c(N, i, depthLimit);
	    N = j;
	    goto Start;
	  }
	  cut3c(j, M, depthLimit);
	  M = i;
	  goto Start;
	}
	cut3c(j, M, depthLimit);
	cut3c(i+1, j-1, depthLimit);
	M = i;
	goto Start;
} // end cut3c


/* deadx is NOT used for sorting.  It can be employed for 
   a test to invoke cut3duplicates */
void deadxc();
void deadx(int N, int M) {
  // printf("deadx N: %d M: %d\n", N, M);
  int L = M - N;
  if ( L <= 1 ) return;    
  int depthLimit = 2 * floor(log(L));
  deadxc(N, M, depthLimit);
} // end deadx

/* deadxc is also NOT used for sorting.  It is employed for 
   a test to invoke cut3duplicates */
void deadxc(int N, int M, int depthLimit) {
  if ( depthLimit <= 0 ) {
    heapc(A, N, M);
    return;
  }
  int L = M - N;  
  if ( L < cut3Limit ) { 
    quicksort0c(N, M, depthLimit);
    return;
  }
  depthLimit--;

    int pivotx;
    int midpointx = (M+N)/2;
    if ( L <= 30 ) { // being defensive
      pivotx = med(A, N, midpointx, M, compareXY);
    } else {
      int step = L * 0.1;
      int P = med(A, N +   step, N + 2*step, N + 3*step, compareXY);
      int Q = med(A, midpointx - step, midpointx, midpointx + step, compareXY);
      int R = med(A, M - 3*step, M - 2*step, M - step, compareXY);
      pivotx = med(A, P, Q, R, compareXY);
    }

    void deadxc();
    cut3duplicates(N, M, pivotx, deadxc, depthLimit);
} // end deadxc

/* Three partition sorting with a single pivot p
   Left elements are less than the pivot
   Middle elements are equal to the pivot
   Right elements are greater than the pivot
   Only middle elements are guaranteed to be non-empty
   This module is invoked on 'pathological' inputs */
void cut3duplicates(int N, int M, int pivotx, void (*cut)(), int depthLimit) {
  // pivotx is the index inside [N,M] for pivot p.  *** DO NOT CHANGE ***
  register void *p = A[pivotx]; // pivot
  // indices 
  register int i, j, lw, up;
  // values
  register void *x, *y, *AM;
  register int r; // used for capturing output of comparison operation

  // initialize end points
  x = A[N]; AM = A[M];
  A[N] = A[M] = p;

  /* We employ dispose/whack-a-mole. We know in which partition element x 
     should be.  Find a close, undecided position where x should go.  
     Store its content in y.  Drop x (simplified).  Set x to y and repeat. 
   */
  // initialize running indices
  i = N;
  j = M;
  //  if ( x < p ) goto StartL;
  r = compareXY(x, p);
  if ( r < 0 ) goto StartL;
  //  if ( p < x ) goto StartR;
  if ( r > 0 ) goto StartR;
  goto CreateMiddle;

 StartL:
  /*
    |o---]-----------------------------[---o|
    N    i                             j    M
    x -> L
   */
    i++;
  y = A[i];
  r = compareXY(y, p);
  if ( r < 0 ) goto StartL; 
  // if ( p < y ) { 
  if ( 0 < r ) { 
    if ( i < j ) {
      A[i] = x;
      x = y;
      goto StartR;
    }
    // gap closed; i==j 
    /*
    |o-----------][----------------------o|
    N             i                       M
    x -> L        j
   */
    A[N] = x;
    // AM must be p
    A[M] = A[j];
    A[j] = AM;
    goto Finish;
  }
  // y = p -> M
  if ( i < j ) {
    A[i] = x;
    x = y;
    goto CreateMiddle;
  }
  /* We have:
     x -> L
     y -> M
     i = j
     N+i <= k < i -> A[k] < p
     i = j <= k < M -> p < A[k]
     Therefore: 
     x must go to A[N] and 
     p = A[M], the only location where y can go.
     Hence: i = j = M 
     This cannot happen under normal use because there must be
     more than one element equal to p when used by the other 
     members of Par/SixSort

    |o------------------------------------]o|
    N                                      M=i=j
    x -> L
   */
  A[N] = x;
  // AM must be p hence the next assignment is defensive
  A[M] = AM;
  i--;
  goto FinishL;

 StartR:
  /*
    |o---]-----------------------------[---o|
    N    i                             j    M
    x -> R
   */
  j--;
  y = A[j];
  r = compareXY(y, p);
  if ( 0 < r ) goto StartR;
  if ( r < 0 ) {
    if ( i < j ) {
      A[j] = x;
      x = y;
      goto StartL;
    }
    // gap closed; i==j
    /*
    |o-----------][----------------------o|
    N            i                        M
    x -> R       j
   */
    A[M] = x;
    // AM must be p
    A[N] = A[i]; A[i] = AM;
    goto Finish;
  }
  // y -> M
  if ( i < j ) {
    A[j] = x;
    x = y;
    goto CreateMiddle;
  }
  /* We have:
     x -> R
     y -> M
     i = j
     N+i <= k <= i -> A[k] < p
     i = j < k < M -> p < A[k]
     Therefore: 
     x must go to A[M] and 
     p = A[N], the only location where y can go.
     Hence: i = j = N
     This cannot happen under normal use because there must be
     more than one element equal to p when used by the other 
     members of Par/SixSort.

    |o][-----------------------------------o|
    N=i=j                                   M
    x -> R
   */
  A[M] = x;
  // AM must be p
  A[N] = AM;
  j++;
  goto FinishR;

 CreateMiddle:
  /*
    |o---]-----------------------------[---o|
    N    i                             j    M
    x -> M
   */
  if ( i+1 == j ) {
    /*
    |o--------------][---------------------o|
    N               ij                      M
    x -> M
    */
    // if ( AM < p ) {
    r = compareXY(AM, p);
    if ( r < 0 ) {
      A[N] = AM;
      A[M] = A[j]; A[j] = x;
      i++;
      goto Finish;
    } 
    //    if ( p < AM ) {  
    if ( 0 < r ) {
      A[M] = AM;
      A[N] = A[i]; A[i] = x;
      j--;
      goto Finish;
    }
    // p = AM
    A[N] = A[i]; A[i] = x; 
    A[M] = A[j]; A[j] = AM; 
    goto Finish;
  }
  lw = (i+j)/ 2;
  up = lw+1;
  y = A[lw];
  A[lw] = x;
  x = y;
  lw--;

  /*
    |o---]-----------+[-]+-------------[---o|
    N    i          lw   up            j    M
   x -> ?
   */
  // if ( x < p ){
  r = compareXY(x, p);

  if ( r < 0 ) {
    goto LLgMgRL;
  }
  // if ( p < x ) {
  if ( 0 < r ) {
    goto LLgMgRR;
  }
  goto LLgMgRM1;

 LLgMgRL:
  /*
    |o---]-----------+[-]+-------------[---o|
    N    i          lw   up            j    M
    x -> L
   */
  i++;
  y = A[i];
  r = compareXY(y, p);
  if ( r < 0 ) goto LLgMgRL;
  if ( 0 < r ) {
    A[i] = x;
    x = y;
    goto LLgMgRR;
  }
  // y -> M
  if ( i <= lw ) {
    A[i] = x;
    x = y;
    goto LLgMgRM1;
  }
  // left gap closed
  /*
    |o---------][------]+-------------[---o|
    N           i       up            j    M
    x -> L
   */
  goto LLMgRL;
  
 LLgMgRR:
  /*
    |o---]-----------+[-]+-------------[---o|
    N    i          lw   up            j    M
    x -> R
   */
  j--;
  y = A[j];
  r = compareXY(p, y );
  if ( r < 0 ) goto LLgMgRR;
  if ( 0 < r ) {
    A[j] = x;
    x = y;
    goto LLgMgRL;
  }
  // y -> M
  if ( up <= j ) {
    A[j] = x;
    x = y;
    goto LLgMgRM2;
  }
  // right gap closed
  /*
    |o---]-----------+[---------][---------o|
    N    i          lw          j           M
    x -> R
   */
  goto LLgMRR;
  
 LLgMgRM1:
  /*
    |o---]-----------+[-]+-------------[---o|
    N    i          lw   up            j    M
    x -> M
   */
  y = A[lw];
  r = compareXY(p, y);
  if ( r < 0 ) {
    A[lw] = x;
    x = y;
    lw--; 
    goto LLgMgRR;
  } 
  if ( r == 0 ) {
   if ( i < lw ) {
      lw--;
      goto LLgMgRM2;
    }
    goto LMgRM;
  }
  // y -> L
  if ( i < lw ) {
    A[lw] = x;
    x = y;
    lw--;
    goto LLgMgRL;
  }
  // left gap closed
  i++; 
  goto LLMgRM;
    
 LLgMgRM2:
  /*
    |o---]-----------+[-]+-------------[---o|
    N    i          lw   up            j    M
    x -> M
   */
  y = A[up];
  r = compareXY(y, p);
  if ( r < 0 ) {
    A[up] = x;
    x = y;
    up++;
    goto LLgMgRL;
  }
  if ( r == 0 ) {
    if ( up < j ) {
      up++;
      goto LLgMgRM1;
    }
    goto LLgMM;
  }
  // y -> R
  if ( up < j ) {
    A[up] = x;
    x = y;
    up++;
    goto LLgMgRR;
  }
  // right gap closed
  j--; 
  goto LLgMRM;

  // left gap closed
 LLMgRL:  
  /*
    |o---------][------]+-------------[---o|
    N           i       up            j    M
    x -> L
  */
  y = A[up];
  r = compareXY(y, p);
  if ( r < 0 ) {
    A[up] = A[i]; A[i] = y;
    up++; i++;
    goto LLMgRL;
  }
  // if ( y == p ) {
  if ( r == 0 ) { 
    if ( up == M ) {
   /*
    |o---------][-------------------]o|
    N           i                     M
    x -> L
   */
      A[N] = x;
      if ( compareXY(p, AM) <= 0 ) {
	A[M] = AM;
	i--;
	goto FinishL;
      }
      /*
      // if ( p == AM ) { 
      if ( compareXY(p, AM) == 0 ) { 
	A[M] = AM;
	i--;
	goto FinishL;
      }
      */
      // compareXY(AM, p) < 0
      A[M] = A[i]; A[i] = AM;
      goto FinishL;
    }
    up++;
    goto LLMgRL;
  }
  // y -> R
  if ( up < j ) {
    A[up] = A[i]; A[i] = x;
    x = y;
    i++; up++;
    goto LLMgRR;
  }
  // right gap closed also
  /*
    |o---------][------][-----------------o|
    N           i       j                  M
    x -> L
   */
  // up == j; A[j] == y 
  r = compareXY(p, AM);
  if ( r < 0 ) {
    A[N] = x; A[M] = AM;
    j--;
    goto Finish;
  }
  if ( r == 0 ) {
    A[N] = x; A[M] = y; A[j] = AM;
    goto Finish;
  }
  // AM -> L and x -> L
  A[M] = y; A[j] = A[i];
  // if ( x <= AM ) { 
  if ( compareXY(x, AM ) <= 0 ) { 
    A[N] = x; 
    A[i] = AM;
  } else {
    A[N] = AM; 
    A[i] = x;
  }
  i++;
  goto Finish;

 LLMgRM:
  /*
    |o---------][------]+-------------[---o|
    N           i       up            j    M
    x -> M
   */
  y = A[up];
  r = compareXY(y, p);
  if ( r < 0 ) {
    A[up] = A[i]; A[i] = y; 
    i++; up++;
    goto LLMgRM;
  }
  if ( r == 0 ) {
    if ( up == M ) { // R empty 
      /*
    |o---------][-------------------]o|
    N           i                     M
    x -> M
      */
      if ( compareXY(AM, p) < 0 ) {
	A[N] = AM;
	A[M] = x;
	i--;
	goto FinishL;
      }
      /*
      if ( compareXY(p, AM) < 0 ) {
	A[M] = AM;
	i--;
	A[N] = A[i]; A[i] = x;
	i--;
	goto FinishL;
      }
      */
      A[M] = AM;
      i--;
      A[N] = A[i]; A[i] = x;
      i--;
      goto FinishL;
    }
    up++;
    goto LLMgRM;
  }
  // y -> R
  if ( up < j ) {
    A[up] = x;
    x = y;
    up++;
    goto LLMgRR;
  }
  // right gap closed also
  /*
    |o---------][------][-----------------o|
    N           i       j                  M
    x -> M
   */
  // up == j; A[j] == y 
  r = compareXY(p, AM);
  if ( r < 0 ) {
    A[M] = AM;
    i--;
    A[N] = A[i]; A[i] = x;
    j--;
    goto Finish;
  } 
  if ( 0 < r ) {
    A[N] = AM; A[M] = y; A[j] = x;
    goto Finish;
  }
  // AM -> M
  i--;
  A[N] = A[i]; A[i] = x;
  A[M] = y; A[j] = AM;
  goto Finish;

 LLMgRR:
  /*
    |o---------][------]+-------------[---o|
    N           i       up            j    M
    x -> R
  */
  j--;
  y = A[j];
  r = compareXY(p, y);
  if ( r < 0 ) goto LLMgRR;
  if ( 0 < r ) {
    A[j] = x;
    x = y;
    goto LLMgRL;
  }
  // y -> M
  if ( up <= j ) {
    A[j] = x;
    x = y;
    goto LLMgRM;
  }
  // right gap closed also   y = A[j]
  /*
    |o---------][------][-----------------o|
    N           i      j                   M
    x -> R
   */
  A[M] = x;
  r = compareXY(AM, p);
  if ( r < 0 ) {
    A[N] = AM; 
    goto Finish;
  }
  if ( r == 0 ) {
    i--;
    A[N] = A[i]; A[i] = AM;
    goto Finish;
  }
  // AM -> R
  i--;
  A[N] = A[i]; A[i] = y; A[j] = AM;
  j--; 
  goto Finish;

  // right gap closed
 LLgMRR:
  /*
    |o---]-----------+[---------][---------o|
    N    i          lw          j           M
    x -> R
  */
  y = A[lw];
  r = compareXY(p, y);
  if ( r < 0 ) {
    A[lw] = A[j]; A[j] = y;
    lw--; j--;
    goto LLgMRR;    
  }
  if ( r == 0 ) {
    if ( N == lw ) { // L is empty 
      /*
    |o[-----------------------][---------o|
    N                         j           M
    x -> R
      */
      A[M] = x;
      if ( compareXY(AM, p) <= 0 ) {
	A[N] = AM;
	j++;
	goto FinishR;
      }
      /*
      if ( compareXY(p, AM) == 0 ) {
	A[N] = AM;
	j++;
	goto FinishR;
      }
      */
      // compareXY(p, AM) < 0
      A[N] = A[j]; A[j] = AM;
      goto FinishR;
    }
    lw--;
    goto LLgMRR; 
  }
  // y -> L
  if ( i < lw ) {
    A[lw] = A[j]; A[j] = x;
    x = y;
    lw--; j--;
    goto LLgMRL;
  }
  // left gap closed also
  /*
    |o---][---------------------][---------o|
    N    i                      j           M
    x -> R
  */
  // y == A[lw] == A[i]
  A[M] = x;
  r = compareXY(AM, p);
  if ( r < 0 ) {
    A[N] = AM;
    i++;
    goto Finish;
  }
  if ( r == 0 ) {
    A[N] = y; A[i] = AM;
    goto Finish;
  }
  // AM -> R
  A[N] = y; A[i] = A[j]; A[j] = AM;
  j--; 
  goto Finish;

 LLgMRM:
  /*
    |o---]-----------+[---------][---------o|
    N    i          lw          j           M
    x -> M
  */
  y = A[lw];
  r = compareXY(p, y);
  if ( r < 0 ) {
    A[lw] = A[j]; A[j] = y;
    lw--; j--;
    goto LLgMRM;    
  }
  if ( r == 0 ) {
    if ( N == lw ) { // L empty 
      /*
    |o[-----------------------][---------o|
    N                         j           M
    x -> M
      */
      if ( compareXY(AM, p) <= 0 ) {
	A[N] = AM;
	j++;
	A[M] = A[j];
	A[j] = x;
	j++;
	goto FinishR; 
      }
      /*
      // if ( p == AM ) {
      if ( compareXY(p, AM) == 0 ) { 
	A[N] = AM;
	j++;
	A[M] = A[j];
	A[j] = x;
	j++;
	goto FinishR; 
      }
      */
      // compareXY(p, AM) < 0
      A[M] = AM; A[N] = x;
      j++;
      goto FinishR; 
    }
    lw--;
    goto LLgMRM; 
  }
  // y -> L
  if ( i < lw ) {
    A[lw] = x;
    x = y;
    lw--;
    goto LLgMRL;
  }
  // left gap closed also
  /*
    |o---][---------------------][---------o|
    N    i                      j           M
    x -> M
  */
  // y == A[lw] == A[i]
  r = compareXY(p, AM);
  if ( r < 0 ) {
    A[M] = AM; 
    A[N] = y; A[i] = x;
    goto Finish;
  }
  if ( 0 < r ) {
    A[N] = AM; 
    j++;
    A[M] = A[j]; A[j] = x;
    i++;
    goto Finish;
  }
  // AM == p
  A[N] = y; A[i] = x;
  j++;
  A[M] = A[j]; A[j] = AM;
  goto Finish;

 LLgMRL:
  /*
    |o---]-----------+[---------][---------o|
    N    i          lw          j           M
    x -> L
   */
  i++;
  y = A[i];
  r = compareXY(y, p);
  if ( r < 0 ) goto LLgMRL;
  if ( 0 < r ) {
    A[i] = x;
    x = y;
    goto LLgMRR;
  }
  // y -> M
  if ( i <= lw ) {
    A[i] = x;
    x = y;
    goto LLgMRM;  
  }
  // left gap closed also  y = A[i];
   /*
    |o---][----------------------][---------o|
    N     i                      j           M
    x -> L
   */
  A[N] = x;
  r = compareXY(p, AM);
  if ( r < 0 ) { 
    A[M] = AM;
    goto Finish;
  }
  if ( r == 0 ) {
    j++;
    A[M] = A[j]; A[j] = AM;
    goto Finish;
  }
  // AM -> L
  j++;
  A[M] = A[j]; A[j] = y; A[i] = AM;
  i++;
  goto Finish;

 LMgRM:
   /*
    |o[---------------]+-------------[---o|
    N                  up            j    M
    x -> M
   */
  y = A[up];
  r = compareXY(y, p);
  if ( r < 0 ) { // N == i 
    i++;
    A[up] = A[i]; A[i] = y;
    i++; up++;
    goto LLMgRM;
  }
  if ( r == 0 ) {
    if ( up == M ) {
   /*
    |o[----------------------------]o|
    N                                M
    x -> M
   */
      if ( compareXY(AM, p) <= 0 ) {
	A[N] = AM; A[M] = x;
	goto Finish0;
      }
      /*
      if ( compareXY(p, AM) == 0 ) {
	A[N] = AM; A[M] = x;
	goto Finish0;
      }
      */
      // compareXY(p, AM) < 0
      A[N] = x; A[M] = AM;
      goto Finish0;
    }
    up++;
    goto LMgRM;
  }
  // compareXY(p, y ) < 0
  if ( up < j ) {
    A[up] = x;
    x = y;
   /*
    |o[---------------]+-------------[---o|
    N                  up            j    M
    x -> R
   */
    goto LMgRR;
  }
  // right gap closed  y = A[up] = A[j] -> R
  /*
    |o[-----------------------------][---o|
    N                                j    M
    x -> M
   */
  if ( compareXY(AM, p) <= 0 ) {
    A[N] = AM;
    A[M] = y; A[j] = x;
    j++;
    goto FinishR;
  }
  /*
  if ( compareXY(AM, p) < 0 ) {
    A[N] = AM;
    A[M] = A[j]; A[j] = x;
    j++;
    goto FinishR;
  }
  if ( compareXY(p, AM) == 0 ) {
    A[N] = AM;
    A[M] = A[j]; A[j] = x;
    j++;
    goto FinishR;
  }
  */
  // AM -> R
  A[N] = x; A[M] = AM;
  goto FinishR;

 LLgMM:
   /*
    |o---]-----------+[------------------]o|
    N    i          lw                     M
    x -> M
   */
  y = A[lw];
  r = compareXY(p, y);
  if ( r < 0 ) {
    j--;
    A[lw] = A[j]; A[j] = y;
    lw--;
    j--;
    goto LLgMRM;
  }
  if ( r == 0 ) {
    if ( N == lw ) { // L empty also 
      /*
    |o[----------------------------]o|
    N                                M
    x -> M
      */
      if ( compareXY(AM, p) <= 0 ) {
	A[N] = AM; A[M] = x;
	goto Finish0;
      }
      /*
      if ( compareXY(p, AM) == 0 ) {
	A[N] = AM; A[M] = x;
	goto Finish0;
      }
      */
      // AM -> R
      A[N] = x; A[M] = AM;
      goto Finish0;
    }
    lw--;
    goto LLgMM;
  }
  // y -> L
  if ( i < lw ) {
    A[lw] = x;
    x = y;
    lw--;
    goto LLgML;
  }
  // left gap closed also  y = A[lw] = A[i] -> L
   /*
    |o---][-----------------------------]o|
    N    i                                M
    x -> M
   */
  r = compareXY(AM, p);
  if ( r < 0 ) {
    A[N] = AM; A[M] = x;
    goto FinishL;
  }
  if ( r == 0 ) {
    A[N] = y; A[i] = AM; A[M] = x;
    i--;
    goto FinishL;
  }
  // compareXY(p, AM) < 0
  A[M] = AM;
  A[N] = y; A[i] = x;
  i--;
  goto FinishL;

 LLgML:
   /*
    |o---]-----------+[------------------]o|
    N    i          lw                     M=j
    x -> L
   */
  i++;
  y = A[i];
  r = compareXY(y, p);
  if ( r < 0 ) goto LLgML;
  if ( r == 0 ) {
    if ( i <= lw ) {
      A[i] = x;
      x = y;
      goto  LLgMM;
    }
    // left gap closed  y = A[i]
   /*
    |o--------------][------------------]o|
    N                i                    M=j
    x -> L
   */
    A[N] = x;
    if ( compareXY(p, AM) <= 0 ) {
      A[M] = AM;
      i--;
      goto FinishL;
    }
    /*
    // if ( p == AM ) {
    if ( compareXY(p, AM) == 0 ) { 
      A[M] = AM;
      i--;
      goto FinishL;
    }
    */
    // AM -> L
    A[M] = y; A[i] = AM;
    goto FinishL;
  }
  // y -> R
  A[i] = x;
  x = y;
  goto LLgMR;

 LLgMR:
   /*
    |o---]-----------+[------------------]o|
    N    i          lw                     M=j
    x -> R
   */
  y = A[lw];
  r = compareXY(p, y);
  if ( r < 0 ) {
    j--;
    A[lw] = A[j]; A[j] = y;
    j--; lw--;
    goto LLgMRR;
  }
  if ( r == 0 ) {
    if ( N == lw ) { // L is empty 
      /*
    |o[----------------------------]o|
    N                                M=j
    x -> R
      */
     if ( compareXY(AM, p) <= 0 ) { 
	A[N] = AM;
	A[M] = x;
	goto Finish0;
     }
     /*
     if ( compareXY(p, AM) == 0 ) { 
	A[N] = AM;
	A[M] = x;
	goto Finish0;
     }
     */
     // AM -> R
     j--;// = M-1
     A[N] = A[j];
     if ( x <= AM ) {
       A[j] = x; A[M] = AM;
     } else {
       A[j] = AM; A[M] = x;
     }
     goto Finish0;
    }
    lw--;
    goto LLgMR;
  }
  // y -> L
  if ( i < lw ) {
    j--;
    A[lw] = A[j]; A[j] = x;
    x = y;
    lw--; j--;
    goto LLgMRL;
  }
  // left gap closed y = A[lw] = A[i] -> L
 /*
    |o---][----------------------------]o|
    N    i                               M=j
    x -> R
   */
  r = compareXY(AM, p);
  if ( r < 0 ) {
    A[N] = AM; A[M] = x;
    goto FinishL;
  }
  if ( r == 0 ) {
    A[M] = x;
    A[N] = y; A[i] = AM;
    i--;
    goto FinishL;
  }
  // AM -> R
  j--;
  A[N] = y; A[i] = A[j];
  if ( x <= AM ) {
    A[j] = x; A[M] = AM;
  } else {
    A[j] = AM; A[M] = x;
  }
  i--;
  goto FinishL;

 LMgRR:
   /*
    |o[---------------]+-------------[---o|
    N                  up            j    M
    x -> R
   */
  j--;
  y = A[j];
  r = compareXY(p, y);
  if ( r < 0 ) goto LMgRR;
  if ( 0 < r ) {
    A[j] = x;
    x = y;
   /*
    |o[---------------]+-------------[---o|
    N                  up            j    M
    x -> L
   */
    goto LMgRL;
  }
  // y -> M
  if ( up <= j ) {
    A[j] = x;
    x = y;
    goto LMgRM;
  }
  // right gap closed  y = A[j]
  /*
    |o[---------------][----------------o|
    N                 j                  M
    x -> R
   */
  A[M] = x;
  if ( compareXY(AM, p) <= 0 ) {
    A[N] = AM;
    j++;
    goto FinishR;
  }
  /*
  if ( compareXY(p, AM) == 0 ) {
    A[N] = AM;
    j++;
    goto FinishR;
  }
  */
  // AM -> R
  A[N] = A[j]; A[j] = AM;
  goto FinishR;

 LMgRL:
   /*
    |o[---------------]+-------------[---o|
    N                  up            j    M
    x -> L
   */
  y = A[up];
  r = compareXY(y, p);
  if ( r < 0 ) { // create L
    i++;
    A[up] = A[i]; A[i] = y;
    i++; up++;
    goto LLMgRL;    
  }
  if ( r == 0 ) {
    if ( up == M ) { // R is empty 
      /*
    |o[----------------------------]o|
    N                                M
    x -> L
      */
      if ( compareXY(p, AM) <= 0 ) {
	A[N] = x; A[M] = AM;
	goto Finish0;
      }
      /*
      if ( compareXY(p, AM) == 0 ) {
	A[N] = x; A[M] = AM;
	goto Finish0;
      }
      */
      // AM -> L
      i++;
      A[M] = A[i];
      if ( x <= AM ) {
       A[N] = x; A[i] = AM;
      } else {
	A[N] = AM; A[i] = x;
      }
      goto Finish0;
    }
    up++;
    goto LMgRL;
  }
  // y -> R
  if ( up < j ) {
    i++;
    A[up] = A[i]; A[i] = x;
    x = y;
    i++; up++;
    goto LLMgRR;
  }
  // right gap closed   y = A[up] = A[j]
  /*
    |o[-----------------------------][---o|
    N                                j    M
    x -> L
   */
  r = compareXY(p, AM);
  if ( r < 0 ) {
    A[N] = x; A[M] = AM;
    goto FinishR;
  }
  if ( r == 0 ) {
    A[N] = x; 
    A[M] = A[j]; A[j] = AM;
    j++;
    goto FinishR;
  }
  // AM -> L
  i++;
  A[M] = A[j]; A[j] = A[i];
  if ( x <= AM ) {
    A[N] = x; A[i] = AM;
  } else {
    A[N] = AM; A[i] = x;
  }
  j--;
  goto FinishR;

  int k;

 Finish0:
  return;

 FinishL:
    (*cut)(N, i, depthLimit);
  return;

 FinishR:
    (*cut)(j, M, depthLimit);
  return;

 Finish:
  /* holes filled
    |o---------][------][-----------------o|
    N           i      j                   M
  */
  /*
  int k;
  if ( i <= N ) {
    printf("Error i-index\n");
    printf("N: %i i: %i j: %i M: %i\n", N, i, j, M);
    exit(1);
  }
  if ( j < i ) {
    printf("Error i-j-index\n");
    printf("N: %i i: %i j: %i M: %i\n", N, i, j, M);
    exit(1);
  }
  if ( M <= j ) {
    printf("Error j-index\n");
    printf("N: %i i: %i j: %i M: %i\n", N, i, j, M);
    exit(1);
  }
  for (k = N; k < i; k++) 
    // if ( p <= A[k] ) {
    if ( compareXY(p, A[k]) <= 0 ) {
      printf("Error L at k: %d \n", k);
      printf("N: %i i: %i j: %i M: %i\n", N, i, j, M);
      exit(1);
    }
  for (k = i; k <= j; k++) 
    // if ( p != A[k] ) {
    if ( compareXY(p, A[k]) != 0 ){
      printf("Error M at k: %d \n", k);
      printf("N: %d i: %d j: %d M: %d\n", N, i, j, M);
      exit(1);
    }
  for (k = j+1; k <= M; k++) 
    // if ( A[k] <= p ) {
    if ( compareXY(A[k], p) <= 0 ) {
      printf("Error R at k: %d \n", k);
      printf("N: %d i: %d j: %d M: %d\n", N, i, j, M);
      exit(1);
    }
  */
  if ( i - N  < M - j ) {
    (*cut)(N, i-1, depthLimit);
    (*cut)(j+1, M, depthLimit);
  } else {
    (*cut)(j+1, M, depthLimit);
    (*cut)(N, i-1);
  }
} // end of cut3duplicates


void cut4c();
// cut4 is doing 4-partitioning using 3 pivots
void cut4(int N, int M) {
  // printf("cut4 %d %d \n", N, M);
  int L = M - N; 
  // cut4c(N, M, 0); return; // for testing heapsort
  int depthLimit = 2 * floor(log(L));
  cut4c(N, M, depthLimit);
} // end cut4


void cut4c(int N, int M, int depthLimit) 
{
  if ( depthLimit <= 0 ) {
    heapc(A, N, M);
    return;
  }
  // printf("cut4c %d %d  %d\n", N, M, depthLimit);
  int L = M - N; 
  if ( L < cut4Limit ) {
    cut3c(N, M, depthLimit); 
    return; 
  }
  depthLimit--;

  // pivots for left/ middle / right regions
  register void *maxl, *middle, *minr;   

  
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
  cut4(N1, M1); // will likely trigger cut3c, or even quicksort0c

  void *first = A[N1];
  maxl = A[maxlx];
  middle = A[middlex];
  minr = A[minrx];
  void *last = A[M1];

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
  void cut4c();
  if ( 0 <= dupx ) { // there are duplicates 
    cut3duplicates(N, M, dupx, cut4c, depthLimit);
    return;
  }

  // fix the end points
  if ( compareXY(A[N1], A[N]) < 0 ) iswap(N1, N, A);
  if ( compareXY(A[M], A[M1]) < 0 ) iswap(M1, M, A);

  register int i, j, lw, up, z; // indices

  // Here the general layout
   /*   L             ML         MR             R
    |-----]------+[---------]---------]+-----[-----|
    N     i      lw         z         up     j     M
   */

    /* There are invariants to be maintained (which are >essential< 
     for machine assisted correctness proofs):
     maxl < middle < minr
     If there are two gaps:
       N <= x <= i --> A[x] <= maxl
       lw < x <= z --> maxl < A[x] <= middle
       z < x < up --> middle < A[x] < minr
       j <= x <= M --> minr <= A[x]
     If the left gap has closed:
       N <= x < i --> A[x] <= maxl
       i <= x <= z --> maxl < A[x] <= middle
       z <  x < up --> middle < A[x] < minr
       j <= x <= M --> minr <= A[x]
     If the right gap has closed:
       N <= x <= i --> A[x] <= maxl
       lw < x <= z --> maxl < A[x] <= middle
       z < x <= j --> middle < A[x] < minr
       j < x <= M --> minr <= A[x]
  */

  void *x, *y; // values
  int hole;

  /* We employ again whack-a-mole. We know in which partition element x 
     should be.  Find a close, undecided position where x should go.  
     Store its content in y.  Drop x (simplified).  Set x to y and repeat. 
   */
  // Ready to roll ...
  i = N;
  j = M;

start:
         /*   L                           R
	   |-----]---------------------[-----|
	   N     i                     j     M
	 */
        i++;
	x = A[i];
	// if ( x <= maxl ) goto start;
	if ( compareXY(x, maxl) <= 0 ) goto start;
	A[i] = A[N];
	hole = N;
	// if ( minr <= x ) goto right;
	if ( compareXY(minr, x) <= 0 ) goto right;
	goto createMiddle;

 right:
         /*   L                           R
	   |o----]---------------------[-----|
	   N     i                     j     M
	   x -> R
	 */
	j--;
	y = A[j];
	// if ( minr <= y ) goto right;
	if ( compareXY(minr, y) <= 0 ) goto right;
	// if ( maxl < y ) { 
	if ( compareXY(maxl, y) < 0 ) {
	  A[j] = x;
	  x = y;
	  goto createMiddle;
	}
	// y -> L 
	if ( j <= i ) {
	  A[hole] = A[i];
	  A[i] = x;
	  if ( i - N < M - j) {
	    cut4c(N, i-1, depthLimit);
	    cut4c(i, M, depthLimit);
	    goto Finish0;
	  }
	  cut4c(i, M, depthLimit);
	  cut4c(N, i-1, depthLimit);
	  goto Finish0;
	}
	// i < j
	A[j] = x;
	x = y;
	// goto left;

 left:
         /*   L                           R
	   |o----]---------------------[-----|
	   N     i                     j     M
	   x -> L
	 */
	i++;
	y = A[i];
	// if ( y <= maxl ) goto left;
	if ( compareXY(y, maxl) <= 0 ) goto left;
	// if ( y < minr ) { 
	if ( compareXY(y, minr) < 0 ) {
	  A[i] = x;
	  x = y;
	  goto createMiddle;
	}
	// y -> R but check whether gap closed
	if ( i < j ) {
	  A[i] = x;
	  x = y;
	  goto right;
	}
	// gap closed
	A[hole] = x;
	if ( i - N < M - j) {
	  cut4c(N, i-1, depthLimit);
	  cut4c(i, M, depthLimit);
	  goto Finish0;
	}
	cut4c(i, M, depthLimit);
	cut4c(N, i-1, depthLimit);
	goto Finish0;

 createMiddle:
         /*   L                           R
	   |o----]---------------------[-----|
	   N     i                     j     M
	   x -> M
	 */
	lw = (i+j)/2;
	up = lw+1;
	y = A[lw];
	A[lw] = x;
	// if ( x <= middle) { 
	if ( compareXY(x, middle) <= 0 ) {
	  z = lw; lw--; } // ML initialized
         /*   L                           R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
                              z
	 */
	else { lw--; z = lw; } // MR initialized
         /*   L                           R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
	                   z
	 */
	x = y;

	if ( lw < z ) { // ML initialized
	  // if ( x <= middle ) {
	  if ( compareXY(x, middle) <= 0 ) {
	    // if ( x <= maxl ) { 
	    if ( compareXY(x, maxl) <= 0 ) goto TLgMLgRL; 
	    goto TLgMLgRML;
	  }
	  // if ( x < minr ) { 
	  if ( compareXY(x, minr) < 0 ) goto TLgMLgRMR;
	  goto TLgMLgRR;
	}
	// MR initialized
	if ( compareXY(x, middle) <= 0 ) {
	  // if ( x <= maxl ) goto TLgMRgRL;
	  if ( compareXY(x, maxl) <= 0 ) goto TLgMRgRL;
	  goto TLgMRgRML;
	}
	// if ( x < minr ) goto TLgMRgRMR;
	if ( compareXY(x, minr) < 0 ) goto TLgMRgRMR;
	goto TLgMRgRR;

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

	// empty ML
 TLgMRgRL:
         /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> L          z
	 */
	i++;
	y = A[i];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) goto TLgMRgRL;
	  // y -> ML
	  A[i] = x;
	  x = y;
	  goto TLgMRgRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  A[i] = x;
	  x = y;
	  goto TLgMRgRR;
	}
	// y -> MR
	if ( i <= lw ) {
	  A[i] = x;
	  x = y;
	  goto TLgMRgRMR;
	}
	// left gap closed, empty ML
	 /*   L                           R
	   |o-------------][-]+--------[-----|
	   N               i  up       j     M
           x -> L         z
	 */
	goto TLMRgRL; 

	// empty ML
 TLgMRgRML:
         /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i         lw  up       j     M
           x -> ML         z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) { // create ML!
	      A[lw] = x;
	      x = y;
	      lw--;
	      goto TLgMLMRgRL;
	    }
	    // i == lw, left gap closed and empty ML
	    i++; 
         /*   L              MR            R
	   |o--------------][-]+--------[-----|
	   N                i  up       j     M
           x -> ML         z
	 */
	    goto TLMRgRML;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLMRgRML;
	}
	// middle < y
	// create ML !
	A[lw] = x;
	x = y;
	lw--;
	if ( compareXY(minr, y) <= 0 ) { 
	  goto TLgMLMRgRR;
	}
	goto TLgMLMRgRMR; 

	// empty ML
 TLgMRgRMR:
         /*   L                           R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> MR         z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    A[up] = x;
 	    x = y;
	    up++;
         /*   L                           R
	   |o----]---------+[-]+--------[-----|
	   N     i         lw up        j     M
           x -> L          z
	 */
	    goto TLgMRgRL;
	  }
	  // y -> ML
	    A[up] = x;
 	    x = y;
	    up++;
         /*   L                           R
	   |o----]---------+[-]+--------[-----|
	   N     i         lw up        j     M
           x -> ML         z
	 */
	    goto TLgMRgRML;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) { 
	  up++; 
	  goto TLgMRgRMR; 
	}
	// y -> R ?
	if ( up < j ) {
	  A[up] = x;
	  x = y;
	  goto TLgMRgRR;
	}
	// right gap closed with empty ML
	j--;
         /*   L                           R
	   |o----]---------+[---------][-----|
	   N     i         lw         j      M
           x -> MR         z
	 */
	goto TLgMRRMR;

	// empty ML
 TLgMRgRR: 
         /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i         lw  up       j     M
           x -> R          z
	 */
	j--;
	y = A[j];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    A[j] = x;
	    x = y;
	    goto TLgMRgRL;
	  }
	  // y -> ML
	 /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i         lw  up       j     M
           x -> ML         z
	 */
	  A[j] = x;
	  x = y;
	  goto TLgMRgRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) goto TLgMRgRR;
	// y -> MR
	if ( up <= j ) {
	  A[j] = x;
	  x = y;
	  goto TLgMRgRMR;
	}
	// right gap closed, empty ML
         /*   L                           R
	   |o----]---------+[---------][-----|
	   N     i        lw          j      M
           x -> R          z
	 */
	goto TLgMRRR;

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

	// right gap closed with empty ML
 TLgMRRR:
         /*   L                           R
	   |o----]---------+[---------][-----|
	   N     i        lw          j      M
           x -> R          z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) { // y -> L
	    if ( i < lw ) {
	      A[lw] = A[j];
	      A[j] = x;
	      j--;
	      x = y;
	      lw--; z = lw;
         /*   L                           R
	   |o----]---------+[---------][-----|
	   N     i        lw          j     M
           x -> L          z
	 */
	      goto TLgMRRL;
	    }
	    // left gap closed
	    A[hole] = A[i];
	    A[i] = A[j];
	    A[j] = x;
	    j--;
	    goto Finish3;
	  }
	  // y -> ML !
	  lw--;
         /*   L                           R
	   |o----]---------+[--|------][-----|
	   N     i        lw          j     M
           x -> R              z
	 */
	  goto TLgMLMRRR;
	}
	// middle < y; shift MR to the left
	if ( compareXY(minr, y) <= 0 ) { // y -> R
	  A[lw] = A[j];
	  A[j] = y;
	  j--; lw--; z = lw;
	  goto TLgMRRR;
	}
	// y -> MR
	lw--; z = lw;
	goto TLgMRRR;

	// right gap closed with empty ML
 TLgMRRL:
         /*   L                           R
	   |o----]---------+[---------][-----|
	   N     i        lw          j      M
           x -> L          z
	 */
	i++;
	y = A[i];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) goto TLgMRRL;
	  // y -> ML!
	  A[i] = x;
	  x = y;
         /*   L                           R
	   |o----]---------+[---------][-----|
	   N     i        lw          j      M
           x -> ML         z
	 */
	  goto TLgMRRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  A[i] = x;
	  x = y;
	  goto TLgMRRR;
	}
	// y -> MR
	if ( i <= lw ) {
	  A[i] = x;
	  x = y;
         /*   L                           R
	   |o----]---------+[---------][-----|
	   N     i        lw          j      M
           x -> MR         z
	 */
	  goto TLgMRRMR;
	}
	// left gap closed
	A[hole] = x;
	goto Finish3;

	// right gap closed, empty ML
 TLgMRRML:
         /*   L                           R
	   |o----]---------+[---------][-----|
	   N     i         lw         j      M
           x -> ML         z
	 */
	// going to create ML
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = x;
	      x = y;
	      lw--;
	      goto TLgMLMRRL;
	    }
	    // left gap closed also
	    // create ML with one element in it ...
	    A[hole] = y; // = A[i]
	    A[i] = x;
	    i--;
	    j++;
	    goto Finish4;
	  }
	  // y -> ML
	  // create ML
	  lw--;
         /*   L                        R
	   |o----]------+[--|------][-----|
	   N     i      lw         j      M
           x -> ML          z
	 */
	  goto TLgMLMRRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  A[lw] = A[j];
	  A[j] = y;
	  j--; lw--; z = lw;
	  goto TLgMRRML;
	}
	// y -> MR
	lw--; z = lw;
	goto TLgMRRML;

	// right gap closed, empty ML
 TLgMRRMR:
         /*   L                           R
	   |o----]---------+[---------][-----|
	   N     i        lw          j      M
           x -> MR         z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = x;
	      x = y;
	      lw--; z = lw;
	      goto TLgMRRL;
	    }
	    // left gap closed also
	    A[hole] = y; // = A[i]
	    A[i] = x;
	    goto Finish3;
	  }
	  // y -> ML; create ML
	  lw--;
	  goto TLgMLMRRMR;
	}
	// middle < y
	z--;
	if ( compareXY(minr, y) <= 0 ) { // y -> R
	  A[lw] = A[j];
	  A[j] = y;
	  j--; lw = z;
	  goto TLgMRRMR;
	}
	// y -> MR
	lw = z;
	goto TLgMRRMR;

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

	// left gap closed, empty ML
 TLMRgRML:
         /*   L              MR            R
	   |o--------------][-]+--------[-----|
	   N                i  up       j     M
           x -> ML         z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    A[up] = A[i]; A[i] = y;
	    z = i; i++; up++;
	    goto TLMRgRML;
	  }
	  // y -> ML !
	  A[up] = A[i]; A[i] = y;
	  z = i; up++;
	  goto TLMLMRgRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  if ( up < j ) { // y -> R; create ML 
	    A[up] = A[i]; A[i] = x;
	    x = y;
	    z = i; up++;
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
	   N      i              up       j     M
           x -> R             z
	 */
	    goto TLMLMRgRR;
	  }
	  // right gap closed; create 1-element ML
	  A[hole] = A[z]; A[z] = x;
	  i = z-1;
	  goto Finish4;
	}
	// y -> MR
	up++;
	goto TLMRgRML;

	// left gap closed, empty ML
 TLMRgRL:
	 /*   L             MR            R
	   |o-------------][-]+--------[-----|
	   N               i  up       j     M
           x -> L         z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    A[up] = A[i]; A[i] = y;
	    z = i; i++; up++;
	    goto TLMRgRL;
	  }
	  // y -> ML, create ML
	  A[up] = A[i]; A[i] = y;
	  z = i; up++;
	  goto TLMLMRgRL;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLMRgRL;
	}
	// y -> R
	if ( up < j ) {
	  A[up] = A[i]; A[i] = x;
	  x = y;
	  z = i; i++; up++;
	 /*   L             MR            R
	   |o-------------][-]+--------[-----|
	   N               i  up       j     M
           x -> R         z
	 */
	  goto TLMRgRR;
	}
	// right gap closed;
	A[hole] = x;
	j--;
	goto Finish3;

	// left gap closed, empty ML
 TLMRgRR:
	 /*   L             MR            R
	   |o-------------][-]+--------[-----|
	   N               i  up       j     M
           x -> R         z
	 */
	j--;
	y = A[j];
	if ( compareXY(y, middle) <= 0 ) {
	  A[j] = x;
	  x = y;
	  if ( compareXY(y, maxl) <= 0 ) goto TLMRgRL;
	  // y -> ML !
	  goto TLMRgRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) goto TLMRgRR;
	// y -> MR
	if ( up <= j ) {
	  A[j] = x;
	  x = y;
	 /*   L             MR            R
	   |o-------------][-]+--------[-----|
	   N               i  up       j     M
           x -> MR        z
	 */
	  goto TLMRgRMR;
	}
	// right gap closed also, with empty ML
	A[hole] = A[z]; A[z] = y; A[j] = x;
	i = z;  j--;
	goto Finish3;

	// left gap closed, empty ML
 TLMRgRMR:
	 /*   L             MR            R
	   |o-------------][-]+--------[-----|
	   N               i  up       j     M
           x -> MR        z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    A[up] = A[i]; A[i] = y;
	    z = i; i++; up++;
	    goto TLMRgRMR;
	  }
	  // y -> ML, create ML
	  A[up] = A[i]; A[i] = y;
	  z = i; up++;
	  goto TLMLMRgRMR;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLMRgRMR;
	}
	// y --> R
	if ( up < j ) {
	  A[up] = x;
	  x = y;
	  up++;
	  goto TLMRgRR;
	}
	// right gap closed, empty ML
	A[hole] = A[z]; A[z] = x;
	i = z;  j--;
	goto Finish3;

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
	// printf("cut4 exit Finish0 N: %d M: %d\n", N, M);
	return;

	// +++++++++++++++both gaps closed+++++++++++++++++++
	int lleft, lmiddle, lright;
	// int k; // for testing
 Finish3:
	// printf("cut4 exit Finish3 N: %d M: %d\n", N, M);

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
	    cut4c(N, i-1, depthLimit);
	    if ( lmiddle < lright ) { // L < M < R
	      cut4c(i, j, depthLimit);
	      cut4c(j+1, M, depthLimit);
	    } else {                  // L < R < M     
	      cut4c(j+1, M, depthLimit);
	      cut4c(i, j, depthLimit);
	    }
	    return;
	  } else {                    // R < L < M 
	    cut4c(j+1, M, depthLimit);
	    cut4c(N, i-1, depthLimit);
	    cut4c(i, j, depthLimit);
	    return;
	  }
	}                             // M < L
	if ( lmiddle < lright ) {     // M < R
	  cut4c(i, j, depthLimit);
	  if ( lleft < lright) {
	     cut4c(N, i-1, depthLimit);
	     cut4c(j+1, M, depthLimit);
	  } else {
	    cut4c(j+1, M, depthLimit);
	    cut4c(N, i-1, depthLimit);
	  }
	  return;
	}                             // R < M < L
	cut4c(j+1, M, depthLimit);
	cut4c(i, j, depthLimit);
	cut4c(N, i-1, depthLimit);
	return;

 Finish4:
	// printf("cut4 exit Finish4 N: %d M: %d\n", N, M);
         /*   L        ML         MR         R
	   |-----][----------|-----------][-----|
	   N     i           z            j     M
	 */
	if ( z-N < M-z ) {
	  cut4c(N, i, depthLimit);
	  cut4c(i+1, z, depthLimit);
	  if ( j-z < M-j ) {
	    cut4c(z+1, j-1, depthLimit);
	    cut4c(j, M, depthLimit);
	    return;
	  }
	  cut4c(j, M, depthLimit);
	  cut4c(z+1, j-1, depthLimit);
	  return;
	}
	// M-z <= z-N
	cut4c(z+1, j-1, depthLimit);
	cut4c(j, M, depthLimit);
	if ( i-N < z-i ) {
	  cut4c(N, i, depthLimit);
	  cut4c(i+1, z, depthLimit);
	  return;
	}
	cut4c(i+1, z, depthLimit);
	cut4c(N, i, depthLimit);
} // end cut4c

void heapSort();
void heapc(void **A, int N, int M) {
  // printf("heapc: %d %d\n", N, M);
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


void sixsort(void **AA, int size, 
	int (*compar ) (const void *, const void * ) ) {
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
  cut4(0, size-1);
}
/* compile with: 
   gcc -c SixSort.c
   which produces:
   SixSort.o
   Compile & link with UseSixSort:
   gcc UseSixSort.c SixSort.o
*/
