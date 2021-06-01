// c:/bsd/rigel/sort/C2sort.c
// Date: Fri Jan 31 13:32:12 2014, 2017 Sun Mar 03 16:14:28 2019 
// Mon Jan 04 10:43:49 2021, Sun May 30 14:48:25 2021
// (C) OntoOO/ Dennis de Champeaux


// This version combines isort + dflgm + ( pivot sample + (fast loops | dflgm ) )

const int dflgmLimit2 = 250;
const int iLimit2 = 9;

void cut2c();
// cut2 is used as a best in class quicksort implementation 
// with a defense against quadratic behavior due to duplicates
// cut2 is a support function to call up the workhorse cut2c
void cut2(void **A, int N, int M, int (*compare)()) { 
  // printf("cut2 %d %d %d\n", N, M, M-N);
  int L = M - N;
  int depthLimit = 2.9 * floor(log(L));
  cut2c(A, N, M, depthLimit, compare);
} // end cut2
// calculate the median of 3
int medq2(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end medq2

void cut2c(void **A, int N, int M, int depthLimit, 
		 int (*compareXY)(const void*, const void*)) {
  // printf("Enter cut2c N: %d M: %d %d\n", N, M, depthLimit);
  // printf(" gap %d \n", M-N);

  while ( N < M ) {
    // printf("cut2c N: %d M %d  L %i\n", N, M, M-N);
    int L = M - N;
    /*
    if ( L <= iLimit2) {
      dflgm3(A, N, M, depthLimit, compareXY);
      return;
    }
    */
    if ( L <= iLimit2) {
      insertionsort(A, N, M, compareXY);
      return;
    }

    if ( L < dflgmLimit2 ) {
      int p0 = N + (L>>1); // N + L/2;
      if ( 7 < L ) {
	int pn = N;
	int pm = M;
	// if ( 51 < L ) {
	if ( 40 < L ) {
	  int d = (L-2)>>3; // L/8;
	  pn = medq2(A, pn, pn + d, pn + 2 * d, compareXY);
	  p0 = medq2(A, p0 - d, p0, p0 + d, compareXY);
	  pm = medq2(A, pm - 2 * d, pm - d, pm, compareXY);
	}
	p0 = medq2(A, pn, p0, pm, compareXY);
      }
      dflgm(A, N, M, p0, cut2c, depthLimit, compareXY);
      return;
    }
    if ( depthLimit <= 0 ) {
      heapc(A, N, M, compareXY);
      return;
    }
    depthLimit--;

    register void *T; // pivot
    register int I = N, J = M; // indices
    int middlex = N + (L>>1); // N + L/2
    void *middle;
    int k, N1, M1; // for sampling
    int probeLng = sqrt(L/7.0); if ( probeLng < 9 ) probeLng = 9;
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    N1 = middlex - halfSegmentLng; //  N + (L>>1) - halfSegmentLng;
    M1 = N1 + probeLng - 1;
    int offset = L/probeLng;  

    // assemble the mini array [N1, M1]
    for (k = 0; k < probeLng; k++) // iswap(N1 + k, N + k * offset, A);
      { int xx = N1 + k, yy = N + k * offset; iswap(xx, yy, A); }
    // sort this mini array to obtain good pivots
    // quicksort0c(A, N1, M1, depthLimit, compareXY);
    cut2c(A, N1, M1, depthLimit, compareXY);
    T = middle = A[middlex];
    if ( compareXY(A[M1], middle) <= 0 ) {
      // give up because cannot find a good pivot
      // dflgm is a dutch flag type of algorithm
      dflgm(A, N, M, middlex, cut2c, depthLimit, compareXY);
      return;
    }
    for ( k = N1; k <= middlex; k++ ) {
    iswap(k, I, A); I++;
    }
    I--;
    for ( k = M1; middlex < k; k--) {
      iswap(k, J, A); J--;
    }
    J++;
 
    // get the ball rolling::
    register void *AI, *AJ; // array values
    // The left segment has elements <= T
    // The right segment has elements >= T
  Left:
    while ( compareXY(A[++I], T) <= 0 ); 
    AI = A[I];
    while ( compareXY(T, A[--J]) < 0 ); 
    AJ = A[J];
    if ( I < J ) { // swap
      A[I] = AJ; A[J] = AI;
      goto Left;
    }
    // Tail iteration
    if ( (I - N) < (M - J) ) { // smallest one first
      cut2c(A, N, J, depthLimit, compareXY);
      N = I; 
    } else {
      cut2c(A, I, M, depthLimit, compareXY);
      M = J;
    }
  } // end while
} // (*  OF cut2; *) the brackets remind that this was once, 1985, Pascal code
