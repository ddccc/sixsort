all: usesixsort useparsixsort

usesixsort: UseSixSort.c SixSort.o
	gcc -O2 -Wall -Werror -o $@ $^ -lm

useparsixsort: UseParSixSort.c ParSixSort.o
	gcc -O2 -Wall -Werror -o $@ $^ -lm -lpthread

clean:
	$(RM) usesixsort useparsixsort *.o
