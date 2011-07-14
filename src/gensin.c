#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sndfile.h>
#include <math.h>

#define DEFAULT_FILE_NAME "out.pcm"

int main(int argc, char *argv[])
{
        int i;

        float *samples;

        SNDFILE *sndfile;
        SF_INFO infos;

	/* Default: 440Hz@44100Hz@1s */
	char *file_name=DEFAULT_FILE_NAME;
	int frequency=440;
	int duration=1;
	int samplerate=44100;
	int samples_count=0;

	char *optstring="f:d:s:c:h";
	struct option longopts[]={
		{"frequency",     1, NULL, 'f'},
		{"duration",      1, NULL, 'd'},
		{"samplerate",    1, NULL, 's'},
		{"samples-count", 1, NULL, 'c'},
		{"help",          0, NULL, 'h'},
		{NULL,            0, NULL,  0 }
	};

	int longindex;
	int option;

	while ((option = getopt_long(argc, argv, optstring, longopts, &longindex)) != -1) {
		switch(option) {
			case 'f':
			if (sscanf(optarg, "%d", &frequency) != 1) {
			}
			break;
			case 'd':
			if (sscanf(optarg, "%d", &duration) != 1) {
			}
			break;
			case 's':
			if (sscanf(optarg, "%d", &samplerate) != 1) {
			}
			break;
			case 'c':
			if (sscanf(optarg, "%d", &samples_count) != 1) {
			}
			break;
			case 'h':
			fprintf(stdout, "Usage: %s --frequency[-f] [--duration[-d]|-samples-count[-c]] --samplerate[-s]\n", argv[0]);
			return 0;
		}
	}

	if (optind != argc) {
		file_name=argv[optind];
	}

        infos.format=SF_FORMAT_RAW | SF_FORMAT_PCM_16;
        infos.channels=1;
        infos.samplerate=samplerate;

	if (samples_count == 0) {
	        samples_count=infos.samplerate*duration;
	} else {
		duration=1/infos.samplerate*samples_count;
	}

	fprintf(stdout, "Parameters: frequency:%dHz duration:%ds samplerate:%dHz samples-count:%d\n", frequency, duration, samplerate, samples_count);
	fprintf(stdout, "Output file: %s\n", file_name);

        samples=malloc(samples_count*sizeof(float));

        for(i=0;i<samples_count;i++) {
                samples[i]=sin(2*M_PI*frequency*(float)(i)*1/infos.samplerate);
        }

        sndfile = sf_open(file_name, SFM_WRITE, &infos);
        sf_write_float(sndfile, &samples[0], samples_count);

        free(samples);
        sf_close(sndfile);
        return 0;
}
