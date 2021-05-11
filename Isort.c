// File: c:/bsd/rigel/sort/Isort
// Date: Thu Jan 30 21:54:12 2014/ Wed Jan 03 19:48:23 2018/ 
//       Mon May 10 11:29:57 2021
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
/*
void insertionsort(void **A, int N, int M, int (*compareXY)()) {
    int size = M-N +1;
    if ( size <= 1 ) return;
      int i = N;
      if ((size & 1) != 0) {   // ensure even size
	void *t = A[N], *u = A[++i];
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
} // end insertionsort

// */
/*
void check(void **A, int N, int M, int (*comp)()) {
  int i;
  for ( int i = N+1; i <= M; i++) 
    if ( comp(A[i], A[i-1]) < 0 ) {
      printf("*** check i %i\n", i);
      exit(0);
    } 
}
*/

/*  DdC version
void insertionsort(void **A, int N, int M, int (*compareXY)()) {
  if ( N == M ) return;
    int size = M-N +1;
    int i, k;
    if ((size & 1) != 0) { // deal with uneven size
      void *a0 = A[N], *a1 = A[N+1], *a2 = A[N+2];
      if ( compareXY(a1, a2) <= 0 ) { // a1 <= a2
	if ( compareXY(a0, a2) <= 0 ) { // a1 <= a2 & a0 <= a2
	  if ( compareXY(a1, a0) < 0 ) { // a1 <= a2 & a0 <= a2 & a1 < a0
	    A[N] = a1; A[N+1] = a0; 
	  } else { // a1 <= a2 & a0 <= a2 & a0 <= a1
	    // nothing
	  } 
	} else { // a1 <= a2 & a2 < a0
	  A[N] = a1; A[N+1] = a2; A[N+2] = a0;
	}
      } else { // a2 < a1
	if ( compareXY(a0, a1) <= 0 ) { // a2 < a1 & a0 <= a1
	  A[N+2] = a1;
	  if ( compareXY(a2, a0) <= 0 ) {  // a2 < a1 & a0 <= a1 & a2 <= a0
	    A[N] = a2; A[N+1] = a0; 
	  } else { // a2 < a1 & a0 <= a1 & a0 < a2
	    A[N+1] = a2;
	  }
	} else { // a2 < a1 & a1 < a0
	  A[N] = a2; A[N+2] = a0;
	}
      }
      if ( 3 == size ) {
	// check(A, N, M, compareXY);
	return;
      }
      i =  N+3;
    } else { // even size
	void *t = A[N], *u = A[N+1];
	if (compareXY(u, t) < 0) { A[N] = u; A[N+1] = t; }
	if ( 2 == size ) {
	  // check(A, N, M, compareXY);
	  return;
	}
       i = N+2;
    }

    for ( ; i < M; i++ ) {
      void *fst = A[k = i], *snd = A[++i];
      if (compareXY(fst, snd) > 0) { // snd < fst
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
    // check(A, N, M, compareXY);
} // end insertionsort
// */

// /*
// Nigel version 
void insertionsort(void **A, int N, int M, int (*compareXY)()) {
  if ( M <= N ) return;
  int size = M-N+1, i, k;

        if ((size & 1) == 0) {   // ensure even size
            void *t = A[N], *u = A[N+1];
            if (compareXY(t, u) > 0) { A[N] = u; A[N+1] = t; }
            i = N+2;
        } else
            i = N+1;
        for (; i < M; ++i) {
            void *fst = A[k = i], *snd = A[++i];
            if (compareXY(fst, snd) > 0) {
                for (; k > N; --k) {
                    void *x = A[k - 1];
                    if (compareXY(fst, x) >= 0)
                        break;
                    A[k + 1] = x;
                }
                A[k + 1] = fst;
                for (; k > N; --k) {
                    void *x = A[k - 1];
                    if (compareXY(snd, x) >= 0)
                        break;
                    A[k] = x;
                }
                A[k] = snd;
            } else {
                for (; k > N; --k) {
                    void *x = A[k - 1];
                    if (compareXY(snd, x) >= 0)
                        break;
                    A[k + 1] = x;
                }
                if (k != i)
                    A[k + 1] = snd;
                for (; k > N; --k) {
                    void *x = A[k - 1];
                    if (compareXY(fst, x) >= 0)
                        break;
                    A[k] = x;
                }
                if (k != i)
                    A[k] = fst;
            }
        }
}

// */


