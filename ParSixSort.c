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

const int cut2Limit = 127;
const int cut3Limit = 250;
const int cut4Limit = 375; 
const int probeParamCut4 = 1000000;

char* expiration = "*** License for sixsort has expired ...\n";

// Here more global entities used throughout
int (*compareXY)();
void **A;
int sleepingThreads = 0;
int NUMTHREADS;

#include "Isort"
#include "Hsort"
#include "Qusort"
#include "Dsort"
#include "C2sort"


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
  int depthLimit = 2.5 * floor(log(L));
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
  if ( probeLng < 20 ) probeLng = 20; // quite short indeed
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

int partitionLeft(int N, int M) { 
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
  int n = getN(tx);
  int m = getM(tx);
  // int T = getDL(tx);
  int ix = partitionLeft(n, m);
  setN(tx, ix);
} // end partitionThreadLeft

int partitionRight(int N, int M) { 
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
  int n = getN(tx);
  int m = getM(tx);
  // int T = getDL(tx);
  int ix = partitionRight(n, m);
  setN(tx, ix);
} // end partitionThreadRight

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
  if ( size <= cut2SLimit || numberOfThreads <= 1) {
    cut2(0, size-1);
    return;
  }
  sleepingThreads = 0;
  NUMTHREADS = numberOfThreads;
  pthread_t thread_id[NUMTHREADS];
  ll = newStack();
  int depthLimit = 2.5 * floor(log(size));
  // Try doing the first partition in parallel with two threads
  int N = 0; int M = size-1; // int L = M-N;

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
    struct task *t = newTask(0, size-1, depthLimit);
    addTaskSynchronized(ll, t);
  } else {
    /*
      |------------------------|------------------------|
      N                        e3                       M
      A[N] < T             A[e3] = T                 T<=A[M]
    */

    struct task *t1 = newTask(N, e3, 0);
    struct task *t2 = newTask(e3, M, 0);
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
	for ( k = 0; k < b; k++ ) iswap(e3-k, i2-k, A);
	m3 = i2 - b;
      }
      else {
	// printf("c < b\n");
	for ( k = 0; k < c+1; k++ ) iswap(middle2+k, i1+1+k, A);
	m3 = i1 + c+1;
      }
    }
    /*
      |------------------------][----------------------------|
      N           <           m3           >=                M
     */
    t1 = newTask(N, m3, depthLimit);
    addTaskSynchronized(ll, t1);
    t1 = newTask(m3+1, M, depthLimit);
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
