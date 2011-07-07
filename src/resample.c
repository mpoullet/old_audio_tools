#include <stdio.h>
#include <sndfile.h>
#include <fftw3.h>

#define BUFFER_SIZE 4096

#define IN_FILE "in.pcm"
#define OUT_FILE "out.pcm"

int main(int argc, char *argv[])
{
unsigned int r=0, i=0, j=0;
float inv_N;

SNDFILE *sfIn, *sfOut;
SF_INFO infos;

infos.format=SF_FORMAT_RAW | SF_FORMAT_PCM_16;
infos.channels=1;
infos.samplerate=44100;

fftw_complex *data, *fft_result, *ifft_result;
fftw_plan plan_forward, plan_backward;

data        = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*BUFFER_SIZE*2);
fft_result  = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*BUFFER_SIZE*2);
ifft_result = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*BUFFER_SIZE*2);

plan_forward = fftw_plan_dft_1d(BUFFER_SIZE, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
plan_backward = fftw_plan_dft_1d(BUFFER_SIZE, fft_result, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE);

printf("data=%p fft_result=%p ifft_result=%p\n", data, fft_result, ifft_result);

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

inv_N = 1./(float)BUFFER_SIZE;

do {
	/* Read a sample */
	r=sf_read_double(sfIn, &(data[i][0]), 1);
	/* Set the imaginary part to 0 */
	data[i][1]=0.0;
	//printf("%d: [ %2.3f ][ %2.3f ]\n", i, data[i][0], data[i][1]);

	/* Data block read */
	if(i==BUFFER_SIZE) 
	{
		printf("Block read...\n");

		/* FFT */
		fftw_execute(plan_forward);
		for(j=0;j<BUFFER_SIZE;j++) {
			//printf("FFT   : [ %2.3f ] [ %2.3f ]\n", fft_result[j][0], fft_result[j][1]);
		}
		
		/* IFFT */
		fftw_execute(plan_backward);

		/* Normalization */
		for(j=0;j<BUFFER_SIZE;j++) {
			ifft_result[j][0]=inv_N*ifft_result[j][0];
			//printf("IFFT  : [ %2.3f ] [ %2.3f ]\n", ifft_result[j][0], ifft_result[j][1]);
		}

		/* Write samples */
		for(j=0;j<BUFFER_SIZE;j++) {
			sf_write_double(sfOut, &(ifft_result[j][0]), 1);
		}

		/* Reset block counter */
		i=0;
	} else {
		/* Iterate block counter */
		i++;
	}
} while(r);

/* Last samples */
plan_forward = fftw_plan_dft_1d(i, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
plan_backward = fftw_plan_dft_1d(i, fft_result, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE);
fftw_execute(plan_forward);
fftw_execute(plan_backward);
for(j=0;j<i;j++) {
	ifft_result[j][0]=inv_N*ifft_result[j][0];
	sf_write_double(sfOut, &(ifft_result[j][0]), 1);
}
printf("Done...\n");

sf_close(sfIn);
sf_close(sfOut);

fftw_destroy_plan(plan_forward);
fftw_destroy_plan(plan_backward);

printf("data=%p fft_result=%p ifft_result=%p\n", data, fft_result, ifft_result);
fftw_free(data);
fftw_free(fft_result);
fftw_free(ifft_result);

return 0;
}
