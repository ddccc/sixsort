// File: c:/bsd/rigel/sort/Isort
// Date: Thu Jan 30 21:54:12 2014/ Wed Jan 03 19:48:23 2018
// (C) OntoOO/ Dennis de Champeaux


// The unrolled version has been replaced by this one
// void iswap();
// int compareXY (const void *a, const void *b);
// if (a<b) then -1 else if a=b then 0 else 1 

#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }

void insertionsort(void **A, int N, int M, int (*compareXY)()) {
  if ( M <= N ) return;
  int i, j;
  for ( i = N+1; i <= M; i++ )
    for ( j = i; N < j && compareXY(A[j-1], A[j]) > 0; j-- )
      // iswap(j-1, j, A); 
      { int xx = j-1; iswap(xx, j, A); }
} // end insertionsort
