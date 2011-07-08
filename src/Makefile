all: gensin resample

gensin: gensin.c
	gcc -Wall -o $@ gensin.c -lm -lsndfile

resample: resample.c
	gcc -Wall -o $@ resample.c -lfftw3 -lsndfile

clean:
	-rm -rf gensin resample

.PHONY: all clean

