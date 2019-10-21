void dflgm(void **A, int N, int M, int pivotx,
           void (*cut)(void**, int, int, int,
                       int (*)(const void*, const void*)),
           int depthLimit, int (*compareXY)(const void*, const void*)) {
  // printf("dflgm N %i M %i pivotx %i depthLimit %i\n", N,M,pivotx,depthLimit);
  /*
    Simple version of partitioning with: L/M/R
    L < pivot, M = pivot, R > pivot
   */
  register int i, j, lw, up; // indices
  register void* p3; // pivot
  register void* x, *y;
  register int r; // comparison output 
  // int z; // for tracing

  i = N; j = M; lw = pivotx-1; up = pivotx+1;
  int N2 = N+1;
    /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      
      N <= i < lw < up < j <= M
      2 <= up - lw
      N <= x < i ==> A[x] < p3
      lw < x < up  ==> A[x] = p3
      j < x <= M & ==> p3 < A[x] 
    */
  x = A[N]; // roving variable, the wack-a-mole item
  p3 = A[N] = A[pivotx]; // There IS a middle value somewhere:
  // p3 is temporarily also in A[N]; will be replaced at the end
  i++; 
  r = compareXY(x, p3);
  // if ( x < p3 ) goto L0;
  if ( r < 0 ) goto L0;
  // if ( p3 < x ) goto R0;
  if ( 0 < r ) goto R0;
  goto ML0;

 L0:  
   /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      x -> L 
   */
  if ( lw < i ) { i--;
    goto L1L;
  }
  // i <= lw
  y = A[i];
  r = compareXY(y, p3);
  if ( r < 0 ) { i++;
    goto L0; 
  }
  // 0 <= r
  A[i++] = x; x = y;
  if ( 0 < r ) { 
    goto R0;
  }
  // r = 0
  // goto ML0;

 ML0: 
   /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      x -> ML 
   */
  if ( lw < i ) { i--;
    goto L1M;
  }
  // i <= lw
  y = A[lw];
  r = compareXY(y, p3);
  if ( r == 0 ) { lw--;
    goto ML0;
  }
  A[lw--] = x; x = y;
  if ( 0 < r ) {
    goto R0;
  }
  // r < 0
  goto L0;
  
 R0:
   /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      x -> R
   */
  if ( j < up ) { // A[j] = p3 !
    y = A[j]; A[j] = x; x = y;
    goto R1M;
  }
  // up <= j
  y = A[j]; 
  r = compareXY(y, p3);
  if ( 0 < r ) { j--;
    goto R0;
  }
  A[j--] = x; x = y;
  if ( r == 0 ) {
    goto MR0;
  }
  goto L0;
  
  MR0:
   /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      x -> MR
   */
  if ( j < up ) { j++;
    goto R1M;
  }
  // up <= j
  y = A[up];
  r = compareXY(y, p3);
  if ( r == 0 ) { up++;
    goto MR0;
  }
  A[up++] = x; x = y;
  if ( r < 0 ) {
    goto L0;
  }
  goto R0;

  L1L: 
   /*
      |---]---------)-------(----|
      N   i        up       j    M
      x -> L
   */
  if ( j < up ) { j++; A[N] = x; 
    goto done; 
  }
  // up <= j
  y = A[up];
  r = compareXY(y, p3);
  if ( r == 0 ) { up++;
    goto L1L;
  }
  if ( r < 0 ) { A[up++] = A[++i]; A[i] = y;
    goto L1L;
  }
  // 0 < r
  A[up++] = A[++i]; A[i] = x; x = y;
  // goto L1R;

  L1R: 
   /*
      |---]---------)-------(----|
      N   i        up       j    M
      x -> R
   */
  if ( j < up ) { 
    A[N] = A[i]; A[i--] = A[j]; A[j] = x;
    goto done; 
  }
  // up <= j
  y = A[j];
  r = compareXY(y, p3);
  if ( 0 < r ) { j--;
    goto L1R;
  }
  A[j--] = x; x = y;
  if ( r == 0 ) {
    goto L1M;
  }
  // r < 0
  goto L1L;

 L1M: 
   /*
      |---]---------)-------(----|
      N   i        up       j    M
      x -> M
   */
  if ( j < up ) { j++; A[N] = A[i]; A[i--] = x;
    goto done;
  }
  // up <= j
  y = A[up];
  r = compareXY(y, p3);
  if ( r == 0 ) { up++;
    goto L1M;
  }
  A[up++] = x; x = y;
  if ( r < 0 ) {
    goto L1L;
  }
  goto L1R;

 R1R: 
   /*
      |---)---(-------------[----|
      N   i   lw            j    M
      x -> R
   */
  if ( lw < i ) {
    if ( i == N2 ) { i = N-1; // left empty
      A[N] = A[--j]; A[j] = x;
      goto done;      
    }
    // N2 < i
    A[N] = A[--i]; A[i--] = A[--j]; A[j] = x;
    goto done;
  }
  // i <= lw
  y = A[lw];
  r = compareXY(y, p3);
  if ( r == 0 ) { lw--;
    goto R1R;
  }
  A[lw--] = A[--j]; A[j] = x; x = y;
  if ( r < 0 ) {
    goto R1L;
  }
  goto R1R;

 R1L: 
   /*
      |---)---(-------------[----|
      N   i   lw            j    M
      x -> L
   */
  if ( lw < i ) {
    A[N] = x; i--;
    goto done;
  }
  // i <= lw
  y = A[i];
  r = compareXY(y, p3);
  if ( r < 0 ) { i++;
    goto R1L;
  }
  A[i++] = x; x = y;
  if ( r == 0 ) {
    goto R1M;
  }
  goto R1R;

 R1M: 
   /*
      |---)---(-------------[----|
      N   i   lw            j    M
      x -> M
   */
  if ( lw < i ) {
    if ( i == N2 ) { i = N-1; // left empty
      A[N] = x;
       goto done;      
    }
    // N2 < i
    A[N] = A[--i]; A[i--] = x;
    goto done;
  }
  // i <= lw
  y = A[lw];
  r = compareXY(y, p3);
  if ( r == 0 ) { lw--;
    goto R1M;
  }
  A[lw--] = x; x = y;
  if ( r < 0 ) {
    goto R1L;
  }
  goto R1R;

 done: 
    /*
      |---]---------[---------|
      N   i         j         M
    */
    /*
      for ( z = N; z <= i; z++ )
	// if ( p3 <= A[z] ) {
	if ( compareXY(p3, A[z]) <= 0 ) {
	  printf("doneL z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      for ( z = i+1; z < j; z++ )
	// if ( p3 != A[z] ) {
	if ( compareXY(p3, A[z]) != 0 ) {
	  printf("doneM z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      for ( z = j; z <= M ; z++ )
	// if ( A[z] <= p3 ) {
	if ( compareXY(A[z], p3) <= 0 ) {
	  printf("doneR z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      */
    if ( i - N  < M - j ) {
      if ( N < i ) (*cut)(A, N, i, depthLimit, compareXY);
      if ( j < M ) (*cut)(A, j, M, depthLimit, compareXY);
      return;
    }
    if ( j < M ) (*cut)(A, j, M, depthLimit, compareXY);
    if ( N < i ) (*cut)(A, N, i, depthLimit, compareXY);
} // end dflgm
