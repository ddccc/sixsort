// c:/bsd/rigel/sort/D3sort.c
// Date: Mon Jan 04 18:42:53 2021
// (C) OntoOO/ Dennis de Champeaux

// calculate the median of 3
int med(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end med

void dflgm3();
// dflgm0 is used as the driver of a sorter using dflgm3
void dflgm0(void **A, int N, int M, int (*compareXY)(const void*, const void*)) {
    // printf("dflgm0 %d %d %d\n", N, M, M-N);
  int L = M - N;
  int depthLimit = 1 + 2.5 * floor(log(L));
  dflgm3(A, N, M, depthLimit, compareXY);
}
// extern int icnt; // invocation cnt
void dflgm3(void **A, int N, int M, 
           int depthLimit, int (*compareXY)(const void*, const void*)) {
    // Simple version of partitioning with: L/M/R
    // L < pivot, M = pivot, R > pivot
  int L;
 Again:;
  // printf("dflgm3 %i M %i depthLimit %i\n", N,M,depthLimit);
  L = M - N +1;
  if ( L <= 1 ) return;

  // if ( L < 12 ) { // insertionsort
  if ( L < 9 ) { // insertionsort
    insertionsort(A, N, M, compareXY);
    return;
  }
  if ( depthLimit <= 0 ) {
    heapc(A, N, M, compareXY);
    return;
  }
  depthLimit--;
  // icnt++; // invocation cnt
    int pivotx = N + (L>>1); // N + L/2;
    int p0 = pivotx;
    if ( 7 < L ) {
      int pn = N;
      int pm = M;
      if ( 51 < L ) {	
	int d = (L-2)>>3; // L/8;
	pn = med(A, pn, pn + d, pn + 2 * d, compareXY);
	p0 = med(A, p0 - d, p0, p0 + d, compareXY);
	pm = med(A, pm - 2 * d, pm - d, pm, compareXY);
      }
      p0 = med(A, pn, p0, pm, compareXY);
    }
    if ( pivotx != p0 ) iswap(p0, pivotx, A);

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
      if ( N < i ) dflgm3(A, N, i, depthLimit, compareXY);
      // if ( j < M ) (*cut)(A, j, M, depthLimit, compareXY);
      // return;
      N = j;
      goto Again;
    }
    if ( j < M ) dflgm3(A, j, M, depthLimit, compareXY);
    // if ( N < i ) (*cut)(A, N, i, depthLimit, compareXY);
    M = i;
    goto Again;
} // end dflgm3
