// c:/bsd/rigel/sort/D3sort.c
// Date: Mon Jan 04 18:42:53 2021
// (C) OntoOO/ Dennis de Champeaux

// calculate the median of 3
int med(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end med

void dflgm3();
// dflgm0 is used as the driver of a sorter using dflgm3
void dflgm0(void **A, int N, int M, int (*compareXY)(const void*, const void*)) {
    // printf("dflgm0 %d %d %d\n", N, M, M-N);
  int L = M - N;
  int depthLimit = 1 + 2.9 * floor(log(L));
  dflgm3(A, N, M, depthLimit, compareXY);
}
// extern int icnt; // invocation cnt
void dflgm3(void **A, int N, int M, 
           int depthLimit, int (*compareXY)(const void*, const void*)) {
    // Simple version of partitioning with: L/M/R
    // L < pivot, M = pivot, R > pivot
  int L;
 Again:;
  // printf("dflgm3 %i M %i depthLimit %i\n", N,M,depthLimit);
  L = M - N +1;
  if ( L <= 1 ) return;

  // if ( L < 12 ) { // insertionsort
  if ( L < 9 ) { // insertionsort
    insertionsort(A, N, M, compareXY);
    return;
  }
  if ( depthLimit <= 0 ) {
    heapc(A, N, M, compareXY);
    return;
  }
  depthLimit--;
    int p0 = N + (L>>1); // N + L/2;
    if ( 7 < L ) {
      int pn = N;
      int pm = M;
      if ( 49 < L ) {
	int d = (L-2)>>3; // L/8;
	pn = med(A, pn, pn + d, pn + 2 * d, compareXY);
	p0 = med(A, p0 - d, p0, p0 + d, compareXY);
	pm = med(A, pm - 2 * d, pm - d, pm, compareXY);
      }
      p0 = med(A, pn, p0, pm, compareXY);
    }
    dflgm(A, N, M, p0, dflgm3, depthLimit, compareXY);

 
} // end dflgm3
