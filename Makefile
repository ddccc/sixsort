all: usesixsort useparsixsort

usesixsort: UseSixSort.c SixSort.o
	gcc -Wall -Werror -lm -o $@ $^

useparsixsort: UseParSixSort.c ParSixSort.o
	gcc -Wall -Werror -lm -lpthread -o $@ $^

clean:
	$(RM) usesixsort useparsixsort *.o
