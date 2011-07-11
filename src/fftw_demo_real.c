#include <stdio.h>
#include <sndfile.h>
#include <fftw3.h>

#define BLOCK_LEN 4096
#define COMPLEX_BLOCK_LEN ((BLOCK_LEN+1)/2+1)

#define IN_FILE "in.pcm"
#define OUT_FILE "out.pcm"

#define DEBUG 0
#if DEBUG
#define IN_FILE_TXT "in.txt"
#define OUT_FILE_TXT "out.txt"
#endif

int main(int argc, char *argv[])
{
unsigned int r=0, i=0, j=0;
unsigned int block_cnt=0, samples_cnt=0;
unsigned int padding=0;
float inv_N;

#if DEBUG
FILE *fIn, *fOut;
#endif

SNDFILE *sfIn, *sfOut;
SF_INFO infos;

infos.format=SF_FORMAT_RAW | SF_FORMAT_PCM_16;
infos.channels=1;
infos.samplerate=44100;

/* Input: real data */
double *data;
/* After FFT: complex data */
fftw_complex *fft_result;
/* Output: real data */
double *ifft_result;

fftw_plan plan_forward, plan_backward;

data        = (double*)fftw_malloc(sizeof(double)*BLOCK_LEN);
fft_result  = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*COMPLEX_BLOCK_LEN);
ifft_result = (double*)fftw_malloc(sizeof(double)*BLOCK_LEN);

plan_forward = fftw_plan_dft_r2c_1d(BLOCK_LEN, data, fft_result, FFTW_ESTIMATE);
plan_backward = fftw_plan_dft_c2r_1d(BLOCK_LEN, fft_result, ifft_result, FFTW_ESTIMATE);

sfIn = sf_open(IN_FILE, SFM_READ, &infos);
if ( sfIn==NULL ) {
	fprintf(stderr, "%d: %s\n", sf_error(sfIn), sf_strerror(sfIn));
	return -1;
}

sfOut = sf_open(OUT_FILE, SFM_WRITE, &infos);
if ( sfOut==NULL ) {
	fprintf(stderr, "%d: %s\n", sf_error(sfOut), sf_strerror(sfOut));
	return -1;
}

#if DEBUG
fIn = fopen("in.txt", "w");
if ( fIn == NULL ) {
	perror("Error");
	return -1;
}

fOut = fopen("out.txt", "w");
if ( fOut == NULL ) {
	perror("Error");
	return -1;
}
#endif

inv_N = 1./(float)BLOCK_LEN;

do {
	/* Try to read a sample */
	r=sf_read_double(sfIn, &(data[i]), 1);

	/* Sample read */
	if (r>0) {
		/* Iterate Samples counter */
		samples_cnt++;
		/* Iterate samples/block counter */
		i++;
#if DEBUG
		fprintf(fIn, "[ %f ]\n", data[i]);
#endif
	} else {
		/* Still some unprocessed samples */
		if (i>0) {
			padding=BLOCK_LEN-i;
			printf("Unprocessed samples: %d, zero padding with %d samples\n", i, padding);
			for(j=0;j<padding;j++) {
				/* Zero padding */
				data[j+i]=0;
			}
		}
	}

	/* Data block ready or last block */
	if ((i==BLOCK_LEN) || ( (i>0) && (r==0) ))
	{
		block_cnt++;
		printf("Block read...\n");

		/* FFT */
		fftw_execute(plan_forward);
		
		/* IFFT */
		fftw_execute(plan_backward);

		/* Normalization */
		for(j=0;j<i;j++) {
			ifft_result[j]=inv_N*ifft_result[j];
#if DEBUG
			fprintf(fOut, "[ %f ]\n", ifft_result[j]);
#endif
		}

		/* Write samples */
		for(j=0;j<i;j++) {
			sf_write_double(sfOut, &(ifft_result[j]), 1);
		}

		/* Reset block counter */
		i=0;
	}
} while(r);

printf("Done: samples=%d blocks=%d\n", samples_cnt, block_cnt);

#if DEBUG
fclose(fIn);
fclose(fOut);
#endif

sf_close(sfIn);
sf_close(sfOut);

fftw_destroy_plan(plan_forward);
fftw_destroy_plan(plan_backward);

fftw_free(data);
fftw_free(fft_result);
fftw_free(ifft_result);

return 0;
}
