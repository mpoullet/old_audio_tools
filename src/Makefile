all: gensin fftw_demo_complex fftw_demo_real

gensin: gensin.c
	gcc -Wall -o $@ gensin.c -lm -lsndfile

fftw_demo_complex: fftw_demo_complex.c
	gcc -Wall -o $@ fftw_demo_complex.c -lfftw3 -lsndfile

fftw_demo_real: fftw_demo_real.c
	gcc -Wall -o $@ fftw_demo_real.c -lfftw3 -lsndfile

clean:
	-rm -rf gensin fftw_demo_complex fftw_demo_real

.PHONY: all clean

