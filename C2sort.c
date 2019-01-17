// c:/bsd/rigel/sort/C2sort
// Date: Fri Jan 31 13:32:12 2014, 2017
// (C) OntoOO/ Dennis de Champeaux

#ifndef C2sort 
    #define C2sort C2sort.c
    #include "Qusort.c"
#endif

const int cut2Limit = 1000;


void cut2c();
// cut2 is used as a best in class quicksort implementation 
// with a defense against quadratic behavior due to duplicates
// cut2 is a support function to call up the workhorse cut2c
void cut2(void **A, int N, int M, int (*compare)()) { 
  // printf("cut2 %d %d %d\n", N, M, M-N);
  int L = M - N;
  if ( L < cut2Limit ) { 
    quicksort0(A, N, M, compare);
    return;
  }
  int depthLimit = 2.5 * floor(log(L));
  cut2c(A, N, M, depthLimit, compare);
} // end cut2

// Cut2c does 2-partitioning with one pivot.
// Cut2c invokes dflgm when trouble is encountered.
void cut2c(void **A, int N, int M, int depthLimit, int (*compareXY)()) {
  int L;
  Start:
  if ( depthLimit <= 0 ) {
    heapc(A, N, M, compareXY);
    return;
  }
  L = M - N +1;
  if ( L < cut2Limit ) { 
    quicksort0c(A, N, M, depthLimit, compareXY);
    return;
  }
  depthLimit--;

  register void *T; // pivot
  register int I, J; // indices
  register void *AI, *AJ; // array values
  // int k;

    int sixth = (L + 1) / 6;
    int e1 = N  + sixth;
    int e5 = M - sixth;
    int e3 = N + (L>>1); // N + L/2;  // the midpoint
    int e4 = e3 + sixth;
    int e2 = e3 - sixth;
    // Sort these elements using a 5-element sorting network
    void *ae1 = A[e1], *ae2 = A[e2], *ae3 = A[e3], *ae4 = A[e4], *ae5 = A[e5];
    void *t;
    // if (ae1 > ae2) { `t = ae1; ae1 = ae2; ae2 = t; }
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

    // if ( compareXY(A[M], ae5) < 0 ) iswap(M, e5, A);
    if ( compareXY(ae5, ae3) <= 0) {
      // give up because cannot find a good pivot
      // dflgm is a dutch flag type of algorithm
      void cut2c();
      dflgm(A, N, M, e3, cut2c, depthLimit, compareXY);
      return;
    }
    iswap(e5, M, A); // right corner OK now
    // put pivot in the left corner
    iswap(N, e3, A);
    T = A[N];
	// initialize running indices
	I= N;
	J= M;

	// The left segment has elements <= T
	// The right segment has elements > T
  Left:
	while ( compareXY(A[++I], T) <= 0 ); 
	AI = A[I];
	while ( compareXY(T, A[--J]) < 0 ); AJ = A[J];
	if ( I < J ) { // swap
	  A[I] = AJ; A[J] = AI;
	  goto Left;
	}
	// Tail iteration
	if ( (I - N) < (M - J) ) { // smallest one first
	  cut2c(A, N, J, depthLimit, compareXY);
	  N = I; 
	  goto Start;
	}
	cut2c(A, I, M, depthLimit, compareXY);
	M = J;
	goto Start;

} // (*  OF cut2; *) the brackets remind that this was once Pascal code
