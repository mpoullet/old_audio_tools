all: gensin fftw_demo_complex

gensin: gensin.c
	gcc -Wall -o $@ gensin.c -lm -lsndfile

fftw_demo_complex: fftw_demo_complex.c
	gcc -Wall -o $@ fftw_demo_complex.c -lfftw3 -lsndfile

clean:
	-rm -rf gensin fftw_demo_complex

.PHONY: all clean

