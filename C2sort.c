// c:/bsd/rigel/sort/C2sort.c
// Date: Fri Jan 31 13:32:12 2014, 2017 Sun Mar 03 16:14:28 2019 
// Mon Jan 04 10:43:49 2021
// (C) OntoOO/ Dennis de Champeaux


const int cut2Limit =  800;
// extern int icnt; // invocation cnt

void quicksort0c();
void cut2c();
// cut2 is used as a best in class quicksort implementation 
// with a defense against quadratic behavior due to duplicates
// cut2 is a support function to call up the workhorse cut2c
void cut2(void **A, int N, int M, int (*compare)()) { 
  // printf("cut2 %d %d %d\n", N, M, M-N);
  int L = M - N;
  int depthLimit = 1 + 2.9 * floor(log(L));

  if ( L < cut2Limit ) { 
    quicksort0c(A, N, M, depthLimit, compare);
    // dflgm3(A, N, M, depthLimit, compare);
    return;
  }
  cut2c(A, N, M, depthLimit, compare);
} // end cut2

// Cut2c does 2-partitioning with one pivot.
// Cut2c invokes dflgm when trouble is encountered.
void cut2c(void **A, int N, int M, int depthLimit, int (*compareXY)()) {
  int L;
 Start:
  L = M - N + 1;
  if ( L <= 1 ) return;

  /*
  if ( L < 12 ) { // insertionsort
    insertionsort(A, N, M, compareXY);
    return;
  }
  //  */

  if ( depthLimit <= 0 ) {
    heapc(A, N, M, compareXY);
    return;
  }
  depthLimit--;

  if ( L < cut2Limit ) { 
    // dflgm3(A, N, M, depthLimit, compareXY);
    quicksort0c(A, N, M, depthLimit, compareXY);
    return;
  }
  // icnt++; // invocation cnt



  register void *T; // pivot
  register int I = N, J = M; // indices
  int middlex = N + (L>>1); // N + L/2
  void *middle;
  const int small = 3200;

  if ( L < small ) { // use 5 elements for sampling
        int sixth = (L + 1) / 6;
        int e1 = N  + sixth;
        int e5 = M - sixth;
	int e3 = middlex; // N + L/2
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
  } else { // small <= L

    int k, N1, M1; // for sampling
    // int middlex = N + (L>>1); // N + L/2

    int probeLng = sqrt(L/9);
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    int quartSegmentLng = probeLng >> 2; // probeLng/4;
    N1 = middlex - halfSegmentLng; //  N + (L>>1) - halfSegmentLng;
    M1 = N1 + probeLng - 1;
    int offset = L/probeLng;  

    // assemble the mini array [N1, M1]
    for (k = 0; k < probeLng; k++) // iswap(N1 + k, N + k * offset, A);
      { int xx = N1 + k, yy = N + k * offset; iswap(xx, yy, A); }
    // sort this mini array to obtain good pivots
    /*
    if ( probeLng < 120 ) quicksort0c(A, N1, M1, depthLimit, compareXY); else {
      // protect against constant arrays
      int p0 = N1 + (probeLng>>1);
      int pn = N1, pm = M1, d = (probeLng-3)>>3;
      pn = med(A, pn, pn + d, pn + 2 * d, compareXY);
      p0 = med(A, p0 - d, p0, p0 + d, compareXY);
      pm = med(A, pm - 2 * d, pm - d, pm, compareXY);
      p0 = med(A, pn, p0, pm, compareXY);
      if ( p0 != middlex ) iswap(p0, middlex, A); 
      dflgm(A, N1, M1, middlex, quicksort0c, depthLimit, compareXY);
    }
    */
    // quicksort0c(A, N1, M1, depthLimit, compareXY);
    cut2c(A, N1, M1, depthLimit, compareXY);

    T = middle = A[middlex];
    if ( compareXY(A[M1], middle) <= 0 ) {
      // give up because cannot find a good pivot
      // dflgm is a dutch flag type of algorithm
      void cut2c();
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
  }
  
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
	  goto Start;
	}
	cut2c(A, I, M, depthLimit, compareXY);
	M = J;
	goto Start;

} // (*  OF cut2; *) the brackets remind that this was once, 1985, Pascal code
