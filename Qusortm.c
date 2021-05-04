// c:/bsd/rigel/sort/Qusortm.c
// Date: Fri Jan 31 13:32:12 2014/ Tue May 19 15:02:00 2015, 2017
// Fri Nov 27 20:13:42 2020/ Fri Apr 23 21:56:51 2021/ 
// (C) OntoOO/ Dennis de Champeaux

// This version combines B&M + fast loop ++ mergesort

/*
#include "Hsort.c"
#include "Dsort.c"
#include "Isort.c"
*/
void mergeSort2c();
void merge2();
void checkMS2();
const int mergeLimit = 150;
const int iLimit = 9;

void mergeSortm(void **A, int N, int M, int (*compare)()) {
  void *B[mergeLimit];
  mergeSort2c(A, B, N, M, compare);
}
void mergeSort2c(void **A, void **B, int N, int M, int (*compare)()) {
  // printf("IN mergeSort2c N: %i M %i\n", N, M);
	int L = M-N;
	if ( L <= iLimit ) { 
	  insertionsort(A, N, M, compare); 
	  return; 
	}
	int k = L>>1; // L/2;
	mergeSort2c(A, B, N, N+k, compare); 
	// checkMS2(A, N, N+k, compare);
	mergeSort2c(A, B, N+k+1, M, compare);
	// checkMS2(A, N+k+1, M, compare);
	if ( compare(A[N+k], A[N+k+1]) > 0 )
	  merge2(A, B, N, M, compare);
	// checkMS2(A, N, M, compare);
	// printf("OUT mergeSort2c N: %i M %i\n", N, M);
    } // end mergeSort2c

void checkMS2(void **A, int N, int M, int (*compare)()) {
  printf("checkMS2 N: %i M %i\n", N, M);
  int i;
  for ( i = N+1; i <= M; i++)
    if ( compare(A[i], A[i-1]) < 0 ) {
      printf("mergesort check N: %i M: %i err i: %i\n", N,M,i);
      exit(0);
    }
}

void merge2(void **A, void **B, int N, int M, int (*compare)()) {
  // printf("merge2 N: %i M %i\n", N, M);
	int L = M-N;
	int k = L>>1; // L/2;
	int i = N; int j = N+k+1; int z = 0; // int z = N;
	int leftDone = 0;
	int rightDone = 0;
	void *ai = A[i]; void *aj = A[j];
	while (1) {
	  // while ( ai <= aj ) {
	  while ( compare(ai, aj) <= 0 ) {
	    B[z] = ai; z++;
	    if ( i < N+k ) { i++; ai = A[i]; } else {
	      leftDone = 1; break;
	    }
	  }
	  if ( leftDone ) break;
	  // aj < ai
	  B[z] = aj; z++;
	  if ( M == j ) { rightDone = 1; break; }
	  j++; aj = A[j];
	  // while ( aj < ai ) {
	  while ( compare(aj, ai) < 0 ) {
	    B[z] = aj; z++;
	    if ( j < M ) { j++; aj = A[j]; } else {
	      rightDone = 1; break;
	    }
	  }
	  if ( rightDone ) break;
	  // ai <= aj
	  B[z] = ai; z++;
	  if ( i == N+k ) { leftDone = 1; break; }
	  i++; ai = A[i];
	}

	int q;
	if ( leftDone ) {
	  // for ( q = N; q < z; q++ ) A[q] = B[q];
	  for ( q = N; q < z+N; q++ ) A[q] = B[q-N];
	} else { // rightDone
	  for ( q = N+k; i <= q; q-- ) A[q + L - k] = A[q];
	  // for ( q = N; q < z; q++ ) A[q] = B[q];
	  for ( q = N; q < z+N; q++ ) A[q] = B[q-N];
	}
    } // end merge2


void quicksortmc(void **, int, int, int, int (*)(const void*, const void*));

// calculate the median of 3
int medm(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end med

void vswapm(void **A, int N, int N3, int eq) {
  void *t;
  while ( 0 < eq ) { eq--; t = A[N]; A[N++] = A[N3]; A[N3++] = t; }
}

const int smallm = 400;

void quicksortm(void **A, int N, int M, int (*compare)(const void*, const void*)) {
  // printf("quicksortm N %i M %i L %i\n", N, M, M-N);
  int L = M - N;
  if ( L <= 0 ) return;
  if ( L < mergeLimit ) {
      mergeSortm(A, N, M, compare);
      return;
    }
  int depthLimit = 2.5 * floor(log(L));
  quicksortmc(A, N, M, depthLimit, compare);
} // end quicksortm

// Quicksort equipped with a defense against quadratic explosion;
// calling heapsort if depthlimit exhausted


void quicksortmc(void **A, int N, int M, int depthLimit, 
		 int (*compareXY)(const void*, const void*)) {
  // printf("Enter quicksortmc N: %d M: %d %d\n", N, M, depthLimit);
  // printf(" gap %d \n", M-N);

  while ( N < M ) {
    // printf("quicksortmc N: %d M %d  L %i\n", N, M, M-N);
    int L = M - N;
    if ( L <= iLimit) {
      insertionsort(A, N, M, compareXY);
      return;
    }

    if ( L < mergeLimit ) {
      mergeSortm(A, N, M, compareXY);
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
	pn = medm(A, pn, pn + d, pn + 2 * d, compareXY);
	p0 = medm(A, p0 - d, p0, p0 + d, compareXY);
	pm = medm(A, pm - 2 * d, pm - d, pm, compareXY);
      }
      p0 = medm(A, pn, p0, pm, compareXY);
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

    if ( L < smallm ) { 
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
      vswapm(A, N, I-eql, eql); 
      int M3 = J+N-N2;
      l = M2-J; r = M-M2;
      eqr = ( l < r ? l : r );
      vswapm(A, I, M-eqr+1, eqr);
      int N3 = I + (M-M2);
      int left = M3-N;
      int right = M-N3;
      if ( left <= right) {
	if ( 0 < left ) quicksortmc(A, N, M3, depthLimit, compareXY);
	N = N3;
	if ( N < M ) { continue; }
	return;
      }
      if ( 0 < right ) quicksortmc(A, N3, M, depthLimit, compareXY);
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
      dflgm(A, N, M, px, quicksortmc, depthLimit, compareXY);
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
	dflgm(A, N, M, px, quicksortmc, depthLimit, compareXY);
	return;
      }
      while ( I < J && compareXY(A[I], T) <= 0 ) { I++; }
      if ( M == I ) { // all elements are <= T, suspect bad input
	int px =  N + (L>>1); // N + L/2;
	iswap(N, px, A);
	dflgm(A, N, M, px, quicksortmc, depthLimit, compareXY);
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

    // Tail iteration
    J = I-1;
    iswap(N, J, A); // put the pivot
    if ( (I - N) < (M - J) ) { // smallest one first
      if ( N < J-1 ) quicksortmc(A, N, J-1, depthLimit, compareXY);
      N = I; 
    } else {
      if (I < M ) quicksortmc(A, I, M, depthLimit, compareXY);
      M = J-1;
    }
    /*
    if ( (I - N) < (M - J) ) { // smallest one first
      quicksortmc(A, N, J, depthLimit, compareXY);
      N = I; 
    } else {
      quicksortmc(A, I, M, depthLimit, compareXY);
      M = J;
    }
    */
  } // end of while loop
} // end of quicksortmc

