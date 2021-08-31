// c:/bsd/rigel/sort/Hsort.h
// Date: Sun Aug 29 20:33:21 2021
// (C) OntoOO/ Dennis de Champeaux

/*
void iswap(int p, int q, void **A) {
  void *t = A[p];
  A[p] = A[q];
  A[q] = t;
} // end of iswap
*/

// #define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }

void heapc(void **A, int lo, int hi, int (*compare)(const void*, const void*)) {}

