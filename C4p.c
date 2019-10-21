// File: c:/bsd/rigel/sort/C7/C4p.c
// Date: Sun Oct 20 14:51:33 2019
// (C) Dennis de Champeaux/ OntoOO

int cut4Limit4 = 500; // transition to 1-pivot

void cut4Pc();
// cut4P is doing 4-partitioning using 3 pivots
void cut4P(void **A, int N, int M, int (*compar)()) {
  // printf("cut4P %d %d \n", N, M);
  int L = M - N; 
  // cut4Pc(N, M, 0); return; // for testing heapsort
  if ( L < cut4Limit4 ) {
    quicksort0(A, N, M, compar); 
    return; 
  }
  int depthLimit = 2.5 * floor(log(L));
  cut4Pc(A, N, M, depthLimit, compar);
} // end cut4P

void cut4Pc(void **A, int N, int M, int depthLimit, int (*compareXY)()) 
{
 Start: ;
  // printf("cut4Pc %d %d  %d\n", N, M, depthLimit);
  int L = M - N + 1; 
  if ( L <= 1 ) return;
  if ( L < cut4Limit4 ) {
    // quicksort0c(A, N, M, depthLimit, compareXY); 
    d4c(A, N, M, depthLimit, compareXY); 
    return; 
  }
  if ( depthLimit <= 0 ) {
    heapc(A, N, M, compareXY);
    return;
  }
  depthLimit--;

  int k, N1, M1; // for sampling
  int maxlx, middlex, mrx, minrx;  
  // pivots for left/ middle / right regions
  register void *maxl, *middle, *minr;
  register int i, j, lw, up, z; // indices
  i = N; j = M;
  z = middlex = N + (L>>1); // N + L/2

  const int small = 900; 
  if ( L < small ) { // use 5 elements for sampling
    int e1, e2, e3, e4, e5;
    e1 = maxlx = N; e5 = minrx = M; mrx = middlex+1;
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
    int probeLng = sqrt(L);
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    int quartSegmentLng = probeLng >> 2; // probeLng/4;
    N1 = middlex - halfSegmentLng; //  N + (L>>1) - halfSegmentLng;
    M1 = N1 + probeLng - 1;
    maxlx = N1 + quartSegmentLng;
    // int middlex = N1 + halfSegmentLng;
    minrx = M1 - quartSegmentLng;
    mrx = middlex + (quartSegmentLng>>1);
    int offset = L/probeLng;  

    // assemble the mini array [N1, M1]
    for (k = 0; k < probeLng; k++) // iswap(N1 + k, N + k * offset, A);
    { int xx = N1 + k, yy = N + k * offset; iswap(xx, yy, A); }
    // sort this mini array to obtain good pivots
    if ( probeLng < 120 ) quicksort0c(A, N1, M1, depthLimit, compareXY); else {
      // protect against constant arrays
      int p0 = N1 + (probeLng>>1);
      int pn = N1, pm = M1, d = (probeLng-3)>>3;
      pn = med(A, pn, pn + d, pn + 2 * d, compareXY);
      p0 = med(A, p0 - d, p0, p0 + d, compareXY);
      pm = med(A, pm - 2 * d, pm - d, pm, compareXY);
      p0 = med(A, pn, p0, pm, compareXY);
      dflgm(A, N1, M1, p0, quicksort0c, depthLimit, compareXY);
    }

    lw = maxlx; up = minrx;
  }

  // pivots
  maxl = A[maxlx]; middle = A[z]; minr = A[minrx];

  // check that segments can be properly initialized
  if ( compareXY(maxl, middle) == 0 || 
       compareXY(middle, A[mrx]) == 0 || 
       compareXY(A[mrx], minr) == 0 ) {
    // no good pivots available, thus escape
    dflgm(A, N, M, middlex, cut4Pc, depthLimit, compareXY);
    return;
  }

  if ( small  <= L) {
    // Swap these two segments to the corners
    for ( k = N1; k <= maxlx; k++ ) {
      iswap(k, i, A); i++;
    }
    i--;
    for ( k = M1; minrx <= k; k--) {
      iswap(k, j, A); j--;
    }
    j++;
  } 


  void *x, *y; // values 
  /* The last element in x must be insert somewhere. The hole
     location is used for this task */
  int hole = N; x = A[++i]; // x is the first element to be inserted somewhere
  A[i] = A[N];


  // Here the general layout:
   /*   L             ML         MR             R
    |-----]------+[---------]---------]+-----[-----|
    N     i      lw         z         up     j     M
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
       N < x <= i --> A[x] <= maxl
       lw < x <= z  --> maxl < A[x] <= middle
       z < x < up  --> middle < A[x] < minr
       j <= x <= M --> minr <= A[x]
     If the left gap has closed:
       N < x < i --> A[x] <= maxl
       i <= x <= z --> maxl < A[x] <= middle
       z <  x < up  --> middle < A[x] < minr
       j <= x <= M --> minr <= A[x]
     If the right gap has closed:
       N < x <= i --> A[x] <= maxl
       lw < x <= z  --> maxl < A[x] <= middle
       z < x <= j  --> middle < A[x] < minr
       j < x <= M --> minr <= A[x]
  */

  /* We employ again whack-a-mole. We know in which partition element x 
     should be.  Find a close, undecided position where x should go.  
     Store its content in y.  Drop x (simplified).  Set x to y and repeat. 
   */
  // Ready to roll ...

   /*   L             ML         MR             R
    o-----]------+[---------]---------]+-----[-----|
    N     i      lw         z         up     j     M
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
	   N     i        lw     up       j     M
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
	   N      i           z  up       j     M
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
	   N     i        lw     up       j     M
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
	   N      i              up       j     M
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
	   N     i        lw     up       j     M
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
	   N     i        lw          j      M
           x -> R             z
	 */
	goto TLgMLMRRR;

 TLgMLMRgRMR:
         /*   L                              R
	   |o----]---------+[-|-]+--------[-----|
	   N     i        lw     up       j     M
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
	   N     i        lw          j      M
           x -> MR            z
	 */
	goto TLgMLMRRMR;


	// right gap closed
 TLgMLMRRR:
         /*   L                           R
	   |o----]---------+[-|-------][-----|
	   N     i        lw          j      M
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
	   N     i        lw          j      M
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
	   N     i     lw          j      M
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
	   N     i        lw          j      M
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
	   N      i           z  up       j     M
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
	   N      i              up       j     M
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
	   N      i              up       j     M
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
	   N      i              up       j     M
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
	   N      i              up       j     M
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
	// printf("cut4P exit Finish4 N: %d M: %d\n", N, M);
         /*   L        ML         MR         R
	   |-----][----------|-----------][-----|
	   N     i           z            j     M
	 */
	if ( z-N < M-z ) {
	  /*
	  cut4P(N, i);
	  cut4P(i+1, z);
	  if ( j-z < M-j ) {
	    cut4P(z+1, j-1);
	    cut4P(j, M);
	    return;
	  }
	  cut4P(j, M);
	  cut4P(z+1, j-1);
	  */
	  if ( j-z < M-j ) {
	    // cut4P(z+1, j-1);
	    // cut4P(j, M);
	    addTaskSynchronized(ll, newTask(A, j, M, depthLimit, compareXY));
	    addTaskSynchronized(ll, newTask(A, z+1, j-1, depthLimit, compareXY));
	  } else {
	    // cut4P(j, M);
	    // cut4P(z+1, j-1);
	    addTaskSynchronized(ll, newTask(A, z+1, j-1, depthLimit, compareXY));
	    addTaskSynchronized(ll, newTask(A, j, M, depthLimit, compareXY));
	  }
	  addTaskSynchronized(ll, newTask(A, i+1, z, depthLimit, compareXY));
	  // cut4Pc(N, i, depthLimit, compareXY);
	  // return;
	  M = i;
	  goto Start;
	}
	// M-z <= z-N
	//cut4P(z+1, j-1);
	// cut4P(j, M);
	if ( i-N < z-i ) {
	  // cut4P(N, i);
	  // cut4P(i+1, z);
	  addTaskSynchronized(ll, newTask(A, N, i, depthLimit, compareXY));
	  addTaskSynchronized(ll, newTask(A, i+1, z, depthLimit, compareXY));
	} else {
	  // cut4P(i+1, z);
	  // cut4P(N, i);
	  addTaskSynchronized(ll, newTask(A, i+1, z, depthLimit, compareXY));
	  addTaskSynchronized(ll, newTask(A, N, i, depthLimit, compareXY));
	}
	addTaskSynchronized(ll, newTask(A, j, M, depthLimit, compareXY));
	// cut4Pc(z+1, j-1, depthLimit, compareXY);
	N = z+1; M = j-1;
	goto Start;
} // end cut4Pc
