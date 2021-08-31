// c:/bsd/rigel/sort/Dsort.h
// Date: Sun Aug 29 20:35:55 2021
// (C) OntoOO/ Dennis de Champeaux

void dflgm(void **A, int lo, int hi, int pivotx,
           void (*cut)(void**, int, int, int,
                       int (*)(const void*, const void*)),
           int depthLimit, int (*compareXY)(const void*, const void*)) {}
