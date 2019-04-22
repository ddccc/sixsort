// File: c:/bsd/rigel/sort/Isort
// Date: Thu Jan 30 21:54:12 2014/ Wed Jan 03 19:48:23 2018
// (C) OntoOO/ Dennis de Champeaux

// Adjusted version from Doug Lea


#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }

void insertionsort(void **A, int N, int M, int (*compareXY)()) {
  if ( M <= N ) return;
  int i, j;
  for ( i = N+1; i <= M; i++ )
    for ( j = i; N < j && compareXY(A[j-1], A[j]) > 0; j-- )
      // iswap(j-1, j, A); 
      { int xx = j-1; iswap(xx, j, A); }
} // end insertionsort


/*
void insertionsort(void **a, int lo, int hi,
	    int (*cpr)(const void *, const void *)) {
  if ( lo <= hi ) return;
  int i = lo;
  if (((hi - lo) & 1) != 0) {   // ensure even size
    void * t = a[lo], *u = a[++i];
    if (cpr(t, u) < 0) { a[lo] = u; a[i] = t; }
  }
  int end, k;
  for ( end = hi - 1, k; (k = i) < end; i += 2) {
    void * x = a[k - 1], *sml = a[k], *lrg = a[k + 1];
    int c = cpr(lrg, sml);
    if (c < 0) { void * t = sml; sml = lrg; lrg = t; }
    while (k > lo && cpr(lrg, x) < 0) {
      a[k + 1] = x;
      x = a[--k - 1];
    }
    if (k != i || c < 0)        // avoid unnecessary writes
      a[k + 1] = lrg;
    while (k > lo && cpr(sml, x) < 0) {
      a[k] = x;
      x = a[--k - 1];
    }
    if (k != i || c < 0)
      a[k] = sml;
  }

} // end insertionsort
*/
