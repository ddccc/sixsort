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



// /*
// Nigel version; clean up from DL from Y?
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
} // end  insertionsort




