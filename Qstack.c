// File: c:/bsd/rigel/sort/foursort/Qstack.c
// Date: Thu Jan 26 15:52:20 2017
// (C) OntoOO/ Dennis de Champeaux


/* Infrastructure for the parallel equipment:
  To obtain the int n field from X: ((struct task *) X)->n
  To obtain the int m field from X: ((struct task *) X)->m
  To obtain the task next field from X: ((struct task *) X)->next
  */

void *myMallocSS();

struct task {
  void **A;
  int n;
  int m;
  int dl;
  int (*compare)();
  struct task *next;
};
void **getA(struct task *t) { return ((struct task *) t)->A; }
int getN(struct task *t) { return ((struct task *) t)->n; }
int getM(struct task *t) { return ((struct task *) t)->m; }
int getDL(struct task *t) { return ((struct task *) t)->dl; }
void *getXY(struct task *t) { return ((struct task *) t)->compare; }
struct task *getNext(struct task *t) { return ((struct task *) t)->next; }

void setA(struct task *t, void **A) { ((struct task *) t)->A = A; }
void setN(struct task *t, int n) { ((struct task *) t)->n = n; }
void setM(struct task *t, int m) { ((struct task *) t)->m = m; }
void setDL(struct task *t, int dl) { ((struct task *) t)->dl = dl; }
void setXY(struct task *t, int (*compare)()) { 
  ((struct task *) t)->compare = compare; }
void setNext(struct task *t, struct task* tn) { 
  ((struct task *) t)->next = tn; }

struct task *newTask(void **A, int N, int M, 
		     int depthLimit, int (*compare)() ) {
  struct task *t = (struct task *) 
    myMallocSS("Called by: newTask()", sizeof (struct task));
  setA(t, A);
  setN(t, N); setM(t, M); setDL(t, depthLimit); setNext(t, NULL);
  setXY(t, compare);
  return t;
} // end newTask

struct stack {
  struct task *first;
  int size;
};
struct task *getFirst(struct stack *ll) { 
  return ((struct stack *) ll)->first; }
int getSize(struct stack *ll) { 
  return ((struct stack *) ll)->size; }
void setFirst(struct stack *ll, struct task *t) { 
  ((struct stack *) ll)->first = t; }
void setSize(struct stack *ll, int s) { 
  ((struct stack *) ll)->size = s; }
void incrementSize(struct stack *ll) { 
  setSize(ll, getSize(ll) + 1); }
void decrementSize(struct stack *ll) { 
  setSize(ll, getSize(ll) - 1); }
struct stack *newStack() {
  struct stack *ll = (struct stack *)
    myMallocSS("Called by: newstack()", sizeof (struct stack));
  setFirst(ll, NULL); setSize(ll, 0);
  return ll;
} // end newStack
int isEmpty(struct stack *ll) { 
  return ( NULL == getFirst(ll) ); 
} // end isEmpty
struct task *pop(struct stack *ll) {
  struct task *t = getFirst(ll);
  if ( NULL == t ) return NULL;
  setFirst(ll, getNext(t));
  decrementSize(ll);
  return t;
} // end pop
void push(struct stack *ll, struct task *t) {
  if ( !isEmpty(ll) ) setNext(t, getFirst(ll)); 
  setFirst(ll, t);
  incrementSize(ll);
} // end push
