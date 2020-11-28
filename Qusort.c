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
int med(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end med

void vswap(void **A, int N, int N3, int eq) {
  void *t;
  while ( 0 < eq ) { eq--; t = A[N]; A[N++] = A[N3]; A[N3++] = t; }
}

const int small = 400;

void quicksort0(void **A, int N, int M, int (*compare)(const void*, const void*)) {
  // printf("quicksort0 N %i M %i L %i\n", N, M, M-N);
  int L = M - N;
  if ( L <= 0 ) return;
  if ( L < 7 ) { 
    insertionsort(A, N, M, compare);
    return;
  }
  int depthLimit = 2.5 * floor(log(L));
  quicksort0c(A, N, M, depthLimit, compare);
} // end quicksort0

// Quicksort equipped with a defense against quadratic explosion;
// calling heapsort if depthlimit exhausted

void quicksort0c(void **A, int N, int M, int depthLimit, 
		 int (*compareXY)(const void*, const void*)) {
  // printf("Enter quicksort0c N: %d M: %d %d\n", N, M, depthLimit);
  // printf(" gap %d \n", M-N);
  while ( N < M ) {
    // printf("quicksort0c N: %d M %d  L %i\n", N, M, M-N);
    int L = 1 + M - N;
    // if ( L < 8 ) {
    if ( L < 12) {
      insertionsort(A, N, M, compareXY);
      return;
    }
    if ( depthLimit <= 0 ) {
      heapc(A, N, M, compareXY);
      return;
    }
    depthLimit--;

    // 7 <= L
    int p0 = N + (L>>1); // N + L/2;
    if ( 7 < L ) {
      int pn = N;
      int pm = M;
      // if ( 51 < L ) {
      if ( 40 < L ) {
	int d = (L-2)>>3; // L/8;
	pn = med(A, pn, pn + d, pn + 2 * d, compareXY);
	p0 = med(A, p0 - d, p0, p0 + d, compareXY);
	pm = med(A, pm - 2 * d, pm - d, pm, compareXY);
      }
      p0 = med(A, pn, p0, pm, compareXY);
    }

    /* optional check when inputs have many equal elements
    if ( compareXY(A[N], A[M]) == 0 ) {
      dflgm(A, N, M, p0, quicksort0c, depthLimit, compareXY);
      return;
    } */

    // p0 is index to 'best' pivot ...    
    iswap(N, p0, A); // ... and is put in first position

    register void *T = A[N]; // pivot
    register int I, J; // indices
    register void *AI, *AJ; // array values

    if ( L < small ) { 
      // This is a B&M variant
      I = N+1; J = M; 
      int N2 = I, M2 = J, l, r, eql, eqr;
    Left2:
      while ( I <= J && (r = compareXY(A[I], T)) <= 0 ) {
	  if ( 0 == r ) { iswap(N2, I, A); N2++; }
	  I++;
      }
      while ( I <= J && (r = compareXY(A[J], T)) >= 0 ) {
	if ( 0 == r ) { iswap(M2, J, A); M2--; }
	J--;
      }
      if ( I > J ) goto Skip2; 
      iswap(I, J, A);
      I++; J--;
      goto Left2;

  Skip2:
      // printf("N %i i %i j %i M %i\n",N,I,J,M);
      l = N2-N; r = I-N2;
      eql = ( l < r ? l : r );
      vswap(A, N, I-eql, eql); 
      int M3 = J+N-N2;
      l = M2-J; r = M-M2;
      eqr = ( l < r ? l : r );
      vswap(A, I, M-eqr+1, eqr);
      int N3 = I + (M-M2);
      int left = M3-N;
      int right = M-N3;
      if ( left <= right) {
	if ( 0 < left ) quicksort0c(A, N, M3, depthLimit, compareXY);
	N = N3;
	if ( N < M ) { continue; }
	return;
      }
      if ( 0 < right ) quicksort0c(A, N3, M, depthLimit, compareXY);
      M = M3;
      if ( N < M ) { continue; }
      return;
    }
    // ------------------------------------------------------
    // 1st round of partitioning for larger segments
    // Minimizing comparisons is sacrificed for faster loops 
	// The left segment has elements <= T
	// The right segment has elements > T
    /*
	  |----------]-------------[-----------|
	  N   <=T    I             J   >T      M   
    */
    J = M+1;
    while ( compareXY(T, A[--J]) < 0 );
    if ( N == J ) { // poor pivot  N < x -> T < A[x], suspect bad input
      int px =  N + (L>>1); // N + L/2;
      iswap(N, px, A);
      dflgm(A, N, M, px, quicksort0c, depthLimit, compareXY);
      return;
    }
    AJ = A[J]; // A[J] <= T

    // N < J <= M  
    I = N;
    if ( J < M ) while ( compareXY(A[++I], T) <= 0 ); 
    else { // J = M
      if ( compareXY(T, A[M]) == 0 ) { // bail out
	int px =  N + (L>>1); // N + L/2;
	iswap(N, px, A);
	dflgm(A, N, M, px, quicksort0c, depthLimit, compareXY);
	return;
      }
      while ( I < J && compareXY(A[I], T) <= 0 ) { I++; }
      if ( M == I ) { // all elements are <= T, suspect bad input
	int px =  N + (L>>1); // N + L/2;
	iswap(N, px, A);
	dflgm(A, N, M, px, quicksort0c, depthLimit, compareXY);
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
	  N   <=T    I             J   >T      M   
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
    printf("Skip ----- N %i M %i  L %i\n", N,M, M-N);
    printf("Skip i %i j %i \n", I, J);
    int k;
    for ( k = J-1; k <= I+1; k++)
      printf("k %i  %i ", k, compareXY(T, A[k]));
    printf("\n\n");
  }
  //  */

    // Tail iteration
    J = I-1;
    iswap(N, J, A); // put the pivot
    if ( (I - N) < (M - J) ) { // smallest one first
      if ( N < J-1 ) quicksort0c(A, N, J-1, depthLimit, compareXY);
      N = I; 
    } else {
      if (I < M ) quicksort0c(A, I, M, depthLimit, compareXY);
      M = J-1;
    }
    /*
    if ( (I - N) < (M - J) ) { // smallest one first
      quicksort0c(A, N, J, depthLimit, compareXY);
      N = I; 
    } else {
      quicksort0c(A, I, M, depthLimit, compareXY);
      M = J;
    }
    */
  } // end of while loop
} // end of quicksort0c

