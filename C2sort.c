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
void cut2(void **A, int lo, int hi, int (*compare)()) { 
  // printf("cut2 %d %d %d\n", lo, hi, hi-lo);
  int L = hi - lo;
  int depthLimit = 2.9 * floor(log(L));
  cut2c(A, lo, hi, depthLimit, compare);
} // end cut2
// calculate the median of 3
int medq2(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end medq2

void cut2c(void **A, int lo, int hi, int depthLimit, 
		 int (*compareXY)(const void*, const void*)) {
  // printf("Enter cut2c lo: %d hi: %d %d\n", lo, hi, depthLimit);
  // printf(" gap %d \n", hi-lo);

  while ( lo < hi ) {
    // printf("cut2c lo: %d hi %d  L %i\n", lo, hi, hi-lo);
    int L = hi - lo;
    /*
    if ( L <= iLimit2) {
      dflgm3(A, lo, hi, depthLimit, compareXY);
      return;
    }
    */
    if ( L <= iLimit2) {
      insertionsort(A, lo, hi, compareXY);
      return;
    }

    if ( L < dflgmLimit2 ) {
      int p0 = lo + (L>>1); // lo + L/2;
      if ( 7 < L ) {
	int pn = lo;
	int pm = hi;
	// if ( 51 < L ) {
	if ( 40 < L ) {
	  int d = (L-2)>>3; // L/8;
	  pn = medq2(A, pn, pn + d, pn + 2 * d, compareXY);
	  p0 = medq2(A, p0 - d, p0, p0 + d, compareXY);
	  pm = medq2(A, pm - 2 * d, pm - d, pm, compareXY);
	}
	p0 = medq2(A, pn, p0, pm, compareXY);
      }
      dflgm(A, lo, hi, p0, cut2c, depthLimit, compareXY);
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
    void *middle;
    int k, lo1, hi1; // for sampling
    int probeLng = sqrt(L/7.0); if ( probeLng < 9 ) probeLng = 9;
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    lo1 = middlex - halfSegmentLng; //  lo + (L>>1) - halfSegmentLng;
    hi1 = lo1 + probeLng - 1;
    int offset = L/probeLng;  

    // assemble the mini array [lo1, hi1]
    for (k = 0; k < probeLng; k++) // iswap(lo1 + k, lo + k * offset, A);
      { int xx = lo1 + k, yy = lo + k * offset; iswap(xx, yy, A); }
    // sort this mini array to obtain good pivots
    // quicksort0c(A, lo1, hi1, depthLimit, compareXY);
    cut2c(A, lo1, hi1, depthLimit, compareXY);
    T = middle = A[middlex];
    if ( compareXY(A[hi1], middle) <= 0 ) {
      // give up because cannot find a good pivot
      // dflgm is a dutch flag type of algorithm
      dflgm(A, lo, hi, middlex, cut2c, depthLimit, compareXY);
      return;
    }
    for ( k = lo1; k <= middlex; k++ ) {
    iswap(k, I, A); I++;
    }
    I--;
    for ( k = hi1; middlex < k; k--) {
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
    if ( (I - lo) < (hi - J) ) { // smallest one first
      cut2c(A, lo, J, depthLimit, compareXY);
      lo = I; 
    } else {
      cut2c(A, I, hi, depthLimit, compareXY);
      hi = J;
    }
  } // end while
} // (*  OF cut2; *) the brackets remind that this was once, 1985, Pascal code
