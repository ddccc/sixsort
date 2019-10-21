all: usesixsort useparsixsort

usesixsort: UseSixSort.c SixSort.o
	gcc -O3 -Wall -Werror -o $@ $^ -lm

useparsixsort: UseParSixSort.c ParSixSort.o
	gcc -O3 -Wall -Werror -o $@ $^ -lm -lpthread

clean:
	$(RM) usesixsort useparsixsort *.o
