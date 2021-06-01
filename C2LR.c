// c:/bsd/rigel/sort/C2LR.c
// Date: Wed Jun 10 15:37:30 2020, Fri May 28 21:24:28 2021
// (C) OntoOO/ Dennis de Champeaux

const int cut2LRLimit =  700; 

const int bufSize = 200;

#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }
void cut2lrc();

void cut2lr(void **A, int N, int M, int (*compare)()) { 
  int L = M - N;
  int depthLimit = 2.9 * floor(log(L));
  if ( L < cut2LRLimit ) { 
    // dflgm3(A, N, M, depthLimit, compare);
    // quicksortmc(A, N, M, depthLimit, compare);
    cut2c(A, N, M, depthLimit, compare);
    return;
  }
  cut2lrc(A, N, M, depthLimit, compare);
} // end cut2lr


void cut2lrc1(void **A, int N, int M, int bufl[], int bufr[],
	     int depthLimit, int (*compareXY)());
void cut2lrc(void **A, int N, int M, 
	       int depthLimit, int (*compareXY)()) { 
  int bufl[bufSize];
  int bufr[bufSize];
  cut2lrc1(A, N, M, bufl, bufr, depthLimit, compareXY);
}

void cut2lrc1(void **A, int N, int M, int bufl[], int bufr[],
	       int depthLimit, int (*compareXY)()) {  
  int L;
 Start:
  // printf("cut2lrc1 N %d M %d %d\n", N, M, M-N);

  L = M - N;
  if ( L <= 0 ) return;
  if ( L < cut2LRLimit ) { 
    // dflgm3(A, N, M, depthLimit, compareXY);
    // quicksortmc(A, N, M, depthLimit, compareXY);
    cut2c(A, N, M, depthLimit, compareXY);
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
    // cut2c(A, N1, M1, depthLimit, compareXY);
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
      cut2lrc1(A, N, J-1, bufl, bufr, depthLimit, compareXY);
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
    cut2lrc1(A, J, M, bufl, bufr, depthLimit, compareXY);
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
} // (*  OF cut2lrc1; *) the brackets remind that this was once, 1985, Pascal code
