// c:/bsd/rigel/sort/C2LR.c
// Date: Wed Jun 10 15:37:30 2020
// (C) OntoOO/ Dennis de Champeaux

const int cut2LRLimit =  600; 

const int bufSize = 200;

#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }
void quicksort0();
void cut2lrc();
// cut2left is a support function to call up the workhorse cut2leftc
void cut2lr(void **A, int N, int M, int (*compare)()) { 
  // printf("cut2left %d %d %d\n", N, M, M-N);
  int L = M - N;
  int depthLimit = 1 + 2.9 * floor(log(L));
  if ( L < cut2LRLimit ) { 
    // dflgm3(A, N, M, depthLimit, compare);
    quicksort0c(A, N, M, depthLimit, compare);
    return;
  }
  cut2lrc(A, N, M, depthLimit, compare);
} // end cut2


// cut2leftc does 2-partitioning with one pivot.
// cut2leftc invokes dflgm when trouble is encountered.
void cut2lrc(void **A, int N, int M, 
	       int depthLimit, int (*compareXY)()) {  
  int bufl[bufSize];
  int bufr[bufSize];
  int L;
 Start:
  // printf("cut2lrc N %d M %d %d\n", N, M, M-N);

  L = M - N + 1;
  if ( L <= 1 ) return;
  /*
  if ( L < 12 ) { // insertionsort
    insertionsort(A, N, M, compareXY);
    return;
  }
  */
  if ( depthLimit <= 0 ) {
    heapc(A, N, M, compareXY);
    return;
  }
  depthLimit--;

  if ( L < cut2LRLimit ) { 
    // dflgm3(A, N, M, depthLimit, compareXY);
    quicksort0c(A, N, M, depthLimit, compareXY);
    return;
  }

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
	  dflgm(A, N, M, e3, cut2lrc, depthLimit, compareXY);
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
    // int quartSegmentLng = probeLng >> 2; // probeLng/4;
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
    cut2lrc(A, N1, M1, depthLimit, compareXY);
    T = middle = A[middlex];
    if ( compareXY(A[M1], middle) <= 0 ) {
      // give up because cannot find a good pivot
      // dflgm is a dutch flag type of algorithm
      void cut2c();
      dflgm(A, N, M, middlex, cut2lrc, depthLimit, compareXY);
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
  
  // register void *AI, *AJ; // array values
	// The left segment has elements <= T
	// The right segment has elements >= T 
        //    and at least one element > T
  int kl, kr, idxl, idxr; 
  int bufx = bufSize-1;

  /*
       |------]--------------------[------|
       N      I                    J      M
   */
Left:
  kl = kr = -1;
  while ( kl < bufx ) {
    while ( compareXY(A[++I], T) <= 0 );
    if ( J <= I ) { I = J-1; goto MopUpL; }
    bufl[++kl] = I;
  }
  /*
       |-------*--*---*-]----------[------|
       N                I          J      M
  */
  /*
  { int z; for ( z = J; z<=M; z++ ) 
		 if ( compareXY(A[z], T) < 0 ) {
		   printf("ERR4 %d %d %d", N, M, M-N);
		   printf(" Err4 J %i M %i z %i\n", J, M, z);
		   exit(0);
		 }
  }
  // */

  // kr = -1;
  while ( kr < bufx ) {
    while ( compareXY(T, A[--J]) < 0 ); 
    if ( J <= I ) { J = I+1; goto MopUpR; }
    bufr[++kr] = J;
  }

  /*
    |-------*--*---*-]----[--*--*-*-------|
    N                I    J               M
  */
  // swap them
  // printf("DD kl %i kr %i \n", kl, kr);
  while ( 0 <= kl ) {
    idxl = bufl[kl--];
    idxr = bufr[kr--];
    iswap(idxl, idxr, A); 
  }
  goto Left;

 MopUpR:
  // swap them
  while ( 0 <= kr ) {
    idxl = bufl[kl--];
    idxr = bufr[kr--];
    iswap(idxl, idxr, A); 
  }

  /*
  { int z; for ( z = J; z<=M; z++ ) 
		 if ( compareXY(A[z], T) < 0 ) {
		   printf("ERR8 %d %d %d", N, M, M-N);
		   printf(" Err8 J %i M %i z %i\n", J, M, z);
		   exit(0);
		 }
  }
  // */
MopUpL:
  /*
       |--------------*---*----*--[------|
       N                          J      M

   */
  /*
  { int z; for ( z = J; z<=M; z++ ) 
		 if ( compareXY(A[z], T) < 0 ) {
		   printf("ERR3 %d %d %d", N, M, M-N);
		   printf(" Err3 J %i M %i z %i\n", J, M, z);
		   exit(0);
		 }
  }
  // */
  // swap them
  while ( 0 <= kl ) {
    idxl = bufl[kl--];
    if ( J <= idxl ) continue;
    if ( idxl + 1 == J ) { J--; continue; }
    { int z = J-1; iswap(z, idxl, A); J = z; }
  }
    /*
    { int z; for ( z = J; z<=M; z++ ) 
		 if ( compareXY(A[z], T) < 0 ) {
		   printf("ERR6 %d %d %d", N, M, M-N);
		   printf(" Err6 %i %i z %i\n", J, M, z);
		   exit(0);
		 }
    }
    { int z; for ( z = N; z<=J-1; z++ ) 
	       if ( compareXY(T, A[z]) < 0 ) {
		   printf("ERR7 %d %d %d", N, M, M-N);
		   printf(" Err7 %i %i z %i\n", N, J-1, z);
		   exit(0);
		 }
    }
    // */
    if ( (I - N) < (M - J) ) { // smallest one first
      cut2lrc(A, N, J-1, depthLimit, compareXY);
      /*
      { int z; for ( z = N+1; z<J; z++ ) 
		 if ( compareXY(A[z], A[z-1]) < 0 ) {
		   printf("Err1 %i %i z %i\n", N, J-1, z);
		   exit(0);
		 }
      }
      // */
      N = J; 
      goto Start;
    }
    cut2lrc(A, J, M, depthLimit, compareXY);
    /*
      { int z; for ( z = J+1; z<=M; z++ ) 
		 if ( compareXY(A[z], A[z-1]) < 0 ) {
		   printf("Err2 %i %i z %i\n", J, M, z);
		   exit(0);
		 }
      }
      // */
    M = J-1;
    goto Start;


} // (*  OF cut2lrc; *) the brackets remind that this was once, 1985, Pascal code
