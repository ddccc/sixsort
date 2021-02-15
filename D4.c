// c:/bsd/rigel/sort/CC/D4.c
// Date: Wed May 15 15:50:09 2019
// (C) OntoOO/ Dennis de Champeaux

// d4 is a quicksort to be applied only on small arrays; the generated partitions
// or not ordered for recursive processing; the right one is handled by tail iteration.

/* 
// calculate the median of 3
int med(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end med
// */

void d4c(void **, int, int, int, int (*)(const void*, const void*));

void d4(void **A, int N, int M, int (*compare)(const void*, const void*)) {
  //  printf("d4 N %i M %i L %i\n", N, M, M-N);
  int L = M - N;
  if ( L <= 0 ) return;
  int depthLimit = 2.9 * floor(log(L));
  d4c(A, N, M, depthLimit, compare);
} // end d3

void d4c(void **A, int N, int M, int depthLimit, int (*compare)(const void*, const void*)) {
  while ( N < M ) {
    // printf("d4c N: %d M %d  L %i\n", N, M, M-N);
    int L = 1 + M - N;
    if ( L < 11 ) {
      insertionsort(A, N, M, compare);
      return;
    }
    if ( depthLimit <= 0 ) {
      heapc(A, N, M, compare);
      return;
    }
    depthLimit--;
    int middlex = N + (L>>1); // N + L/2;
    {
      int p0 = middlex;
      //  if ( 8 < L ) {
	int pn = N;
	int pm = M;
	if ( 51 < L ) {
	  int d = (L-2)>>3; // L/8;
	  pn = med(A, pn, pn + d, pn + 2 * d, compare);
	  p0 = med(A, p0 - d, p0, p0 + d, compare);
	  pm = med(A, pm - 2 * d, pm - d, pm, compare);
	}
	p0 = med(A, pn, p0, pm, compare);
	// }
      if ( p0 != N ) iswap(p0, N, A);
    }
    void *p = A[N]; // pivot
    int i = N+1, j = M;
    void *ai, *aj;
    /*
      |------)----------------(--------|
      N      i                j        M
      N <= x < i => A[x] <= p
      j < x <= M => p <= A[x] 
    */
    while ( 1 ) { // i <= j
      aj = A[j];
      while ( compare(p, aj) <= 0 ) {
	if ( --j < i ) goto GapClosed; 
	aj = A[j]; 
      }
      // aj -> L

      if (i == j) { i++; goto GapClosed; }
      ai = A[i];
      while (  compare(p, ai) >= 0 ) {
	if ( j < ++i ) goto GapClosed;
	ai = A[i];
      }
      // ai -> R

      if ( i < j ) {
	A[i++] = aj; A[j--] = ai;
	if ( i <= j ) continue;
      }
      // j < i
      break;
    }

  GapClosed: ; // j+1 = i


    //  printf("GapClosed N %i n2 %i j %i i %i m2 %i M %i\n", N, n2, j, i, m2, M);


    /*
      |-------------][-----------------|
      N             ji                 M
    */
    /*
    int k; // for tracing
      for ( k = N; k <= n2; k++ )
	if ( compare(p, A[k]) != 0 ) {
	  printf("ErrX ML k %i\n", k);
	  // printf("N %i n2 %i j %i i %i m2 %i M %i\n", N, n2, j, i, m2, M);
	  exit(0);
	}
      for ( k = n2+1; k <= j; k++ )
	if ( compare(p, A[k]) <= 0 ) {
	  printf("ErrX L k %i\n", k);
	  // printf("N %i n2 %i j %i i %i m2 %i M %i\n", N, n2, j, i, m2, M);
	  exit(0);
	}
     for ( k = i; k < m2; k++ )
       if ( compare(A[k], p) <= 0 ) {
	  printf("ErrX R k %i\n", k);
	  // printf("N %i n2 %i j %i i %i m2 %i M %i\n", N, n2, j, i, m2, M);
	  exit(0);
       }
     for ( k = m2; k <= M; k++ )
	if ( compare(p, A[k]) != 0 ) {
	  printf("ErrX MR k %i\n", k);
	  // printf("N %i n2 %i j %i i %i m2 %i M %i\n", N, n2, j, i, m2, M);
	  exit(0);
	}
     //  */
    iswap(N, j, A);
    j--;
    if ( N < j ) d4c(A, N, j, depthLimit, compare);
    N = i;
  } // end while ( N < M ) 
}  // end d4c


