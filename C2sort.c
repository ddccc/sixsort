// c:/bsd/rigel/sort/C2sort.c
// Date: Fri Jan 31 13:32:12 2014, 2017 Sun Mar 03 16:14:28 2019
// (C) OntoOO/ Dennis de Champeaux

const int cut2Limit =  900; 

void cut2c();
// cut2 is used as a best in class quicksort implementation 
// with a defense against quadratic behavior due to duplicates
// cut2 is a support function to call up the workhorse cut2c
void cut2(void **A, int N, int M, int (*compare)()) { 
  // printf("cut2 %d %d %d\n", N, M, M-N);
  int L = M - N;
  if ( L < cut2Limit ) { 
    quicksort0(A, N, M, compare);
    return;
  }
  int depthLimit = 2.5 * floor(log(L));
  cut2c(A, N, M, depthLimit, compare);
} // end cut2

// Cut2c does 2-partitioning with one pivot.
// Cut2c invokes dflgm when trouble is encountered.
void cut2c(void **A, int N, int M, int depthLimit, int (*compareXY)()) {
  int L;
 Start:
  if ( depthLimit <= 0 ) {
    heapc(A, N, M, compareXY);
    return;
  }
  L = M - N +1;
  if ( L < cut2Limit ) { 
    quicksort0c(A, N, M, depthLimit, compareXY);
    return;
  }
  depthLimit--;

  int k, N1, M1; // for sampling
  int middlex = N + (L>>1); // N + L/2

  int probeLng = sqrt(L);
  int halfSegmentLng = probeLng >> 1; // probeLng/2;
  int quartSegmentLng = probeLng >> 2; // probeLng/4;
  N1 = middlex - halfSegmentLng; //  N + (L>>1) - halfSegmentLng;
  M1 = N1 + probeLng - 1;
  int offset = L/probeLng;  

  // assemble the mini array [N1, M1]
  for (k = 0; k < probeLng; k++) // iswap(N1 + k, N + k * offset, A);
    { int xx = N1 + k, yy = N + k * offset; iswap(xx, yy, A); }
  // sort this mini array to obtain good pivots
  quicksort0(A, N1, M1, compareXY); 

  void *middle = A[middlex];
  
  if ( compareXY(A[M1], middle) <= 0 ) {
    // give up because cannot find a good pivot
    // dflgm is a dutch flag type of algorithm
    void cut2c();
    dflgm(A, N, M, middlex, cut2c, depthLimit, compareXY);
    return;
  }
  register void *T = middle; // pivot
  register int I = N, J = M; // indices
  for ( k = N1; k <= middlex; k++ ) {
    iswap(k, I, A); I++;
  }
  I--;
  for ( k = M1; middlex < k; k--) {
    iswap(k, J, A); J--;
  }
  J++;
 
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
	if ( (I - N) < (M - J) ) { // smallest one first
	  cut2c(A, N, J, depthLimit, compareXY);
	  N = I; 
	  goto Start;
	}
	cut2c(A, I, M, depthLimit, compareXY);
	M = J;
	goto Start;

} // (*  OF cut2; *) the brackets remind that this was once, 1985, Pascal code
