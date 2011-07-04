#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <math.h>

#define FILE_NAME "out.pcm"

int main(int argc, char *argv[])
{
        int i;

        int size;
        float *samples;

        SNDFILE *sndfile;
        SF_INFO infos;

        infos.format=SF_FORMAT_RAW | SF_FORMAT_PCM_16;
        infos.channels=1;
        infos.samplerate=44100;
        
        /* 10 s */
        int duration=10;
        size=infos.samplerate*duration;
        samples=malloc(size*sizeof(float));

        for(i=0;i<size;i++) {
                samples[i]=sin(2*M_PI*440*(float)(i)*1/infos.samplerate);
        }

        sndfile = sf_open(FILE_NAME, SFM_WRITE, &infos);
        sf_write_float(sndfile, &samples[0], size);

        free(samples);
        sf_close(sndfile);
        return 0;
}
