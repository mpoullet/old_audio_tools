all: gensin

gensin: gensin.c
	gcc -Wall -o $@ gensin.c -lm -lsndfile

clean:
	-rm -rf gensin

.PHONY: all clean

