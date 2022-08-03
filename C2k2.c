// c:/bsd/rigel/sort/C2k2.c
// Date: Sun Mar 06 12:44:36 2022
// (C) OntoOO/ Dennis de Champeaux


// This version combines isort + dflgm + ( pivot sample + (fast loops | dflgm ) )

static const int dflgmLimit6 = 250;
// static const int iLimit6 = 9;

void cut2k2c(); // is called also
// cut2k2 is used as a best in class quicksort implementation 
// with a defense against quadratic behavior due to duplicates
// cut2k2 is a support function to call up the workhorse cut2k2c
void cut2k2(void **A, int lo, int hi, int (*compare)()) { 
  // printf("cut2k %d %d %d\n", lo, hi, hi-lo);
  int L = hi - lo;
  int depthLimit = 2.9 * floor(log(L));
  cut2k2c(A, lo, hi, depthLimit, compare);
} // end cut2k

/*
// calculate the median of 3
static int medq6(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end medq5
*/

void cut2k2c(void **A, int lo, int hi, int depthLimit, 
		 int (*compareXY)(const void*, const void*)) {
  // Unlike other quicksort designs, this one has only ONE
  // right moving loop at the left side that creates a partition 
  // at the left half.  Ditto at the right side.
  // It identifies elements that belong at the left side and swaps them 
  // to the front of the array. The other segments creeps to the right.
  // The trick is a very tight single loop and locality.
  // This produces a layout: LRLR. The two inner once are swapped.

  // printf("Enter cut2k2c lo: %d hi: %d %d\n", lo, hi, depthLimit);
  // printf(" gap %d \n", hi-lo);

  while ( lo < hi ) {
    // printf("while cut2k2c lo: %d hi: %d %d \n", lo, hi, depthLimit);
    int L = hi - lo;
    // printf("L %i \n", L);
    /*
    if ( L <= iLimit6) {
      insertionsort(A, lo, hi, compareXY);
      return;
    }
    if ( L < dflgmLimit6 ) {
      int p0 = lo + (L>>1); // lo + L/2;
      if ( 7 < L ) {
	int pn = lo;
	int pm = hi;
	// if ( 51 < L ) {
	if ( 40 < L ) {
	  int d = (L-2)>>3; // L/8;
	  pn = medq6(A, pn, pn + d, pn + 2 * d, compareXY);
	  p0 = medq6(A, p0 - d, p0, p0 + d, compareXY);
	  pm = medq6(A, pm - 2 * d, pm - d, pm, compareXY);
	}
	p0 = medq6(A, pn, p0, pm, compareXY);
      }
      dflgm(A, lo, hi, p0, cut2k2c, depthLimit, compareXY);
      return;
    }
    */
    if ( L < dflgmLimit6 ) {
      dflgm3(A, lo, hi, depthLimit, compareXY);
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
    cut2k2c(A, lo1, hi1, depthLimit, compareXY);
    T = A[middlex];
    if ( compareXY(A[hi1], T) <= 0 ) {
      // give up because cannot find a good pivot      
      // dflgm is a dutch flag type of algorithm
      // int center = lo + (L>>1); // lo + L/2
      // iswap(middlex, center, A);
      // dflgm(A, lo, hi, center, cut2kc, depthLimit, compareXY);
      dflgm(A, lo, hi, middlex, cut2k2c, depthLimit, compareXY);
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
  
    k = middlex; int k2 = k+1;
    /*      L         R                        L      R     
      |------------]------]------|---------[-----[---------|
      lo           I      J2     kk2       I2    J        hi
    */
    iswap(I, k, A); I--; // A[k] <= T create left value at k
    iswap(J, k2, A); J++; // T <= A[k2] create right value at k2 
    iswap(I, J, A); 
    int J2 = I; I--; int I2 = J; J++;

  Left: 
    // printf("Left I %i J2 %i k %i k2 %i I2 %i J %i \n", I, J2, k, k2, I2, J);
    /*
    { int k;
    for ( k = lo; k <= I; k++ ) 
      if ( compareXY(T, A[k]) < 0 ) {
	printf("Error Left L lo %i I %i J %i hi %i k %i\n",lo,I,J,hi,k); exit(0);
      } 
    for ( k = I+1; k <= J2; k++ ) 
      if ( compareXY(A[k], T) < 0 ) { // equal is ok 
	printf("Error Left R lo %i hi %i k %i\n", lo, hi, k); exit(0);
      } 
    }
    */
    while ( compareXY(T, A[++J2]) < 0 ); // stop with A[J2] <= T
    { int z = I+1; iswap(z, J2, A); I = z; }
    if ( J2 < k ) goto Left;
    // switch to the right side

  Right:
    while ( compareXY(A[--I2], T) <= 0); // stop with T <= A[I2]
    { int z = J-1; iswap(z, I2, A); J = z; }
    if ( k2 < I2 ) goto Right;

    /*      L         R                 L           R     
      |------------]------------|--------------[---------|
      lo           I            kk2            J        hi
    */
    if ( k-I <= J-k2 ) {
      { int z; for ( z = k; I < z; z-- ) { J--; iswap(z, J, A) } }
      I = J; J--;
    } else {
      { int z; for ( z = k2; z < J; z++ ) { I++; iswap(I, z, A) } }
      J = I; I = J+1;
    }


    // Done: 
    // printf("trace lo %i I %i J %i hi %i\n", lo, I, J, hi);
   /*
    for ( k = lo; k <= J; k++ ) 
      if ( compareXY(T, A[k]) < 0 ) {
	printf("Error Done L lo %i I %i J %i hi %i k %i\n",lo,I,J,hi,k); exit(0);
      }
    for ( k = I; k <= hi; k++ ) 
      if ( compareXY(A[k], T) < 0 ) { // equal is ok 
	printf("Error Done R lo %i hi %i k %i\n", lo, hi, k); exit(0);
      } 
    // */

    // Tail iteration
    if ( (I - lo) < (hi - J) ) { // smallest one first
      cut2k2c(A, lo, J, depthLimit, compareXY);
      lo = I; 
    } else {
      cut2k2c(A, I, hi, depthLimit, compareXY);
      hi = J;
    }
  } // end while
} // (*  OF cut2k2; *) the brackets remind that this was once, 1985, Pascal code

