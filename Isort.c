// File: c:/bsd/rigel/sort/Isort
// Date: Thu Jan 30 21:54:12 2014/ Wed Jan 03 19:48:23 2018
// (C) OntoOO/ Dennis de Champeaux


#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }

/*
void insertionsort(void **A, int N, int M, int (*compareXY)()) {
  if ( M <= N ) return;
  int i, j;
  for ( i = N+1; i <= M; i++ )
     for ( j = i; N < j && compareXY(A[j-1], A[j]) > 0; j-- ) 
      // iswap(j-1, j, A); 
       { int xx = j-1; iswap(xx, j, A); }
} // end insertionsort
// */ 


// The version below was done by Doug Lea.
// It is not competitive for the range [2-7] in our applications
/*
 * Insertion sorting where two adjacent elements are inserted per
 * iteration: first the larger then smaller, but starting from the
 * position of the larger. If size is odd, first inserts one item.
 *
 * @param a the array
 * @param lo base index of subarray
 * @param size (sub)array size
 * @param cpr comparator
 */
// /*
void insertionsort(void **A, int N, int M, int (*compareXY)()) {
    int size = M-N +1;
    if (size > 1) {
      int i = N;
      if ((size & 1) != 0) {   // ensure even size
	void * t = A[N], *u = A[++i];
	if (compareXY(t, u) < 0) { A[N] = u; A[i] = t; }
      }
      int end, k;
      for (end = N + size - 1; i < end; ++i) {
	void *fst = A[k = i], *snd = A[++i];
	if (compareXY(fst, snd) > 0) {
	  for (; k > N; --k) {
	    void * x = A[k - 1];
	    if (compareXY(fst, x) >= 0)
	      break;
	    A[k + 1] = x;
	  }
	  A[k + 1] = fst;
	  for (; k > N; --k) {
	    void * x = A[k - 1];
	    if (compareXY(snd, x) >= 0)
	      break;
	    A[k] = x;
	  }
	  A[k] = snd;
	} else {
	  for (; k > N; --k) {
	    void * x = A[k - 1];
	    if (compareXY(snd, x) >= 0)
	      break;
	    A[k + 1] = x;
	  }
	  if (k != i)
	    A[k + 1] = snd;
	  for (; k > N; --k) {
	    void * x = A[k - 1];
	    if (compareXY(fst, x) >= 0)
	      break;
	    A[k] = x;
	  }
	  if (k != i)
	    A[k] = fst;
	}
      }
    }
}

// */


