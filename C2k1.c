// c:/bsd/rigel/sort/C2k1.c
// Date: Thu Mar 10 19:39:54 2022
// (C) OntoOO/ Dennis de Champeaux

// This version combines isort + dflgm + ( pivot sample + (fast loops | dflgm ) )

static const int dflgmLimit7 = 250;
static const int iLimit7 = 9;
// static const int bufSizek7 = 200;
static const int bufSizek7 = 300;

void cut2k1c(); // is called also

void cut2k1(void **A, int lo, int hi, int (*compare)()) { 
  // printf("cut2k %d %d %d\n", lo, hi, hi-lo);
  int L = hi - lo;
  int depthLimit = 2.9 * floor(log(L));
  cut2k1c(A, lo, hi, depthLimit, compare);
} // end cut2k

// calculate the median of 3
static int medq7(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end medq7

// int partitionx();
void cut2k1c(void **A, int lo, int hi, int depthLimit, 
		 int (*compareXY)(const void*, const void*)) {
  // printf("Enter cut2k1c lo: %d hi: %d %d\n", lo, hi, depthLimit);
  int bufl[bufSizek7];

  while ( lo < hi ) {
    // printf("while cut2k1c lo: %d hi: %d %d \n", lo, hi, depthLimit);
    int L = hi - lo;
    if ( L <= iLimit7) {
      insertionsort(A, lo, hi, compareXY);
      return;
    }
  
    if ( L < dflgmLimit7 ) {
      int p0 = lo + (L>>1); // lo + L/2;
      if ( 7 < L ) {
	int pn = lo;
	int pm = hi;
	// if ( 51 < L ) {
	if ( 40 < L ) {
	  int d = (L-2)>>3; // L/8;
	  pn = medq7(A, pn, pn + d, pn + 2 * d, compareXY);
	  p0 = medq7(A, p0 - d, p0, p0 + d, compareXY);
	  pm = medq7(A, pm - 2 * d, pm - d, pm, compareXY);
	}
	p0 = medq7(A, pn, p0, pm, compareXY);
      }
      dflgm(A, lo, hi, p0, cut2k1c, depthLimit, compareXY);
      return;
    }
    if ( depthLimit <= 0 ) {
      heapc(A, lo, hi, compareXY);
      return;
    }
    depthLimit--;
  
    register void *T; // pivot
    register int I = lo, J = hi; // indices
    int middlex = lo + (L>>1); // lo + L/2
    int k, lo1, hi1; // for sampling
    int probeLng = sqrt(L/7.0); // 2/3 ?
    // int probeLng = sqrt(L/6.5); // - - 
    // int probeLng = sqrt(L/6.0); // 0.99 no good
    if ( probeLng < 9 ) probeLng = 9;
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    lo1 = middlex - halfSegmentLng; //  lo + (L>>1) - halfSegmentLng;
    hi1 = lo1 + probeLng - 1;
    int offset = L/probeLng;  
  
    // assemble the mini array [lo1, hi1]
    for (k = 0; k < probeLng; k++) // iswap(lo1 + k, lo + k * offset, A);
      { int xx = lo1 + k, yy = lo + k * offset; iswap(xx, yy, A); }
    // sort this mini array to obtain good pivots
    // quicksort0c(A, lo1, hi1, depthLimit, compareXY);
    cut2k1c(A, lo1, hi1, depthLimit, compareXY);
    T = A[middlex];
    if ( compareXY(A[hi1], T) <= 0 ) {
      // give up because cannot find a good pivot      
      // dflgm is a dutch flag type of algorithm
      // int center = lo + (L>>1); // lo + L/2
      // iswap(middlex, center, A);
      // dflgm(A, lo, hi, center, cut2kc, depthLimit, compareXY);
      dflgm(A, lo, hi, middlex, cut2k1c, depthLimit, compareXY);
      return;
    }
    // move the sample to the corners
    for ( k = lo1; k <= middlex; k++ ) {
      iswap(k, I, A); I++;
    }
    I--;
    for ( k = hi1; middlex < k; k--) {
      iswap(k, J, A); J--;
    }
    J++;
  
    iswap(I, J, A); // create left value at k and right value at J2
    J++; 
    int J2 = I; I--;

    /*      L         R                              R     
      |------------]------]-------------------|[---------|
      lo           I      J2                  kJ        hi
    */
    k = J-1;

    // get the ball rolling:: (the core loop used above  throughout
    // The left segment has elements <= T
    // The right segment has elements >= T (!!!)

    int kl,idxl; 
    int bufx = bufSizek7-1;

  Left: ;
    int again = 1;
    kl = -1;
    while ( kl < bufx ) {
      while ( compareXY(T, A[++J2]) < 0 ); // stop with A[J2] <= T
      bufl[++kl] = J2;
      if ( k <= J2 ) { // reached the end
	again = 0; break; 
      }
    }

    /*      L            R                        R     
      |------------]---*---*-]-------------|[---------|
      lo           I        J2             kJ        hi
      The *-elements will be exchenged - even if it is the only one.
    */
    // swap them

    int p;
    for ( p = 0; p <= kl; p++ ) {
      idxl = bufl[p];
      I++;
      iswap(idxl, I, A); 
    }
    if (again) goto Left;

    J = I; I = J+1;

    // Done: 
    // Tail iteration
    if ( (I - lo) < (hi - J) ) { // smallest one first
      cut2k1c(A, lo, J, depthLimit, compareXY);
      lo = I; 
    } else {
      cut2k1c(A, I, hi, depthLimit, compareXY);
      hi = J;
    }
  } // end while
} // (* End of cut2kl1; *) the brackets remind that this was once, 1985, Pascal code

