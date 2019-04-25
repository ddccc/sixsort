// File: c:/bsd/rigel/sort/Isort
// Date: Thu Jan 30 21:54:12 2014/ Wed Jan 03 19:48:23 2018
// (C) OntoOO/ Dennis de Champeaux


#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }

// /*
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


/*
// void insertionsort(void * a[], int lo, int size, int (*compareXY)()) {
void insertionsort(void * a[], int lo, int M, int (*CPR)()) {
  int size = M-lo +1;
  if (size > 1) {
    int i = lo;
    if ((size & 1) != 0) {   // ensure even size
      void * t = a[lo], *u = a[++i];
      if (CPR(t, u) < 0) { a[lo] = u; a[i] = t; }
    }
    int end, k;
    for (end = lo + size - 1, k; i < end; ++i) {
      void *fst = a[k = i], *snd = a[++i];
      if (CPR(fst, snd) > 0) {
        for (; k > lo; --k) {
          void * x = a[k - 1];
          if (CPR(fst, x) >= 0)
            break;
          a[k + 1] = x;
        }
        a[k + 1] = fst;
        for (; k > lo; --k) {
          void * x = a[k - 1];
          if (CPR(snd, x) >= 0)
            break;
          a[k] = x;
        }
        a[k] = snd;
      } else {
        for (; k > lo; --k) {
          void * x = a[k - 1];
          if (CPR(snd, x) >= 0)
            break;
          a[k + 1] = x;
        }
        if (k != i)
          a[k + 1] = snd;
        for (; k > lo; --k) {
          void * x = a[k - 1];
          if (CPR(fst, x) >= 0)
            break;
          a[k] = x;
        }
        if (k != i)
          a[k] = fst;
      }
    }
  }
}
// */


