// c:/bsd/rigel/sort/C2LR.h
// Date: Sun Aug 29 18:54:23 2021
// (C) OntoOO/ Dennis de Champeaux

const int cut2LRLimit =  700; 

const int bufSize = 200;

#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }

void cut2lr(void **A, int lo, int hi, int (*compare)()){}
