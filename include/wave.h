#include <math.h>

#ifndef M_PI
 #define M_PI (3.1415926535897932) 
#endif
#define TWOPI (2.0 * M_PI)

/* Oscillator type */
typedef struct oscil
{
	double twopiovrsr;
	double curfreq;
	double curphase;
	double incr;
} OSCIL;

typedef double (*tickfunc)(OSCIL*, double);

/* Intitialize an OSCIL */
void oscil_init(OSCIL* osc, unsigned long srate);

/* Sinewave generator */
double sinetick(OSCIL* osc, double freq);

/*Squarewave generator */
double squaretick(OSCIL* osc, double freq);

/* Downward sawtooth generator */
double sawdowntick(OSCIL* osc, double freq);

/* Upward sawtooth generator */
double sawuptick(OSCIL* osc, double freq);

/* Square with pwm */
double pwmtick(OSCIL* osc, double freq, double pwm);