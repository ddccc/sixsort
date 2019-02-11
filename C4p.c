// File: c:/bsd/rigel/sort/sixsort/c4p.c
// Date: Thu Jan 26 16:26:00 2017
// (C) Dennis de Champeaux/ OntoOO

int cut4Limit = 3000;
int probeParamCut4 = 1000000;

#ifndef Qusort 
    #include "Qusort.c"
    #define Qusort Qusort.c
#endif

void cut4Pc();
// cut4P is doing 4-partitioning using 3 pivots
void cut4P(void **A, int N, int M, int (*compar)()) {
  // printf("cut4P %d %d \n", N, M);
  int L = M - N; 
  // cut4Pc(N, M, 0); return; // for testing heapsort
  if ( L < cut4Limit ) {
    // cut2f(A, N, M, compar); 
    quicksort0(A, N, M, compar); 
    return; 
  }
  int depthLimit = 2.5 * floor(log(L));
  cut4Pc(A, N, M, depthLimit, compar);
} // end cut4P

void cut4Pc(void **A, int N, int M, int depthLimit, int (*compareXY)()) 
{
 Start:
  // printf("cut4Pc %d %d  %d\n", N, M, depthLimit);
  if ( depthLimit <= 0 ) {
    heapc(A, N, M, compareXY);
    return;
  }
  int L = M - N; 
  if ( L < cut4Limit ) {
    // cut2fc(A, N, M, depthLimit, compareXY); 
    quicksort0c(A, N, M, depthLimit, compareXY); 
    return; 
  }
  depthLimit--;

  register void *maxl, *middle, *minr; // pivots for left/ middle / right regions

  int probeLng = L/ probeParamCut4;
  if ( probeLng < 20 ) probeLng = 20; // quite short indeed
  int halfSegmentLng = probeLng >> 1; // probeLng/2;
  int N1 = N + (L>>1) - halfSegmentLng;
  int M1 = N1 + probeLng - 1;
  int quartSegmentLng = probeLng >> 2; // probeLng/4;
  int maxlx = N1 + quartSegmentLng;
  int middlex = N1 + halfSegmentLng;
  int minrx = M1 - quartSegmentLng;
  int mrx = middlex + (quartSegmentLng>>1);
  int offset = L/probeLng;  

  // assemble the mini array [N1, M1]  
  int k;
  // for (k = 0; k < probeLng; k++) iswap(N1 + k, N + k * offset, A);
  for (k = 0; k < probeLng; k++) {
    int xx = N1 + k, yy = N + k * offset; iswap(xx, yy, A);
  }
  // sort this mini array to obtain good pivots
  // cut2(A, N1, M1, compareXY);
 quicksort0(A, N1, M1, compareXY); 


  if ( compareXY(A[maxlx], A[middlex]) == 0 || 
       compareXY(A[middlex], A[mrx]) == 0 || 
       compareXY(A[mrx], A[minrx]) == 0 ) {
    // no good pivots available, thus escape
    dflgm(A, N, M, middlex, cut4Pc, depthLimit, compareXY);
    return;
  }
  
  void *x, *y; // values  
  int hole;

  // iswap(N+1, maxlx, A); maxl = A[N+1]; // left pivot
  { int xx = N+1; iswap(xx, maxlx, A); maxl = A[xx]; } 
  iswap(M, minrx, A); minr = A[M]; // right pivot
  x = A[N]; // 1st roving variable element
  middle = A[N] = A[middlex]; // middle pivot
  // iswap(middlex+1, mrx, A); // init MR
  { int xx = middlex+1; iswap(xx, mrx, A); }


  register int i, j, lw, up, z; // indices

  // Here the general layout:
   /*   L             ML         MR             R
    |-----]------+[---------]---------]+-----[-----|
    N     i      lw         z         up     j     M
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

  i = N+1; j = M; z = middlex; lw = z-1; up = z+2; hole = N;

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
