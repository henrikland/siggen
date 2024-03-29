#include <portsf.h>
#include <stdio.h>
#include <stdlib.h>
#include <wave.h>

#define SRATE 44100
#define BUFSIZE 1024

enum {ARG_PROGNAME, ARG_WAVETYPE, ARG_DUR,
	 ARG_GAIN, ARG_OUTFILE, ARG_OSCFLAG, ARG_FREQS};

int main(int argc, char* argv[])
{
	if (argc < ARG_OSCFLAG) {
		printf("\t##siggen - 3 oscillator signal generator##\n"
			"\t__________________________________________\n"
			"usage: siggen wavetype duration gain outfile "
			"(optional) -n[Number of oscs] freq1 freq2 . . . freqN\n"
			"wavetype:\n\t1 - sine\n\t2 - square\n"
			"\t3 - downward saw\n\t4 - upward saw\n");
		return 1;
	}

	PSF_PROPS props;
	OSCIL** oscs = NULL;
	double* oscfreqs = NULL;
	float* outframe = NULL;
	int ofd = -1;
	int wavetype = atoi(argv[ARG_WAVETYPE]);
	double dur;
	double gain;
	double val;
	unsigned long totalframes;
	unsigned long nframes;
	unsigned long nbufs;
	unsigned long remain;
	unsigned long i, j, k;	
	int noscs = 1;
	char* oscflag = (char*) &argv[ARG_OSCFLAG][0];
	if (oscflag && *oscflag == '-') {
		if (*(++oscflag) == 'n') {
			oscflag++;
			if (*oscflag < '1' || *oscflag > '9') {
				printf("number of oscillators must be a number from 1-9\n");
				return 1;
			}
			noscs = *oscflag - '0';
		}
	} else {
		/*	...*/
	}
	/*	TODO check that number of frequency args == noscs */

	if (psf_init()) {
		printf("failed to initialize portsf\n");
		return 1;
	}

	dur = atof(argv[ARG_DUR]);
	if (dur <= 0.0) {
		printf("duration can't be negative or 0.\n");
		return 1;
	}
	gain = atof(argv[ARG_GAIN]);
	if (gain > 1.0 || gain < 0.0) {
		printf("0.0 <= gain <= 1.0\n");
		return 1;
	}
	gain /= noscs;

	/*  The total number of frames that need
		to be written. */
	totalframes = (unsigned long) (SRATE * dur + 0.5);
	/*  The number of frames in a buffer. */
	nframes = BUFSIZE;
	/*	The number of buffers of size nframes required. */
	nbufs = totalframes / BUFSIZE;
	/*	Any remainder from the division. */
	remain = totalframes - nbufs * BUFSIZE;
	if (remain > 0) nbufs++;
	
	tickfunc tick;
	switch (wavetype) {
		case 1:
			tick = sinetick;
			break;
		case 2:
			tick = squaretick;
			break;
		case 3:
			tick = sawdowntick;
			break;
		case 4:
			tick = sawuptick;
			break;
		default:
			printf("invalid wavetype!\n");
			return 1;
	}
	
	/*	Set up a soundfile...
		TODO get chans from command line? */
	props.srate = SRATE;
	props.chans = 1;
	props.samptype = PSF_SAMP_IEEE_FLOAT;
	props.chformat = STDWAVE;
	props.format = psf_getFormatExt(argv[ARG_OUTFILE]);
	if (!props.format) {
		printf("invalid soundfile format!\n");
		return 1;
	}
	ofd = psf_sndCreate(argv[ARG_OUTFILE], &props, 
		0, 0, PSF_CREATE_RDWR);
	if (ofd < 0) {
		printf("failed to create outfile %s\n", argv[ARG_OUTFILE]);
		return 1;
	}

	/* Initialize the oscillators */
	oscs = malloc(sizeof(OSCIL*) * noscs);
	for (i = 0; i < noscs; ++i) {
		oscs[i] = malloc(sizeof(OSCIL));
		oscil_init(oscs[i], SRATE);
	}

	oscfreqs = malloc(sizeof(double) * noscs);
	for (i = 0; i < noscs; ++i) {
		if (argv[ARG_FREQS + i]) {
			oscfreqs[i] = atof(argv[ARG_FREQS + i]);
		} else {
			oscfreqs[i] = 0.0;
		}
	}

	outframe = malloc(sizeof(float) * props.chans * nframes);

	/*	For each buffer... */
	for (i = 0; i < nbufs; ++i) {
		/*	If this is the last buffer,
			we only need the remainder. */
		if (i == nbufs - 1 && remain > 0) {
			nframes = remain;
		}
		/*	Fill up the frames in the buffer... */
		for (j = 0; j < nframes; ++j) {
			val = 0.0;
			for (k = 0; k < noscs; ++k) {
				val += (float) (tick(oscs[k], oscfreqs[k]) * gain);
			}
			outframe[j] = val;
		}
		/*	...and try to write the buffer to outfile. */
		if (psf_sndWriteFloatFrames(ofd, outframe, nframes)
			< nframes) {
			printf("error writing to outfile %s!\n", argv[ARG_OUTFILE]);
			break;
		}
	}
	printf("length: %f secs\n", (float) psf_sndSize(ofd) / SRATE);
	
	/*	Cleanup... */
	if (oscs) {
		for (i = 0; i < noscs; ++i) {
			if (oscs[i]) free(oscs[i]);
		}
		free(oscs);
	}
	if (oscfreqs) free(oscfreqs);
	if (outframe) free(outframe);
	psf_sndClose(ofd);

	return 0;
}