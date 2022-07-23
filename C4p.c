// File: c:/bsd/rigel/sort/C4p.c
// Date: Sun Oct 20 14:51:33 2019
// (C) Dennis de Champeaux/ OntoOO

// static int cut4Limit4 = 500; // transition to 1-pivot
static int cut4Limit4 = 1024*200; // transition to 1-pivot

static void cut4Pc();
// cut4P is doing 4-partitioning using 3 pivots
void cut4P(void **A, int lo, int hi, int (*compare)()) {
  // printf("cut4P %d %d \n", lo, hi);
  int L = hi - lo; 
  // cut4Pc(lo, hi, 0); return; // for testing heapsort
  int depthLimit = 2.9 * floor(log(L));
  if ( L < cut4Limit4 ) {
    // quicksort0(A, lo, hi, compare); 
    cut2c(A, lo, hi, depthLimit, compare);
    return; 
  }
  cut4Pc(A, lo, hi, depthLimit, compare);
} // end cut4P

void cut4Pc(void **A, int lo, int hi, int depthLimit, int (*compareXY)()) 
{
 Start: ;
  // printf("cut4Pc %d %d  %d\n", lo, hi, depthLimit);
  int L = hi - lo + 1; 
  if ( L <= 1 ) return;
  if ( L < cut4Limit4 ) {
    // quicksort0c(A, lo, hi, depthLimit, compareXY); 
    // d4c(A, lo, hi, depthLimit, compareXY); 
    cut2c(A, lo, hi, depthLimit, compareXY); 
    return; 
  }
  if ( depthLimit <= 0 ) {
    heapc(A, lo, hi, compareXY);
    return;
  }
  depthLimit--;

  int k, lo1, hi1; // for sampling
  int maxlx, middlex,  minrx;  
  // pivots for left/ middle / right regions
  register void *maxl, *middle, *minr;
  register int i, j, lw, up, z; // indices
  i = lo; j = hi;
  z = middlex = lo + (L>>1); // lo + L/2
  /*
        Obsolete because of the size of cut4Limit
  const int small = 900; 
  if ( L < small ) { // use 5 elements for sampling
    int e1, e2, e3, e4, e5;
    e1 = maxlx = lo; e5 = minrx = hi; mrx = middlex+1;
    e3 = middlex;
    int quartSegmentLng = L >> 2; // L/4
    e2 = e3 - quartSegmentLng;
    e4 = e3 + quartSegmentLng;
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
    iswap(mrx, e4, A);
    lw = z-1; up = mrx+1;
  } else { // small <= L
    int probeLng = sqrt(5.6);
  */
    int probeLng = sqrt(L/3.0);
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    int quartSegmentLng = probeLng >> 2; // probeLng/4;
    lo1 = middlex - halfSegmentLng; //  lo + (L>>1) - halfSegmentLng;
    hi1 = lo1 + probeLng - 1;
    maxlx = lo1 + quartSegmentLng;
    // int middlex = lo1 + halfSegmentLng;
    minrx = hi1 - quartSegmentLng;
    //    mrx = middlex + (quartSegmentLng>>1);
    int offset = L/probeLng;  

    // assemble the mini array [lo1, hi1]
    for (k = 0; k < probeLng; k++) // iswap(lo1 + k, lo + k * offset, A);
    { int xx = lo1 + k, yy = lo + k * offset; iswap(xx, yy, A); }
    cut2c(A, lo1, hi1, depthLimit, compareXY);
    lw = maxlx; up = minrx;
  }

  // pivots
  maxl = A[maxlx]; middle = A[z]; minr = A[minrx];

  // check that segments can be properly initialized
  if ( compareXY(maxl, middle) == 0 || 
       compareXY(middle, minr) == 0 || 
       compareXY(minr, A[hi1]) == 0 ) {
    // no good pivots available, thus escape
    dflgm(A, lo, hi, middlex, cut4Pc, depthLimit, compareXY);
    return;
  }

//  if ( small  <= L) {
    // Swap these two segments to the corners
    for ( k = lo1; k <= maxlx; k++ ) {
      iswap(k, i, A); i++;
    }
    i--;
    for ( k = hi1; minrx <= k; k--) {
      iswap(k, j, A); j--;
    }
    j++;
//  } 


  void *x, *y; // values 
  /* The last element in x must be insert somewhere. The hole
     location is used for this task */
  int hole = lo; x = A[++i]; // x is the first element to be inserted somewhere
  A[i] = A[lo];


  // Here the general layout:
   /*   L             ML         MR             R
    |-----]------+[---------]---------]+-----[-----|
   lo     i      lw         z         up     j     hi
   */

      /* ***********
       It is actually possible that ML contains an element equal to maxl 
       and similarly an element in MR equal to minr.
       ***********
    */ 
  /* There are invariants to be maintained (which are >essential< 
     for machine assisted correctness proofs):
     maxl < middle < minr
     If there are two gaps:
       lo < x <= i --> A[x] <= maxl
       lw < x <= z  --> maxl < A[x] <= middle
       z < x < up  --> middle < A[x] < minr
       j <= x <= hi --> minr <= A[x]
     If the left gap has closed:
       lo < x < i --> A[x] <= maxl
       i <= x <= z --> maxl < A[x] <= middle
       z <  x < up  --> middle < A[x] < minr
       j <= x <= hi --> minr <= A[x]
     If the right gap has closed:
       lo < x <= i --> A[x] <= maxl
       lw < x <= z  --> maxl < A[x] <= middle
       z < x <= j  --> middle < A[x] < minr
       j < x <= hi --> minr <= A[x]
  */

  /* We employ again whack-a-mole. We know in which partition element x 
     should be.  Find a close, undecided position where x should go.  
     Store its content in y.  Drop x (simplified).  Set x to y and repeat. 
   */
  // Ready to roll ...

   /*   L             ML         MR             R
    o-----]------+[---------]---------]+-----[-----|
   lo     i      lw         z         up     j     hi
   */

  // if ( x <= middle ) {
  if ( compareXY(x, middle) <= 0 ) {
    // if ( x <= maxl ) goto TLgMLMRgRL;
    if ( compareXY(x, maxl) <= 0 ) goto TLgMLMRgRL;
    goto TLgMLMRgRML;
  }
  // middle < x
  // if ( x < minr ) goto TLgMLMRgRMR;
  if ( compareXY(x, minr) < 0 ) goto TLgMLMRgRMR;
    goto TLgMLMRgRR;


 TLgMLMRgRL:
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	  lo     i        lw     up       j     hi
           x -> L             z
	 */
	i++;
	y = A[i];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) goto TLgMLMRgRL;
	  // y -> ML
	  if ( lw < i ) {
	 /*   L                              R
	   |o----][-----------|-]+--------[-----|
	  lo      i           z  up       j     hi
           x -> L             
	 */
	    goto TLMLMRgRL;
	  }
	  A[i] = x;
	  x = y;
	  goto TLgMLMRgRML;
	}
	// middle < y
	A[i] = x;
	x = y;
	if ( compareXY(minr, y) <= 0 ) { 
	  goto TLgMLMRgRR;
	}
	goto TLgMLMRgRMR;

 TLgMLMRgRML:
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	  lo     i        lw     up       j     hi
           x -> ML            z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = x;
	      x = y;
	      lw--;
	      goto TLgMLMRgRL;
	    }
	    // left gap closed
	    i++;
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
	  lo      i              up       j     hi
           x -> ML            z
	 */
	    goto TLMLMRgRML;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLMRgRML;
	}
	// middle < y
	A[lw] = x;
	x = y;
	lw--;
	if ( compareXY(minr, y) <= 0 ) { 
	  goto TLgMLMRgRR;
	}
	goto TLgMLMRgRMR;

 TLgMLMRgRR:
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	  lo     i        lw     up       j     hi
           x -> R             z
	 */
	j--;
	y = A[j];
	if ( compareXY(y, middle) <= 0 ) {
	  A[j] = x;
	  x = y;
	  if ( compareXY(y, maxl) <= 0 ) { 
	    goto TLgMLMRgRL;
	  }
	  goto TLgMLMRgRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  goto TLgMLMRgRR;
	}
	// y -> MR
	if ( up <= j ) {
	  A[j] = x;
	  x = y;
	  goto TLgMLMRgRMR;
	}
	// right gap closed
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	  lo     i        lw          j      hi
           x -> R             z
	 */
	goto TLgMLMRRR;

 TLgMLMRgRMR:
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
          lo     i        lw     up       j     hi
           x -> MR            z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  A[up] = x;
	  x = y;
	  up++;
	  if ( compareXY(y, maxl) <= 0 ) { 
	    goto TLgMLMRgRL;
	  }	  
	  goto TLgMLMRgRML;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLgMLMRgRMR;
	}
	// y -> R
	if ( up < j ) {
	  A[up] = x;
	  x = y;
	  up++;
	  goto TLgMLMRgRR;
	}
	// right gap closed
	j--;
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	  lo     i        lw          j      hi
           x -> MR            z
	 */
	goto TLgMLMRRMR;


	// right gap closed
 TLgMLMRRR:
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	  lo     i        lw          j      hi
           x -> R             z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {  // shift ML&MR to the left
	      A[lw] = A[z]; A[z] = A[j]; A[j] = x;
	      x = y;
	      lw--; z--; j--; 
	      goto TLgMLMRRL;
	    }
	    // left gap also closed; shift ML&MR to the left
	    A[hole] = A[i]; A[i] = A[z]; A[z] = A[j]; A[j] = x;
	    i--; z--; 
	    goto Finish4;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLMRRR;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) { // shift ML&MR to the left
	  A[lw] = A[z]; A[z] = A[j]; A[j] = y;
	  lw--; z--; j--; 
	  goto TLgMLMRRR;
	}
	// y -> MR; shift ML to the left
	A[lw] = A[z]; A[z] = y;
	lw--; z--; 
	goto TLgMLMRRR;

 TLgMLMRRMR:
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	  lo     i        lw          j      hi
           x -> MR            z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = A[z]; A[z] = x;
	      x = y;
	      z--; lw--;
	      goto TLgMLMRRL;
	    }
	    // left gap also closed
	    A[hole] = y; A[lw] = A[z]; A[z] = x;
	    i--; z--; j++;
	    goto Finish4;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLMRRMR;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  A[lw] = A[z]; A[z] = A[j]; A[j] = y;
	  z--; j--; lw--;
	  goto TLgMLMRRMR;
	}
	// y -> MR
	A[lw] = A[z]; A[z] = y;
	z--; lw--;
	goto TLgMLMRRMR;

 TLgMLMRRML:
        /*   L                         R
	   |o----]------+[--|------][-----|
	  lo     i     lw          j      hi
           x -> ML          z
	 */
	y = A[lw];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    if ( i < lw ) {
	      A[lw] = x;
	      x = y;
	      lw--;
	      goto TLgMLMRRL;
	    }
	    // left gap also closed
	    A[hole] = y; // y = A[i] = A[lw]
	    A[lw] = x;
	    i--; j++;
	    goto Finish4;
	  }
	  // y -> ML
	  lw--;
	  goto TLgMLMRRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) {
	  A[lw] = A[z]; A[z] = A[j]; A[j] = y;
	  z--; j--; lw--;
	  goto TLgMLMRRML;
	}
	// y -> MR
	A[lw] = A[z]; A[z] = y;
	z--; lw--;
	goto TLgMLMRRML;

 TLgMLMRRL:
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	  lo     i        lw          j      hi
           x -> L             z
	 */
	i++;
	y = A[i];
	if ( compareXY(y, middle) <= 0) {
	  if ( compareXY(y, maxl) <= 0 ) goto TLgMLMRRL;
	  if ( i <= lw ) {
	    A[i] = x;
	    x = y;
	    goto TLgMLMRRML;
	  }
	  // left gap closed
	  A[hole] = x;
	  i--; j++;
	  goto Finish4;
	}
	// middle < y
	A[i] = x;
	x = y;
	if ( compareXY(minr, y) <= 0 ) goto TLgMLMRRR;
	// y -> MR
	goto TLgMLMRRMR;

	// left gap closed
 TLMLMRgRL:
        /*   L                               R
	   |o----][-----------|-]+--------[-----|
	  lo      i           z  up       j     hi
           x -> L             
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) { // y -> L
	    z++;
	    A[up] = A[z]; A[z] = A[i]; A[i] = y;
	    i++; up++;
	    goto TLMLMRgRL;
	  }
	  // y -> ML
	  z++;
	  A[up] = A[z]; A[z] = y;
	  up++;
	  goto TLMLMRgRL;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLMLMRgRL;
	}
	// y -> R
	if ( up < j ) {
	  z++;
	  A[up] = A[z]; A[z] = A[i]; A[i] = x;
	  x = y;
	  i++; up++;
	  goto TLMLMRgRR;
	}
	// right gap closed
	A[hole] = x; 
	i--;
	goto Finish4;

 TLMLMRgRML:
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
	  lo      i              up       j     hi
           x -> ML            z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    z++;
	    A[up] = A[z]; A[z] = A[i]; A[i] = y;
	    i++; up++;
	    goto TLMLMRgRML;
	  }
	  // y -> ML
	  z++;
	  A[up] = A[z]; A[z] = y;
	  up++;
	  goto TLMLMRgRML;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLMLMRgRML;
	}
	// y --> R
	if ( up < j ) {
	    z++;
	    A[up] = A[z]; A[z] = x;
	    x = y;
	    up++;
	    goto TLMLMRgRR;
	}
	// right gap closed
	i--;
	A[hole] = A[i]; A[i] = x;
	i--;
	goto Finish4;
	
 TLMLMRgRR:
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
	  lo      i              up       j     hi
           x -> R             z
	 */
	j--;
	y = A[j];
	if ( compareXY(y, middle) <= 0 ) {
	  A[j] = x;
	  x = y;
	  if ( compareXY(y, maxl) <= 0 ) {
	    goto TLMLMRgRL;
	  }
	  // y -> ML
	  goto TLMLMRgRML;
	}
	// middle < y
	if ( compareXY(minr, y) <= 0 ) goto TLMLMRgRR;
	// y -> MR
	if ( up <= j ) {
	  A[j] = x;
	  x = y;
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
          lo      i              up       j     hi
           x -> MR            z
	 */
	  goto TLMLMRgRMR;
	}
	// right gap closed
	i--;
	A[hole] = A[i]; A[i] = A[z]; A[z] = A[j]; A[j] = x;
	z--; i--;
	goto Finish4;

 TLMLMRgRMR:
         /*   L                              R
	   |o----][-----------|-]+--------[-----|
	  lo      i              up       j     hi
           x -> MR            z
	 */
	y = A[up];
	if ( compareXY(y, middle) <= 0 ) {
	  if ( compareXY(y, maxl) <= 0 ) {
	    z++;
	    A[up] = A[z]; A[z] = A[i]; A[i] = y;
	    i++; up++;
	    goto TLMLMRgRMR;
	  }
	  // y -> ML
	  z++;
	  A[up] = A[z]; A[z] = y;
	  up++;
	  goto TLMLMRgRMR;
	}
	// middle < y
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLMLMRgRMR;
	}
	// y --> R
	if ( up < j ) {
	  A[up] = x;
	  x = y;
	  up++;
	  goto TLMLMRgRR;
	}
	// right gap closed
	i--;
	A[hole] = A[i]; A[i] = A[z]; A[z] = x;
	i--; z--;
	goto Finish4;


	// +++++++++++++++both gaps closed+++++++++++++++++++
	int lleft, lmiddle, lright;
	// int k; // for testing

 Finish4:
	// printf("cut4P exit Finish4 lo: %d hi: %d\n", lo, hi);
         /*   L        ML         MR         R
	   |-----][----------|-----------][-----|
	  lo     i           z            j     hi
	 */
	if ( z-lo < hi-z ) {
	  /*
	  cut4P(lo, i);
	  cut4P(i+1, z);
	  if ( j-z < hi-j ) {
	    cut4P(z+1, j-1);
	    cut4P(j, hi);
	    return;
	  }
	  cut4P(j, hi);
	  cut4P(z+1, j-1);
	  */
	  if ( j-z < hi-j ) {
	    // cut4P(z+1, j-1);
	    // cut4P(j, hi);
	    addTaskSynchronized(ll, newTask(A, j, hi, depthLimit, compareXY));
	    addTaskSynchronized(ll, newTask(A, z+1, j-1, depthLimit, compareXY));
	  } else {
	    // cut4P(j, hi);
	    // cut4P(z+1, j-1);
	    addTaskSynchronized(ll, newTask(A, z+1, j-1, depthLimit, compareXY));
	    addTaskSynchronized(ll, newTask(A, j, hi, depthLimit, compareXY));
	  }
	  addTaskSynchronized(ll, newTask(A, i+1, z, depthLimit, compareXY));
	  // cut4Pc(lo, i, depthLimit, compareXY);
	  // return;
	  hi = i;
	  goto Start;
	}
	// hi-z <= z-lo
	//cut4P(z+1, j-1);
	// cut4P(j, hi);
	if ( i-lo < z-i ) {
	  // cut4P(lo, i);
	  // cut4P(i+1, z);
	  addTaskSynchronized(ll, newTask(A, lo, i, depthLimit, compareXY));
	  addTaskSynchronized(ll, newTask(A, i+1, z, depthLimit, compareXY));
	} else {
	  // cut4P(i+1, z);
	  // cut4P(lo, i);
	  addTaskSynchronized(ll, newTask(A, i+1, z, depthLimit, compareXY));
	  addTaskSynchronized(ll, newTask(A, lo, i, depthLimit, compareXY));
	}
	addTaskSynchronized(ll, newTask(A, j, hi, depthLimit, compareXY));
	// cut4Pc(z+1, j-1, depthLimit, compareXY);
	lo = z+1; hi = j-1;
	goto Start;
} // end cut4Pc
