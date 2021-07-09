// c:/bsd/rigel/sort/Qusort.c
// Date: Fri Jan 31 13:32:12 2014/ Tue May 19 15:02:00 2015, 2017
// Fri Nov 27 20:13:42 2020
// (C) OntoOO/ Dennis de Champeaux

/*
#include "Hsort.c"
#include "Dsort.c"
#include "Isort.c"
*/

void quicksort0c(void **, int, int, int, int (*)(const void*, const void*));

// calculate the median of 3
int medq0(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end med

void vswap(void **A, int lo, int lo3, int eq) {
  void *t;
  while ( 0 < eq ) { eq--; t = A[lo]; A[lo++] = A[lo3]; A[lo3++] = t; }
}

const int small = 400;

void quicksort0(void **A, int lo, int hi, int (*compare)(const void*, const void*)) {
  // printf("quicksort0 lo %i hi %i L %i\n", lo, hi, hi-lo);
  int L = hi - lo;
  if ( L <= 0 ) return;
  if ( L < 7 ) { 
    insertionsort(A, lo, hi, compare);
    return;
  }
  int depthLimit = 2.9 * floor(log(L));
  quicksort0c(A, lo, hi, depthLimit, compare);
} // end quicksort0

// Quicksort equipped with a defense against quadratic explosion;
// calling heapsort if depthlimit exhausted

void quicksort0c(void **A, int lo, int hi, int depthLimit, 
		 int (*compareXY)(const void*, const void*)) {
  // printf("Enter quicksort0c lo: %d hi: %d %d\n", lo, hi, depthLimit);
  // printf(" gap %d \n", hi-lo);
  while ( lo < hi ) {
    // printf("quicksort0c lo: %d hi %d  L %i\n", lo, hi, hi-lo);
    int L = 1 + hi - lo;
    // if ( L < 8 ) {
    if ( L < 9) {
      insertionsort(A, lo, hi, compareXY);
      return;
    }
    if ( depthLimit <= 0 ) {
      heapc(A, lo, hi, compareXY);
      return;
    }
    depthLimit--;

    // 7 <= L
    int p0 = lo + (L>>1); // lo + L/2;
    if ( 7 < L ) {
      int pn = lo;
      int pm = hi;
      // if ( 51 < L ) {
      if ( 40 < L ) {
	int d = (L-2)>>3; // L/8;
	pn = medq0(A, pn, pn + d, pn + 2 * d, compareXY);
	p0 = medq0(A, p0 - d, p0, p0 + d, compareXY);
	pm = medq0(A, pm - 2 * d, pm - d, pm, compareXY);
      }
      p0 = medq0(A, pn, p0, pm, compareXY);
    }

    /* optional check when inputs have many equal elements
    if ( compareXY(A[lo], A[hi]) == 0 ) {
      dflgm(A, lo, hi, p0, quicksort0c, depthLimit, compareXY);
      return;
    } */

    // p0 is index to 'best' pivot ...    
    iswap(lo, p0, A); // ... and is put in first position

    register void *T = A[lo]; // pivot
    register int I, J; // indices
    register void *AI, *AJ; // array values

    if ( L < small ) { 
      // This is a B&M variant
      I = lo+1; J = hi; 
      int lo2 = I, hi2 = J, l, r, eql, eqr;
    Left2:
      while ( I <= J && (r = compareXY(A[I], T)) <= 0 ) {
	  if ( 0 == r ) { iswap(lo2, I, A); lo2++; }
	  I++;
      }
      while ( I <= J && (r = compareXY(A[J], T)) >= 0 ) {
	if ( 0 == r ) { iswap(hi2, J, A); hi2--; }
	J--;
      }
      if ( I > J ) goto Skip2; 
      iswap(I, J, A);
      I++; J--;
      goto Left2;

  Skip2:
      // printf("lo %i i %i j %i hi %i\n",lo,I,J,hi);
      l = lo2-lo; r = I-lo2;
      eql = ( l < r ? l : r );
      vswap(A, lo, I-eql, eql); 
      int hi3 = J+lo-lo2;
      l = hi2-J; r = hi-hi2;
      eqr = ( l < r ? l : r );
      vswap(A, I, hi-eqr+1, eqr);
      int lo3 = I + (hi-hi2);
      int left = hi3-lo;
      int right = hi-lo3;
      if ( left <= right) {
	if ( 0 < left ) quicksort0c(A, lo, hi3, depthLimit, compareXY);
	lo = lo3;
	if ( lo < hi ) { continue; }
	return;
      }
      if ( 0 < right ) quicksort0c(A, lo3, hi, depthLimit, compareXY);
      hi = hi3;
      if ( lo < hi ) { continue; }
      return;
    }
    // ------------------------------------------------------
    // 1st round of partitioning for larger segments
    // Minimizing comparisons is sacrificed for faster loops 
	// The left segment has elements <= T
	// The right segment has elements > T
    /*
	  |----------]-------------[-----------|
	  lo  <=T    I             J   >T      hi   
    */
    J = hi+1;
    while ( compareXY(T, A[--J]) < 0 );
    if ( lo == J ) { // poor pivot  lo < x -> T < A[x], suspect bad input
      int px =  lo + (L>>1); // lo + L/2;
      iswap(lo, px, A);
      dflgm(A, lo, hi, px, quicksort0c, depthLimit, compareXY);
      return;
    }
    AJ = A[J]; // A[J] <= T

    // lo < J <= hi  
    I = lo;
    if ( J < hi ) while ( compareXY(A[++I], T) <= 0 ); 
    else { // J = hi
      if ( compareXY(T, A[hi]) == 0 ) { // bail out
	int px =  lo + (L>>1); // lo + L/2;
	iswap(lo, px, A);
	dflgm(A, lo, hi, px, quicksort0c, depthLimit, compareXY);
	return;
      }
      while ( I < J && compareXY(A[I], T) <= 0 ) { I++; }
      if ( hi == I ) { // all elements are <= T, suspect bad input
	int px =  lo + (L>>1); // lo + L/2;
	iswap(lo, px, A);
	dflgm(A, lo, hi, px, quicksort0c, depthLimit, compareXY);
	return;
      }
    }

  if ( I < J ) {
    // iswap(i, j, A);
    A[J] = A[I]; A[I] = AJ;
  } else goto Skip;

  Left: 
    /*
	  |----------]-------------[-----------|
	  lo  <=T    I             J   >T      hi   
    */
    while ( compareXY(A[++I],  T) <= 0 ); 
    if ( J <= I ) goto Skip;
    AI = A[I];
    while ( compareXY(T, A[--J]) < 0 ); 
    if ( J <= I ) goto Skip;
    AJ = A[J];
    A[I] = AJ; A[J] = AI;
    goto Left;
    /*
    if ( I < J ) { // swap
      A[I] = AJ; A[J] = AI;
      goto Left;
    }
    */
 Skip:

    /*
  {
    printf("Skip ----- lo %i hi %i  L %i\n", lo,hi, hi-lo);
    printf("Skip i %i j %i \n", I, J);
    int k;
    for ( k = J-1; k <= I+1; k++)
      printf("k %i  %i ", k, compareXY(T, A[k]));
    printf("\n\n");
  }
  //  */

    // Tail iteration
    J = I-1;
    iswap(lo, J, A); // put the pivot
    if ( (I - lo) < (hi - J) ) { // smallest one first
      if ( lo < J-1 ) quicksort0c(A, lo, J-1, depthLimit, compareXY);
      lo = I; 
    } else {
      if (I < hi ) quicksort0c(A, I, hi, depthLimit, compareXY);
      hi = J-1;
    }
    /*
    if ( (I - lo) < (hi - J) ) { // smallest one first
      quicksort0c(A, lo, J, depthLimit, compareXY);
      lo = I; 
    } else {
      quicksort0c(A, I, hi, depthLimit, compareXY);
      hi = J;
    }
    */
  } // end of while loop
} // end of quicksort0c

